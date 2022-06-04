.at 0x0900
print:

.at 0x1000

loop:
MOV ASB string RCQ ASN print
ADD RCQ IQ 1 RCQ
JNE ASB string RCQ IS 0 ASN loop

halt:
JMP ASN halt

string:
.data "Hello, World!" 0
