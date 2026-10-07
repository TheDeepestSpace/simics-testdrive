# © 2023 Intel Corporation

simics_add_module(toy-i2c-master
  CLASSES toy_i2c_master
  SOURCES toy-i2c-master.dml module_load.py
  SIMICS_API 7
)
