# © 2023 Intel Corporation

simics_add_module(toy-i2c-slave
  CLASSES toy_i2c_slave
  SOURCES toy-i2c-slave.dml module_load.py
  SIMICS_API 7
)
