#ifndef _LINKER_H
#define _LINKER_H

#include "structures.h"
#include <string>
#include <vector>
#include <unordered_map>

class Linker
{
public:
	void ReadBinaryFiles(std::vector<std::string> files);
	void MergeSectionHeaderTables();
	void FixSymbolsSectionIndices();
	void FixReltabSectionIndices();
	void MergeRelocationTables();
	void ResolveSymbols();
	void MergeSections();
	void Patch();
	void Dump(std::string outputFile);
private:
	uint16_t FindProperSectionIndexFromGlobalTable(SectionHeaderTableEntry* section);
	void SetLoadAddressForSameNameSections(std::string name, uint16_t loadAddress);

};

struct ObjectFileContent
{
	ObjectFileContent(ELFHeader hdr, SectionHeaderTable sct, SymbolTable symtab, RelocationTable rel);
	ObjectFileContent() = default;
	ELFHeader header;
	SectionHeaderTable sctHdrTab;
	SymbolTable symtab;
	RelocationTable reltab;
	std::unordered_map<std::string, std::vector<int8_t>> sectionsContent;
};

#endif