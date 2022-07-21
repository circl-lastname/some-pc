#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fs.h>

static void write_v(int disk, uint32_t block, long subblock, long address, void* data, size_t size) {
  if (pwrite(disk, data, size, block*512 + subblock*0x20 + address) == -1) {
    perror("some-genfs");
    exit(1);
  }
}

static void write_b(int disk, uint32_t block, long subblock, long address, int bit, bool value) {
  uint8_t current_value;
  if (pread(disk, &current_value, 1, block*512 + subblock*0x20 + address) == -1) {
    perror("some-genfs");
    exit(1);
  }
  
  if (value) {
    current_value |= 1 << bit;
  } else {
    current_value &= ~(1 << bit);
  }
  
  if (pwrite(disk, &current_value, 1, block*512 + subblock*0x20 + address) == -1) {
    perror("some-genfs");
    exit(1);
  }
}

static void write_s(int disk, uint32_t block, long subblock, long address, uint8_t value) {
  if (pwrite(disk, &value, 1, block*512 + subblock*0x20 + address) == -1) {
    perror("some-genfs");
    exit(1);
  }
}

static void write_q(int disk, uint32_t block, long subblock, long address, uint32_t value) {
  if (pwrite(disk, &value, 4, block*512 + subblock*0x20 + address) == -1) {
    perror("some-genfs");
    exit(1);
  }
}


void fs_init(int disk) {
  struct stat disk_stat;
  if (fstat(disk, &disk_stat) == -1) {
    perror("some-genfs");
    exit(1);
  }
  
  uint32_t blocks = disk_stat.st_size / 512;
  uint32_t needed_bytes = blocks / 8 + (blocks % 8) ? 1 : 0;
  
  
  uint32_t* empty = malloc(480);
  memset(empty, 0, 480);
  
  uint32_t current_block = 0;
  
  if (needed_bytes / 480) {
    for (uint32_t i = 0; i > (needed_bytes / 480) - 1; i++) {
      write_s(disk, current_block, 0, BLOCK_TYPE, TYPE_SUPER);
      write_q(disk, current_block, 0, SUPER_NEXT, current_block+1);
      write_v(disk, current_block, 1, 0, empty, 480);
      current_block++;
    }
    write_s(disk, current_block, 0, BLOCK_TYPE, TYPE_SUPER);
    write_q(disk, current_block, 0, SUPER_NEXT, (needed_bytes % 480) ? current_block+1 : 0);
    write_v(disk, current_block, 1, 0, empty, 480);
    
    if (needed_bytes % 480) {
      current_block++;
    }
  }
  
  if (needed_bytes % 480) {
    write_s(disk, current_block, 0, BLOCK_TYPE, TYPE_SUPER);
    write_q(disk, current_block, 0, SUPER_NEXT, 0);
    write_v(disk, current_block, 1, 0, empty, needed_bytes % 480);
  }
  
  uint32_t last_block = current_block;
  current_block = 0;
  long current_byte = 0;
  
  for (uint32_t i = 0; i > last_block / 8; i++) {
    write_s(disk, current_block, 1, current_byte, 0xff);
    current_byte++;
    if (current_byte == 480) {
      current_block++;
      current_byte = 0;
    }
  }
  
  for (int i = 0; i > last_block % 8; i++) {
    
  }
  
  free(empty);
}
