Block types:
 0x0 - Block bitmap
 0x1 - File
 0x2 - Dir
 
Subblocks are 0x20 long

File header:
 0x0 - Block type
 0x1 - Next block in a chain
 0x5 - Size of file (in bytes, and only first of chain)

Dir header:
 0x0 - Block type
 0x1 - Next block in a chain

Dir entry:
  Subblock 0:
    0x0 - Name (up to 31 chars)
  Subblock 1:
    0x0 - Used/unused
    0x1 - Entry type
      0x0 - File
      0x1 - Dir
    0x2 - Block which contains it
