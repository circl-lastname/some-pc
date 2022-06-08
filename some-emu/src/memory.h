extern int hd0_fd;

void write_mem_s(uint32_t address, uint8_t value);
uint8_t read_mem_s(uint32_t address);
void write_mem_d(uint32_t address, uint16_t value);
uint16_t read_mem_d(uint32_t address);
void write_mem_q(uint32_t address, uint32_t value);
uint32_t read_mem_q(uint32_t address);
