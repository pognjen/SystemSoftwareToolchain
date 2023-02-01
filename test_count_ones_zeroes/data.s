# file data.s
# Arguments for counter are defined here

.global GetNValue

.section data

N0:
  .word 0x0123 #0000 0001 0010 0011 -> R0 = 12, R1 = 4;
N1:
  .word 0x4567 #0100 0101 0110 0111 -> R0 = 8, R1 = 8
N2:
  .word 0x89AB #1000 1001 1010 1011 -> R0 = 8, R1 = 8
N3:
  .word 0xCDEF #1100 1101 1110 1111 -> R0 = 4, R1 = 12


.section text

# // Return N[index]
# int GetNValue(int index);
#
GetNValue:
  # Number of value is stored in R4
  # Return through R4 register
  # In case of error put 0xFFFF in R4
  push r0
  push r1

  ldr r0, $0x03

  cmp r4, r0
  jgt error

  ldr r1, $2
  mul r4, r1

  ldr r4, [r4 + N0]

exit:
  pop r1
  pop r0
  ret

error:
  ldr r4, $0xFFFF
  jmp exit

.end