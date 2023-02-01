# In this file we will define function for checking if number is prime

.global IsPrime

.section text

#
# int IsPrime(int num); -> return value is > 0 if number is prime, or == 0 if number is not prime
# return value in R0, input in R1
#
IsPrime:
 push r2
 push r3
 push r4
 push r5


 ldr r0, r1 # R0 = N
 ldr r2, $1
 ldr r1, $2 # R1 = 2
 ldr r2, $1


loop:
 cmp r1, r0
 jeq end
 
 call GetRemainder
 # In r2 we have reminder

 ldr r5, $0
 cmp r2, r5
 jeq ClearR0

 ldr r2, $1
 add r1, r2
 jmp %loop

end:
 ldr r0, $1
 pop r5
 pop r4
 pop r3
 pop r2
 ret

ClearR0:
  ldr R0, $0
  pop r5
  pop r4
  pop r3
  pop r2
  ret

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
