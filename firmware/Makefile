CROSS ?= riscv64-unknown-elf-
CFLAGS = -march=rv64imac -mabi=lp64 -mcmodel=medany -O2 -Wall -Wextra \
         -ffreestanding -nostdlib -static

firmware.elf: start.S main.c link.ld
	$(CROSS)gcc $(CFLAGS) -T link.ld -o $@ start.S main.c

clean:
	rm -f firmware.elf
