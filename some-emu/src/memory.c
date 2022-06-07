#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

uint8_t memory[MEMORY_SIZE];

void write_mem_s(uint32_t address, uint8_t value) {
  if (address == 0x900) {
    if (value == 1) {
      exit(0);
    }
  } else if (address == 0x901) {
    putchar(value);
  }
  
  if (address >= MEMORY_SIZE) {
    // nothing
  } else {
    memory[address] = value;
  }
}

uint8_t read_mem_s(uint32_t address) {
  if (address >= MEMORY_SIZE) {
    return 0;
  } else {
    return memory[address];
  }
}

void write_mem_d(uint32_t address, uint16_t value) {
  write_mem_s(address,   (value & 0x00ff));
  write_mem_s(address+1, (value & 0xff00) >> 8);
}

uint16_t read_mem_d(uint32_t address) {
  uint16_t value = 0;
  
  value |= read_mem_s(address);
  value |= read_mem_s(address+1) << 8;
  
  return value;
}

void write_mem_q(uint32_t address, uint32_t value) {
  write_mem_s(address,   (value & 0x000000ff));
  write_mem_s(address+1, (value & 0x0000ff00) >> 8);
  write_mem_s(address+2, (value & 0x00ff0000) >> 16);
  write_mem_s(address+3, (value & 0xff000000) >> 24);
}

uint32_t read_mem_q(uint32_t address) {
  uint32_t value = 0;
  
  value |= read_mem_s(address);
  value |= read_mem_s(address+1) << 8;
  value |= read_mem_s(address+2) << 16;
  value |= read_mem_s(address+3) << 24;
  
  return value;
}
