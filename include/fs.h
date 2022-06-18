enum {
  TYPE_SUPER,
  TYPE_FILE,
  TYPE_DIR,
};

#define BLOCK_TYPE 0x0

#define SUPER_NEXT 0x1
#define SUPER_ROOT_DIR 0x5

#define FILE_NEXT 0x1
#define FILE_BYTES 0x5

#define DIR_NEXT 0x1

#define DIR_ENT_NAME 0x0
#define DIR_ENT_USED 0x20
#define DIR_ENT_TYPE 0x21
#define DIR_ENT_BLOCK 0x22
