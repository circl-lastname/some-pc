#define ENUMERATE_INSTS(f) \
  f(NOP,  0, 0x00) \
  f(EIN,  0, 0x01) \
  f(DIN,  0, 0x02) \
  \
  f(MOV,  2, 0x10) \
  f(PUSH, 1, 0x11) \
  f(POP,  1, 0x12) \
  \
  f(JMP,  1, 0x20) \
  f(JEQ,  3, 0x21) \
  f(JNE,  3, 0x22) \
  f(JGT,  3, 0x23) \
  f(JGE,  3, 0x24) \
  f(JLT,  3, 0x25) \
  f(JLE,  3, 0x26) \
  f(CALL, 1, 0x27) \
  f(RET,  0, 0x28) \
  \
  f(ADD,  3, 0x30) \
  f(SUB,  3, 0x31) \
  f(MUL,  3, 0x32) \
  f(DIV,  3, 0x33) \
  \
  f(OR,   3, 0x40) \
  f(NOR,  3, 0x41) \
  f(AND,  3, 0x42) \
  f(NAND, 3, 0x43) \
  f(XOR,  3, 0x44) \
  f(XNOR, 3, 0x45) \
  f(SHL,  3, 0x46) \
  f(SHR,  3, 0x47) \
  f(ROL,  3, 0x48) \
  f(ROR,  3, 0x49)

#define ACS_TYPE_I   0x00
#define ACS_TYPE_R   0x01
#define ACS_TYPE_A   0x02
#define ACS_TYPE_P   0x03

#define ACS_REG_A    0x00
#define ACS_REG_B    0x04
#define ACS_REG_C    0x08
#define ACS_REG_S    0x0c

#define ACS_WIDTH_S  0x00
#define ACS_WIDTH_D  0x10
#define ACS_WIDTH_Q  0x20

#define ACS_OFFSET_N 0x00
#define ACS_OFFSET_B 0x40
#define ACS_OFFSET_A 0x80
