CALL IQ main
MOV IS 1 ASN 0x900

put_char:
MOV RAS ASN 0x901
RET

put_string:
MOV RAQ AQN .temp
PUSH RCQ
MOV IQ 0 RCQ

.loop:
MOV PSA .temp RCQ RAS
CALL IQ put_char
ADD RCQ IS 1 RCQ
JNE PSA .temp RCQ IS 0 IQ .loop

POP RCQ
RET

.temp:
.data 0 0 0 0

main:
MOV IQ .string RAQ
CALL IQ put_string

RET

.string:
.data "Hello, World!" 0x0a 0
