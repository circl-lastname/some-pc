#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "memory.h"
#include "cpu.h"

void main(int argc, char** argv) {
  FILE* rom = fopen(ROM_FILE, "rb");
  if (!rom) {
    perror("some-emu");
    exit(1);
  }
  
  int rom_i = ROM_BASE;
  for (int ch = fgetc(rom); ch != EOF; ch = fgetc(rom)) {
    write_mem_s(rom_i, ch);
    rom_i++;
  }
  
  fclose(rom);
  
  write_mem_q(0x4, 0x1000);
  
  hd0_fd = open(HD0_FILE, O_RDWR);
  if (hd0_fd == -1) {
    perror("some-emu");
    exit(1);
  }
  
  cpu_loop();
}
