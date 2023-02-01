# In this file we define N numbers in pool of bytes
#
.section data
pool:
  .skip 20 # Create pool for 10 words

.section text
.global pool
# // Init pool of bytes by following next pattern:
# // data[0] = base * (2^0)
# // data[1] = base * (2^1)
# // data[2] = base * (2^2)
# ...
# ...
# // data[9] = base * (2^9)
#
# NOTE: BE CAREFUL ABOUT OVERFLOW AS MAXIMUM NUMBER WE CAN LOAD IS 65536(0xFFFF).
# base will be transfered through R4 register
#
# void InitValues(int base); 
#
InitValues:
  push r0
  push r1
  push r2
  push r3
  push r5

  ldr r0, $1
  ldr r1, $2
  ldr r2, $1
  ldr r3, $10
  ldr r5, $0

loop:
  push r5
  ldr r5, $0
  cmp r3, r5
  pop r5
  jeq end

  push r4
  mul r4, r0

  push r5
  mul r5, r1
  str r4, [r5 + pool]
  pop r5
  pop r4

  mul r0, r1

  add r5, r2

  push r0
  ldr r0, $1
  sub r3, r0
  pop r0
  jmp loop

end:
  pop r5
  pop r3
  pop r2
  pop r1
  pop r0
  ret

.global InitValues

