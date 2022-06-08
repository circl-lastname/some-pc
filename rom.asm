; Constant addresses
.at 0x900
sys_power:

.at 0x901
sys_print:

.at 0x902
sys_hd0_size:

.at 0x906
sys_hd0_target:

.at 0x90b
sys_hd0_ctl:

.at 0xa00
sys_hd0_block:

; Main code
.at 0x1000

main:
  MOV IQ .s_welcome RAQ
  CALL IQ put_string
  
  
  MOV IQ .s_check_memory RAQ
  CALL IQ put_string
  
  CALL IQ check_memory
  
  MOV AQN g_memory RAQ
  CALL IQ put_dec_num
  
  MOV IS 0x0a RAS
  CALL IQ put_char
  
  
  MOV IQ .s_hd_size RAQ
  CALL IQ put_string
  
  MUL AQN sys_hd0_size IQ 512 RAQ
  CALL IQ put_dec_num
  
  MOV IS 0x0a RAS
  CALL IQ put_char
  
  
  MOV IQ 0 AQN sys_hd0_target
  MOV IQ 1 AQN sys_hd0_ctl
  
  MOV IQ sys_hd0_block RAQ
  MOV IQ 0x2000 RBQ
  MOV IQ 512 RCQ
  CALL IQ memcpy
  
  
  CALL IQ 0x2000
  
  
  JMP IQ halt
  
  .s_welcome:
  .data "SomePC Firmware" 0x0a
  .data 0x0a
  .data 0
  
  .s_check_memory:
  .data "Checking amount of memory... "
  .data 0
  
  .s_hd_size:
  .data "Size of HD0 is "
  .data 0

check_memory:
  PUSH RAQ
  MOV IQ m_end_of_rom RAQ
  JMP IQ .loop_entry
  
  .loop:
  ADD RAQ IQ 1 RAQ
  .loop_entry:
  MOV IS 0xff ASB 0 RAQ
  JEQ ASB 0 RAQ IS 0xff IQ .loop
  
  MOV RAQ AQN .g_memory
  POP RAQ
  RET

; Helper functions
halt:
  MOV IS 1 ASN sys_power
  
  ; Failsafe
  .loop:
  JMP IQ .loop

memcpy:
  MOV RAQ AQN .temp_1
  MOV RBQ AQN .temp_2
  
  .loop:
  MOV PSA .temp_1 RCQ PSA .temp_2 RCQ
  SUB RCQ IQ 1 RCQ
  JNE RCQ IQ 0xffffffff IQ .loop
  
  RET
  
  .temp_1:
  .data 0 0 0 0
  .temp_2:
  .data 0 0 0 0

put_char:
  MOV RAS ASN sys_print
  RET

put_string:
  PUSH RCQ
  MOV RAQ RCQ
  
  .loop:
  MOV ASB 0 RCQ RAS
  CALL IQ put_char
  ADD RCQ IQ 1 RCQ
  JNE ASB 0 RCQ IS 0 IQ .loop
  
  POP RCQ
  RET

put_dec_num:
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
  ; Otherwise skip printing
  JMP IQ .skip_printing
  ; Set printing to 1
  .print_and_set:
  MOV IS 1 ASN .printing
  ; Print digit
  .print:
  PUSH RAQ
  ADD RCS IS 0x30 RAS
  CALL IQ put_char
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

; Global variables
g_memory:
  .data 0 0 0 0

; End of rom marker
m_end_of_rom:
