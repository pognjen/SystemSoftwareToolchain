ASSEMBLER=../assembler
LINKER=../linker
EMULATOR=../emulator

${ASSEMBLER} -o gcd.o gcd.s
${ASSEMBLER} -o ivt.o ivt.s
${ASSEMBLER} -o isr_reset.o isr_reset.s
${ASSEMBLER} -o util.o util.s
${ASSEMBLER} -o main.o main.s

${LINKER} -hex -o program.hex ivt.o isr_reset.o main.o util.o gcd.o

${EMULATOR} program.hex