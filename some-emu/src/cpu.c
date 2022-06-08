#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <isa.h>

#include "memory.h"
#include "acs.h"

enum {
  #define F(n, a, c) \
    INST_ ## n = c,
  ENUMERATE_INSTS(F)
  #undef F
};

uint32_t pc;

uint32_t reg_a;
uint32_t reg_b;
uint32_t reg_c;
uint32_t reg_s = 0x100;

void cpu_loop() {
  // load program counter with initial value
  pc = read_mem_q(0x4);
  
  while (true) {
    acs_target target;
    uint32_t value;
    
    uint8_t inst = read_mem_s(pc);
    pc++;
    
    switch (inst) {
      case INST_NOP:
      break;
      case INST_EIN:
        // interrupts not supported yet
      break;
      case INST_DIN:
        // interrupts not supported yet
      break;
      
      case INST_MOV:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_PUSH:
        target = resolve_acs(true);
        value = read_acs(target);
        
        switch (target.type & 0x30) {
          case ACS_WIDTH_S:
            write_mem_s(reg_s, value);
            reg_s++;
          break;
          case ACS_WIDTH_D:
            write_mem_d(reg_s, value);
            reg_s += 2;
          break;
          case ACS_WIDTH_Q:
            write_mem_q(reg_s, value);
            reg_s += 4;
          break;
        }
      break;
      case INST_POP:
        target = resolve_acs(true);
        
        switch (target.type & 0x30) {
          case ACS_WIDTH_S:
            reg_s--;
            value = read_mem_s(reg_s);
          break;
          case ACS_WIDTH_D:
            reg_s -= 2;
            value = read_mem_d(reg_s);
          break;
          case ACS_WIDTH_Q:
            reg_s -= 4;
            value = read_mem_q(reg_s);
          break;
        }
        
        write_acs(target, value);
      break;
      
      case INST_JMP:
        target = resolve_acs(true);
        pc = read_acs(target);
      break;
      case INST_JEQ:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        if (value == read_acs(target)) {
          target = resolve_acs(true);
          pc = read_acs(target);
        } else {
          resolve_acs(true);
        }
      break;
      case INST_JNE:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        if (value != read_acs(target)) {
          target = resolve_acs(true);
          pc = read_acs(target);
        } else {
          resolve_acs(true);
        }
      break;
      case INST_JGT:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        if (value > read_acs(target)) {
          target = resolve_acs(true);
          pc = read_acs(target);
        } else {
          resolve_acs(true);
        }
      break;
      case INST_JGE:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        if (value >= read_acs(target)) {
          target = resolve_acs(true);
          pc = read_acs(target);
        } else {
          resolve_acs(true);
        }
      break;
      case INST_JLT:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        if (value < read_acs(target)) {
          target = resolve_acs(true);
          pc = read_acs(target);
        } else {
          resolve_acs(true);
        }
      break;
      case INST_JLE:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        if (value <= read_acs(target)) {
          target = resolve_acs(true);
          pc = read_acs(target);
        } else {
          resolve_acs(true);
        }
      break;
      case INST_CALL:
        target = resolve_acs(true);
        
        write_mem_q(reg_s, pc);
        reg_s += 4;
        
        pc = read_acs(target);
      break;
      case INST_RET:
        reg_s -= 4;
        pc = read_mem_q(reg_s);
      break;
      
      case INST_ADD:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value + read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_SUB:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value - read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_MUL:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value * read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_DIV:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value / read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      
      case INST_OR:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value | read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_NOR:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = ~(value | read_acs(target));
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_AND:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value & read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_NAND:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = ~(value & read_acs(target));
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_XOR:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value ^ read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_XNOR:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = ~(value ^ read_acs(target));
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_SHL:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value << read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
      case INST_SHR:
        target = resolve_acs(true);
        value = read_acs(target);
        
        target = resolve_acs(true);
        value = value >> read_acs(target);
        
        target = resolve_acs(true);
        write_acs(target, value);
      break;
    }
  }
}
