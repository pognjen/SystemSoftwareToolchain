ASSEMBLER=../assembler
LINKER=../linker
EMULATOR=../emulator

${ASSEMBLER} -o prime_finder.o prime_finder.s
${ASSEMBLER} -o data.o data.s
${ASSEMBLER} -o ivt.o ivt.s
${ASSEMBLER} -o isr_reset.o isr_reset.s
${ASSEMBLER} -o util.o util.s
${LINKER} -hex -o program.hex ivt.o isr_reset.o prime_finder.o data.o util.o
${EMULATOR} program.hex