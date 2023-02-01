ASSEMBLER=../assembler
LINKER=../linker
EMULATOR=../emulator

${ASSEMBLER} -o counter.o counter.s
${ASSEMBLER} -o data.o data.s
${ASSEMBLER} -o ivt.o ivt.s
${ASSEMBLER} -o isr_reset.o isr_reset.s
${LINKER} -hex -o program.hex ivt.o isr_reset.o counter.o data.o
${EMULATOR} program.hex