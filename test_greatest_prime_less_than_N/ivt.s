# file: ivt.s

.extern isr_reset

.section ivt
.word isr_reset
.skip 2 # isr_error
.word 0
.word 0
.word 0
.skip 6
.end
