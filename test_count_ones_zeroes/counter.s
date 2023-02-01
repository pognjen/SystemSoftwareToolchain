#file a.s
#

.extern GetNValue
.global my_start

.section text

my_start:
#Initialize SP
ldr sp, $0xFFEE

ldr r4, $0x0008

call %GetNValue

# In case of error in execution, 0xFFFF value will be written to R4 register
#

ldr r0, $0xFFFF
cmp R4, r0

jeq error

# Count number of ones and zeroes in loop
# Do it 16 times(once for each bit)
ldr r5, $16
ldr r3, $0
ldr r2, $1

loop:
cmp r5, r3
jeq %end

ldr r0, $1
test r4, r0

jeq inc_zeroes
jmp inc_ones

continue:
  sub r5, r2
  shr r4, r2
  jmp loop

end:
  ldr r0, zeroes
  ldr r1, ones
  halt

error:
  halt

inc_zeroes:
  push r0
  push r1

  ldr r0, zeroes
  ldr r1, $1

  add r0, r1

  str r0, zeroes

  pop r1
  pop r0

  jmp continue

inc_ones:
  push r0
  push r1

  ldr r0, ones
  ldr r1, $1

  add r0, r1

  str r0, ones

  pop r1
  pop r0

  jmp %continue

.section data
zeroes:
  .word 0x00

ones:
  .word 0x00