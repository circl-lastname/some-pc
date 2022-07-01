.include "const.asm"

; Main code
.at 0x1000

main:
  MOV IQ .s_welcome RAQ
  CALL IQ puts
  
  
  MOV AQN sys_memory RAQ
  CALL IQ putdec
  
  MOV IQ .s_memory RAQ
  CALL IQ puts
  
  
  MOV IQ .s_hd_size RAQ
  CALL IQ puts
  
  DIV AQN sys_hd0_size IQ 2 RAQ
  CALL IQ putdec
  
  MOV IQ .s_kb RAQ
  CALL IQ puts
  
  
  MOV IQ .s_booting RAQ
  CALL IQ puts
  
  MOV IQ 0 AQN sys_hd0_target
  MOV IQ 1 AQN sys_hd0_ctl
  
  MOV AQN 0xa05 RAQ
  MOV IQ .s_someboot RBQ
  CALL IQ fs_read_dir
  MOV IQ .s_payload RBQ
  CALL IQ fs_read_dir
  MOV IQ bootloader RBQ
  CALL IQ fs_load_file
  
  MOV IQ halt AQN export_halt
  MOV IQ memcpy AQN export_memcpy
  MOV IQ strcmp AQN export_strcmp
  MOV IQ putc AQN export_putc
  MOV IQ puts AQN export_puts
  MOV IQ putdec AQN export_putdec
  MOV IQ fs_load_file AQN export_fs_load_file
  MOV IQ fs_read_dir AQN export_fs_read_dir
  
  JMP IQ bootloader
  
  
  .s_welcome:
  .data "SomePC Firmware" 0x0a
  .data "Features: Serial, Disk, SomeFS" 0x0a
  .data 0x0a
  .data 0
  
  .s_memory:
  .data " bytes of memory" 0x0a
  .data 0
  
  .s_hd_size:
  .data "Size of disk is "
  .data 0
  
  .s_kb:
  .data " KiB" 0x0a
  .data 0
  
  .s_booting:
  .data "Booting..." 0x0a
  .data 0
  
  .s_someboot:
  .data "someboot" 0
  
  .s_payload:
  .data "payload" 0

.include "lib.asm"
.include "fs.asm"
