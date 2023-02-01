# Algorithm for finding greatest primer smaller than given N
# 

.global my_start
.extern InitValues
.extern pool
.extern IsPrime
.section text

my_start:
  ldr sp, $0xFFEE
  ldr r4, $3
  call %InitValues

  # Pick 9th value from the pool
  ldr r1, $18
  ldr r1, [r1 + pool] # R1 = N

  # Traverse all numbers from 2 to N-1 and find greatest that is primer
  ldr r0, $2 # R0 = 2

loop:
  cmp r0, r1
  jeq end

  push r1
  push r0
  ldr r1, r0
  # We need to find gratest primer that is smaller than value in R1
  call IsPrime # result in R0(0 or 1)
  ldr r3, r0 
  pop r0
  pop r1

  ldr r2, $1
  cmp r3, r2
  jeq %putResult

continue:
  add r0, r2
  jmp loop

end:
  ldr r0, result
  halt

putResult:
 str r0, result
 jmp %continue

.section data
result:
  .word 0x00

.global loop