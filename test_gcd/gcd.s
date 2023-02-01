# file: gcd.s
# function for finding gcd is executed as interrupt

.section isr
.global gcd
.extern GetRemainder

#
# int Gcd(int a,int b)
# {
#   if (b == 0) return a;
#   else return gcd(b, a % b)
# }
#
# R0 = a, R1 = b, result in R0
#
gcd:
  push r2
  push r3
  push r4
  push r5

  ldr r2, $0
  cmp r1, r2
  jeq end

  push r0
  push r1

  call GetRemainder # result in R2

  pop r1
  pop r0
  
  ldr r0, r1
  ldr r1, r2

  call gcd

end:
  pop r5
  pop r4
  pop r3
  pop r2
  ret
.end
