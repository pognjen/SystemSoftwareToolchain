# file main.s

.global my_start

.section text

my_start:
  ldr sp, $0xFFEE
  ldr r2, $4

  # we will find gcd of r0 and r1
  ldr r0, $0xFFEC 
  ldr r1, $0xFE12

  int r2

  halt
