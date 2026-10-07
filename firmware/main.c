/* Bare-metal firmware driving the toy I2C master -> toy I2C slave. */
#include <stdint.h>

#define UART_THR  (*(volatile uint8_t *)0x10000000UL)

#define I2C_BASE  0x10100000UL
#define I2C_DATA  (*(volatile uint32_t *)(I2C_BASE + 0x00))
#define I2C_CMD   (*(volatile uint32_t *)(I2C_BASE + 0x04))
#define I2C_STAT  (*(volatile uint32_t *)(I2C_BASE + 0x08))

#define CMD_START 1
#define CMD_WRITE 2
#define CMD_READ  3
#define CMD_STOP  4
#define ST_BUSY   (1u << 0)
#define ST_NACK   (1u << 1)
#define ST_DONE   (1u << 2)

#define SLAVE_ADDR 0x42

static void uart_init(void)
{
    *(volatile uint8_t *)0x10000003UL = 0x03;   /* LCR: 8N1, DLAB off */
}

static void putc_(char c)
{
    while (!(*(volatile uint8_t *)0x10000005UL & 0x20)) ;   /* LSR.THRE */
    UART_THR = (uint8_t)c;
}
static void puts_(const char *s) { while (*s) putc_(*s++); }
static void puthex(uint8_t v)
{
    const char *d = "0123456789abcdef";
    puts_("0x"); putc_(d[v >> 4]); putc_(d[v & 15]);
}

/* Returns 0 on ACK, -1 on NACK. */
static int i2c_cmd(uint32_t cmd, uint32_t data)
{
    I2C_DATA = data;
    I2C_CMD = cmd;
    while (!(I2C_STAT & ST_DONE)) ;
    int nack = (I2C_STAT & ST_NACK) != 0;
    I2C_STAT = ST_DONE | ST_NACK;   /* write-1-to-clear */
    return nack ? -1 : 0;
}

static int i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t val)
{
    int rc = i2c_cmd(CMD_START, addr << 1);
    if (!rc) rc = i2c_cmd(CMD_WRITE, reg);
    if (!rc) rc = i2c_cmd(CMD_WRITE, val);
    i2c_cmd(CMD_STOP, 0);
    return rc;
}

static int i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *val)
{
    int rc = i2c_cmd(CMD_START, addr << 1);              /* write phase */
    if (!rc) rc = i2c_cmd(CMD_WRITE, reg);
    if (!rc) rc = i2c_cmd(CMD_START, (addr << 1) | 1);   /* repeated START */
    if (!rc) { i2c_cmd(CMD_READ, 0); *val = I2C_DATA & 0xff; }
    i2c_cmd(CMD_STOP, 0);
    return rc;
}

int main(void)
{
    uint8_t v;
    int fails = 0;

    uart_init();
    puts_("i2c testdrive firmware\n");

    if (i2c_read_reg(SLAVE_ADDR, 0x00, &v)) { puts_("NACK on WHO_AM_I\n"); return 1; }
    puts_("WHO_AM_I = "); puthex(v); puts_(v == 0xA5 ? " OK\n" : " FAIL\n");
    fails += v != 0xA5;

    i2c_write_reg(SLAVE_ADDR, 0x01, 0x5a);
    i2c_read_reg(SLAVE_ADDR, 0x01, &v);
    puts_("SCRATCH  = "); puthex(v); puts_(v == 0x5a ? " OK\n" : " FAIL\n");
    fails += v != 0x5a;

    uint8_t c0, c1;
    i2c_read_reg(SLAVE_ADDR, 0x02, &c0);
    i2c_read_reg(SLAVE_ADDR, 0x02, &c1);
    puts_("COUNTER  = "); puthex(c0); puts_(" then "); puthex(c1);
    puts_(c1 == (uint8_t)(c0 + 1) ? " OK\n" : " FAIL\n");
    fails += c1 != (uint8_t)(c0 + 1);

    if (i2c_cmd(CMD_START, 0x55 << 1) == -1) puts_("absent addr 0x55: NACK OK\n");
    else { puts_("absent addr 0x55: unexpected ACK FAIL\n"); fails++; }
    i2c_cmd(CMD_STOP, 0);

    puts_(fails ? "RESULT: FAIL\n" : "RESULT: PASS\n");
    return fails;
}
