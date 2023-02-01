#ifndef _STRUCTURES_H
#define _STRUCTURES_H

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

enum Visibility {GLOBAL, LOCAL};
enum ImportExport {IMPORTED, EXPORTED, NONE};

class SymbolTableEntry;
struct ForwardReferenceTableEntry;

class SymbolTable
{
public:
	void AppendSymbol(SymbolTableEntry* entry);
	SymbolTableEntry* FindSymbol(std::string name);
	std::vector<SymbolTableEntry*> GetSymbols() const;
private:
	std::vector<SymbolTableEntry*> mSymbols;
};

enum SymbolType {SECTION, OTHER};

class SymbolTableEntry
{
public:
	SymbolTableEntry() = default;
	SymbolTableEntry(std::string name, Visibility visibility, uint16_t value, uint16_t section, bool mIsDefined, SymbolType type = OTHER, ImportExport importExport = NONE);

	std::string GetName() const;
	Visibility GetVisibility() const;
	uint16_t GetValue() const;
	uint16_t GetSection() const;
	bool GetDefined() const;
	ImportExport GetImportExport() const;
	std::vector<ForwardReferenceTableEntry*> GetForwardReferenceTable() const;
	SymbolType GetType() const;

	void SetDefined(bool isDefined);
	void SetName(std::string name);
	void SetVisibility(Visibility visibility);
	void SetValue(uint16_t value);
	void SetSection(uint16_t section);
	void SetImportExport(ImportExport importExport);
	void SetType(SymbolType type);

	void InsertForwardReferenceEntry(std::string section, uint16_t patch, bool instruction = true);

	void WriteObj(std::ostream& f);
	void WriteTxt(std::ostream& f, int idx);
	void ReadObj(std::istream& f);
private:
	std::string mName;
	Visibility mVisibility;
	uint16_t mValue;
	uint16_t mSection;
	ImportExport mImportExport;
	SymbolType mType;

	// Specific to one-pass assembler
	//
	bool mIsDefined;
	std::vector<ForwardReferenceTableEntry*> mForwardReferenceTable;
};

struct ForwardReferenceTableEntry
{
	ForwardReferenceTableEntry(std::string section, uint16_t patch, bool instruction = true):section(section), patch(patch), instruction(instruction){}
	bool instruction;
	std::string section;
	uint16_t patch;
};

class SectionHeaderTableEntry;

class SectionHeaderTable
{
public:
	SectionHeaderTable();
	void AppendSection(SectionHeaderTableEntry* entry);
	SectionHeaderTableEntry* FindSection(std::string name);
	SectionHeaderTableEntry* FindSection(uint16_t sectionIndex);
	uint16_t GetSectionIndex(std::string name) const;
	std::vector<SectionHeaderTableEntry*> GetSections();
private:
	std::vector<SectionHeaderTableEntry*> mSections;
};

class SectionHeaderTableEntry
{
public:
	SectionHeaderTableEntry(std::string name, uint16_t length);
	SectionHeaderTableEntry() = default;

	std::string GetName() const;
	uint16_t GetLength() const;
	uint16_t GetLoadAddress() const;
	std::string GetSourceFile() const;

	void SetSourceFile(std::string src);
	void SetLoadAddres(uint16_t addr);
	void SetName(std::string name);
	void SetLength(uint16_t length);
	void IncreaseLength(uint16_t addend);

	void WriteObj(std::ostream& f);
	void WriteTxt(std::ostream& f, int idx);
	void ReadObj(std::istream& f);

	std::unordered_map<std::string, std::vector<int8_t>> sectionsContent;
private:
	std::string mName;
	uint16_t mLength;

	// Specific to linker
	//
	uint16_t mLoadAddress = 0;
	std::string sourceFile = "";
};

class RelocationTableEntry;

class RelocationTable
{
public:
	void AppendRelocation(RelocationTableEntry* entry);
	std::vector<RelocationTableEntry*> GetRelocations();
private:
	std::vector<RelocationTableEntry*> mRelocations;
};

enum RelocationType
{
	REL_16, REL_PC_16
};

class RelocationTableEntry
{
public:
	RelocationTableEntry(std::string section, uint16_t offset, RelocationType type, std::string symbol, uint16_t addend);
	RelocationTableEntry() = default;

	void SetSection(std::string section);
	void SetOffset(uint16_t offset);
	void SetRelocationType(RelocationType type);
	void SetSymbol(std::string symbol);
	void SetAddend(int16_t addend);
	void SetSectionIndex(uint16_t sectionIndex);

	uint16_t GetSectionIndex() const;
	int16_t GetAddend() const;
	std::string GetSection() const;
	uint16_t GetOffset() const;
	RelocationType GetRelocationType() const;
	std::string GetSymbol() const;

	void WriteObj(std::ostream& f);
	void WriteTxt(std::ostream& f,int idx);
	void ReadObj(std::istream& f);

	uint16_t mSectionIndexSectionRelocation = -1;
private:
	std::string mSection;
	uint16_t mOffset;
	RelocationType mType;
	std::string mSymbol;
	int16_t mAddend;

	// Specific to linker
	//
	uint16_t mSectionIndex;
};

struct ELFHeader
{
	uint16_t SectionHeaderTableCount;
	uint16_t SymbolTableCount;
	uint16_t RelocationTableCount;
};
#endif