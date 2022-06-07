#include <stdbool.h>
#include <stdint.h>

#include <isa.h>

#include "memory.h"
#include "cpu.h"
#include "acs.h"

acs_target resolve_acs(bool allow_offset) {
  acs_target target;
  
  uint8_t acs = read_mem_s(pc);
  pc++;
  
  target.type = (acs & 0x03) | (acs & 0x30);
  
  switch (acs & 0x03) {
    case ACS_TYPE_I:
      switch (acs & 0x30) {
        case ACS_WIDTH_S:
          target.value = read_mem_s(pc);
          pc++;
        break;
        case ACS_WIDTH_D:
          target.value = read_mem_d(pc);
          pc += 2;
        break;
        case ACS_WIDTH_Q:
          target.value = read_mem_q(pc);
          pc += 4;
        break;
      }
    break;
    case ACS_TYPE_R:
      target.type |= (acs & 0x0c);
    break;
    case ACS_TYPE_A:
    case ACS_TYPE_P:
      target.value = read_mem_q(pc);
      pc += 4;
      
      if ((acs & 0xc0) == ACS_OFFSET_B && allow_offset) {
        acs_target offset_target = resolve_acs(false);
        target.value += read_acs(offset_target);
      }
      
      if ((acs & 0x03) == ACS_TYPE_P) {
        target.value = read_mem_q(target.value);
      }
      
      if ((acs & 0xc0) == ACS_OFFSET_A && (acs & 0x03) == ACS_TYPE_P && allow_offset) {
        acs_target offset_target = resolve_acs(false);
        target.value += read_acs(offset_target);
      }
    break;
  }
  
  return target;
}

void write_acs(acs_target target, uint32_t value) {
  switch (target.type & 0x03) {
    case ACS_TYPE_I:
      // error
    break;
    case ACS_TYPE_R:
      uint32_t* reg;
      
      switch (target.type & 0x0c) {
        case ACS_REG_A:
          reg = &reg_a;
        break;
        case ACS_REG_B:
          reg = &reg_b;
        break;
        case ACS_REG_C:
          reg = &reg_c;
        break;
        case ACS_REG_S:
          reg = &reg_s;
        break;
      }
      
      switch (target.type & 0x30) {
        case ACS_WIDTH_S:
          *reg = (uint8_t)value;
        break;
        case ACS_WIDTH_D:
          *reg = (uint16_t)value;
        break;
        case ACS_WIDTH_Q:
          *reg = (uint32_t)value;
        break;
      }
    break;
    case ACS_TYPE_A:
    case ACS_TYPE_P:
      switch (target.type & 0x30) {
        case ACS_WIDTH_S:
          return write_mem_s(target.value, value);
        break;
        case ACS_WIDTH_D:
          return write_mem_d(target.value, value);
        break;
        case ACS_WIDTH_Q:
          return write_mem_q(target.value, value);
        break;
      }
    break;
  }
}

uint32_t read_acs(acs_target target) {
  switch (target.type & 0x03) {
    case ACS_TYPE_I:
      return target.value;
    break;
    case ACS_TYPE_R:
      uint32_t* reg;
      
      switch (target.type & 0x0c) {
        case ACS_REG_A:
          reg = &reg_a;
        break;
        case ACS_REG_B:
          reg = &reg_b;
        break;
        case ACS_REG_C:
          reg = &reg_c;
        break;
        case ACS_REG_S:
          reg = &reg_s;
        break;
      }
      
      switch (target.type & 0x30) {
        case ACS_WIDTH_S:
          return (uint8_t)*reg;
        break;
        case ACS_WIDTH_D:
          return (uint16_t)*reg;
        break;
        case ACS_WIDTH_Q:
          return (uint32_t)*reg;
        break;
      }
    break;
    case ACS_TYPE_A:
    case ACS_TYPE_P:
      switch (target.type & 0x30) {
        case ACS_WIDTH_S:
          return read_mem_s(target.value);
        break;
        case ACS_WIDTH_D:
          return read_mem_d(target.value);
        break;
        case ACS_WIDTH_Q:
          return read_mem_q(target.value);
        break;
      }
    break;
  }
}
