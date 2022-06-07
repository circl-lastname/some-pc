typedef struct {
  uint8_t type;
  
  uint32_t value;
} acs_target;

acs_target resolve_acs(bool allow_offset);
void write_acs(acs_target target, uint32_t value);
uint32_t read_acs(acs_target target);
