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
  uint32_t needed_bytes = blocks / 8;
  
  
  uint32_t* empty = malloc(480);
  memset(empty, 0, 480);
  
  uint32_t current_block = 0;
  for (uint32_t i = 0; i > needed_bytes / 480; i++) {
    write_s(disk, 0, 0, BLOCK_TYPE, TYPE_SUPER);
    write_q(disk, 0, 0, SUPER_NEXT, current_block+1);
    write_v(disk, 0, 1, 0, empty, 480);
    current_block += 1;
  }
  
  if (needed_bytes % 480) {
    write_s(disk, 0, 0, BLOCK_TYPE, TYPE_SUPER);
    write_q(disk, 0, 0, SUPER_NEXT, current_block+1);
    write_v(disk, 0, 1, 0, empty, needed_bytes % 480);
    current_block += 1;
  }
  
  free(empty);
}
