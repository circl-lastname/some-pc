; FS functions
fs_load_file:
  ; RAQ - File block
  ; RBQ - Destination
  
  PUSH RCQ
  
  MOV RAQ AQN sys_hd0_target
  MOV IQ 1 AQN sys_hd0_ctl
  
  MOV IQ 0xa20 RAQ
  
  .loop:
  MOV AQN 0xa05 RCQ
  CALL IQ memcpy
  
  JEQ AQN 0xa01 IQ 0 IQ .end_loop
  
  ADD RBQ AQN 0xa05 RBQ
  
  MOV AQN 0xa01 AQN sys_hd0_target
  MOV IQ 1 AQN sys_hd0_ctl
  JMP IQ .loop
  .end_loop:
  
  POP RCQ
  RET

fs_read_dir:
  ; RAQ - Dir block
  ; RBQ - Filename
  
  PUSH RCQ
  MOV IQ 0xa40 RCQ
  
  MOV RAQ AQN sys_hd0_target
  MOV IQ 1 AQN sys_hd0_ctl
  
  .loop:
  JNE ASB 0x20 RCQ IS 0xff IQ .next_entry
  
  MOV RCQ RAQ
  CALL IQ strcmp
  
  JEQ RAQ IQ 0 IQ .next_entry
  
  MOV AQB 0x22 RCQ RAQ
  JMP IQ .end_loop
  
  .next_entry:
  ADD RCQ IQ 0x40 RCQ
  JEQ RCQ IQ 0xc00 IQ .next_block
  JMP IQ .loop
  
  .next_block:
  JEQ AQN 0xa01 IQ 0 IQ .not_found
  
  MOV AQN 0xa01 AQN sys_hd0_target
  MOV IQ 1 AQN sys_hd0_ctl
  
  MOV IQ 0xa40 RCQ
  JMP IQ .loop
  .not_found:
  MOV IQ 0 RAQ
  .end_loop:
  
  POP RCQ
  RET
