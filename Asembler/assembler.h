#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include "structures.h"

class Line;
class Instruction;
class Directive;

class Assembler
{
public:
	Assembler();
	void Assemble(std::vector<Line*> lines);
	void ExecuteBackpatching();

	void SetLC(uint16_t LC);
	void SetCurrentSection(std::string section);
	void SetCurrentSectionIndex(uint16_t index);

	void CheckInstructionInSectionAndExpectedOperands(uint16_t lineNumber, Instruction* instruction);

	int8_t ExtractRegisterNumber(std::string reg);

	void AddRelocationEntry(SymbolTableEntry* symbol, uint16_t patch, bool isInstruction = true);

	uint16_t GetCurrentSectionIndex() const;
	std::string GetCurrentSection() const;
	uint16_t GetLC() const;

	void Dump(std::string outputFileName);
private:
	void AssembleLine(Line* line);

	void LabelHandler(Line* line);
	void DirectiveHandler(Line* line);
	void InstructionHandler(Line* line);

	uint16_t LC;
	std::string currentSection;
	uint16_t currentSectionIndex;
};

#endif