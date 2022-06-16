Block types:
 0x0 - Block bitmap
 0x1 - File
 0x2 - Dir
 
Subblocks are 0x20 long

File header:
 0x0 - Block type
 0x1 - Next block in a chain
 0x5 - Bytes in block

Dir header:
 0x0 - Block type
 0x1 - Next block in a chain

Entries starts at 0x40
Dir entry:
  Subblock 0:
    0x0 - Name (up to 31 chars)
  Subblock 1:
    0x0 - Used/unused
    0x1 - Entry type
      Block type
    0x2 - Block which contains it
