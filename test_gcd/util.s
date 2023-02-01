# In this file we will define function for getting remainder

.global GetRemainder

.section text
#
# int GetRemainder(int f, int s) -> return f % s
# input in r0(f) and r1(s), returns in r2
GetRemainder:
  push r0
  push r1
  push r3
  push r4
  push r5

  # return f - s*(f/s)

  ldr r2,r0  # r2 = f
  ldr r3, r1 # r3 = s
  div r0, r1 # r0 = s/f
  mul r0, r3 # r0 = s*(f/s)
  sub r2, r0 # r2 = f - s*(f/s)

  pop r5
  pop r4
  pop r3
  pop r1
  pop r0
  ret
