#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "memory.h"

uint8_t memory[MEMORY_SIZE];

uint32_t hd0_size;
int hd0_fd;

void write_mem_s(uint32_t address, uint8_t value) {
  switch (address) {
    case 0xc:
      if (value == 1) {
        exit(0);
      }
    break;
    case 0x900:
      putchar(value);
      fflush(stdout);
    break;
    case 0x909:
      if (value = 1) {
        if (pread(hd0_fd, &memory[0xa00], 512, read_mem_q(0x905) * 512) == -1) {
          perror("some-emu");
          exit(1);
        }
      } else if (value = 2) {
        if (pwrite(hd0_fd, &memory[0xa00], 512, read_mem_q(0x905) * 512) == -1) {
          perror("some-emu");
          exit(1);
        }
      }
    break;
  }
  
  if (address >= MEMORY_SIZE) {
    // nothing
  } else {
    memory[address] = value;
  }
}

uint8_t read_mem_s(uint32_t address) {
  switch (address) {
    // find a way to make this better
    case 0x901:
      return (hd0_size & 0x000000ff);
    break;
    case 0x902:
      return (hd0_size & 0x0000ff00) >> 8;
    break;
    case 0x903:
      return (hd0_size & 0x00ff0000) >> 16;
    break;
    case 0x904:
      return (hd0_size & 0xff000000) >> 24;
    break;
  }
  
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
