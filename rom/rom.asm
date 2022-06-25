; Constant addresses
.at 0x8
sys_memory:

.at 0xc
sys_power:

.at 0x900
sys_print:

.at 0x901
sys_hd0_size:

.at 0x905
sys_hd0_target:

.at 0x909
sys_hd0_ctl:

.at 0xa00
sys_hd0_block:


.at 0x1f00
export_halt:

.at 0x1f04
export_memcpy:

.at 0x1f08
export_strcmp:

.at 0x1f0c
export_putc:

.at 0x1f10
export_puts:

.at 0x1f14
export_putdec:

.at 0x1f18
export_fs_load_file:

.at 0x1f1c
export_fs_read_dir:

.at 0x2000
bootloader:


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

; Helper functions
halt:
  MOV IS 1 ASN sys_power
  
  ; Failsafe
  .loop:
  JMP IQ .loop

memcpy:
  MOV RAQ AQN .temp_1
  MOV RBQ AQN .temp_2
  SUB RCQ IQ 1 RCQ
  
  .loop:
  JEQ RCQ IQ 0xffffffff IQ .end_loop
  MOV PSA .temp_1 RCQ PSA .temp_2 RCQ
  SUB RCQ IQ 1 RCQ
  JMP IQ .loop
  .end_loop:
  
  RET
  
  .temp_1:
  .data 0 0 0 0
  .temp_2:
  .data 0 0 0 0

strcmp:
  PUSH RCQ
  MOV IQ 0 RCQ
  MOV RAQ AQN .temp_1
  MOV RBQ AQN .temp_2
  
  .loop:
  JNE PSA .temp_1 RCQ PSA .temp_2 RCQ IQ .return_0
  JEQ PSA .temp_1 RCQ IQ 0 IQ .return_1
  ADD RCQ IQ 1 RCQ
  JMP IQ .loop
  
  .return_0:
  MOV IQ 0 RAQ
  POP RCQ
  RET
  
  .return_1:
  MOV IQ 1 RAQ
  POP RCQ
  RET
  
  .temp_1:
  .data 0 0 0 0
  .temp_2:
  .data 0 0 0 0

putc:
  MOV RAS ASN sys_print
  RET

puts:
  PUSH RCQ
  MOV RAQ RCQ
  
  .loop:
  MOV ASB 0 RCQ RAS
  CALL IQ putc
  ADD RCQ IQ 1 RCQ
  JNE ASB 0 RCQ IS 0 IQ .loop
  
  POP RCQ
  RET

putdec:
  ; RAQ - Number
  ; RBQ - Divider
  ; RCQ - Temp
  
  PUSH RBQ
  PUSH RCQ
  MOV IQ 1000000000 RBQ
  MOV IS 0 ASN .printing
  
  .loop:
  ; Determine digit in the place of divider
  DIV RAQ RBQ RCS
  
  ; Check if printing digit
  JEQ ASN .printing IS 1 IQ .print
  
  ; Else check if digit non-zero
  JNE RCS IS 0 IQ .print_and_set
  
  ; Else always print last digit
  JEQ RBQ IQ 1 IQ .print
  
  ; Otherwise skip printing
  JMP IQ .skip_printing
  
  ; Set printing to 1
  .print_and_set:
  MOV IS 1 ASN .printing
  
  ; Print digit
  .print:
  PUSH RAQ
  ADD RCS IS 0x30 RAS
  CALL IQ putc
  POP RAQ
  .skip_printing:
  
  ; Subtract digit from number
  MUL RBQ RCS RCQ
  SUB RAQ RCQ RAQ
  
  ; End loop if divider is 1
  JEQ RBQ IQ 1 IQ .end_loop
  
  ; Otherwise move divider one digit
  DIV RBQ IQ 10 RBQ
  
  ; Loop
  JMP IQ .loop
  .end_loop:
  
  POP RCQ
  POP RBQ
  RET
  
  .printing:
  .data 0

.include "fs.asm"
