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
