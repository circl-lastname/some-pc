#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fslib.h"

void main(int argc, char** argv) {
  if (argc != 3) {
    puts("usage: some-genfs <dir> <disk>");
    exit(1);
  }
  
  int disk = open(argv[2], O_RDWR);
  if (disk == -1) {
    perror("some-genfs");
    exit(1);
  }
  
  fs_init(disk);
}
