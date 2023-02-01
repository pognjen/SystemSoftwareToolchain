#include "structures.h"
#include <iomanip>

SymbolTableEntry::SymbolTableEntry(std::string name, Visibility visibility, uint16_t value, uint16_t section, bool isDefined, SymbolType type, ImportExport importExport):
	mName(name), mVisibility(visibility), mValue(value), mSection(section), mIsDefined(isDefined), mType(type), mImportExport(importExport)
{
}

std::string SymbolTableEntry::GetName() const
{
	return mName;
}

Visibility SymbolTableEntry::GetVisibility() const
{
	return mVisibility;
}

uint16_t SymbolTableEntry::GetValue() const
{
	return mValue;
}

uint16_t SymbolTableEntry::GetSection() const
{
	return mSection;
}

bool SymbolTableEntry::GetDefined() const
{
	return mIsDefined;
}

ImportExport SymbolTableEntry::GetImportExport() const
{
	return mImportExport;
}

std::vector<ForwardReferenceTableEntry*> SymbolTableEntry::GetForwardReferenceTable() const
{
	return mForwardReferenceTable;
}

SymbolType SymbolTableEntry::GetType() const
{
	return mType;
}

void SymbolTableEntry::SetDefined(bool isDefined)
{
	mIsDefined = isDefined;
}

void SymbolTableEntry::SetName(std::string name)
{
	mName = name;
}

void SymbolTableEntry::SetVisibility(Visibility visibility)
{
	mVisibility = visibility;
}

void SymbolTableEntry::SetValue(uint16_t value)
{
	mValue = value;
}

void SymbolTableEntry::SetSection(uint16_t section)
{
	mSection = section;
}

void SymbolTableEntry::SetImportExport(ImportExport importExport)
{
	mImportExport = importExport;
}

void SymbolTableEntry::SetType(SymbolType type)
{
	mType = type;
}

void SymbolTableEntry::InsertForwardReferenceEntry(std::string section, uint16_t patch, bool instruction)
{
	mForwardReferenceTable.push_back(new ForwardReferenceTableEntry(section, patch, instruction));
}

void SymbolTableEntry::WriteObj(std::ostream& f)
{
	// Write name
	//
	size_t size;
	size = mName.size();
	f.write((char*)&size, sizeof(size_t));
	f.write((char*)mName.c_str(), size);

	// Write visibility
	//
	f.write((char*)&mVisibility, sizeof(Visibility));

	// Write value
	//
	f.write((char*)&mValue, sizeof(uint16_t));

	// Write section
	//
	f.write((char*)&mSection, sizeof(uint16_t));

	// Write ImportExport
	//
	f.write((char*)&mImportExport, sizeof(ImportExport));

	// Write symbol type
	//
	f.write((char*)&mType, sizeof(SymbolType));
}

void SymbolTableEntry::WriteTxt(std::ostream& f,int idx)
{
	f << std::left << std::setw(15) << std::setfill(' ') << idx;
	f << std::left << std::setw(15) << std::setfill(' ') << mName;
	
	char hex_string[20];
	sprintf(hex_string, "%.2X", mValue);
	std::string value = hex_string;
	value = value.substr(value.size() - 2);
	value.insert(0, "0x");

	std::string impExp = (mImportExport == EXPORTED) ? "Exported(global)" : "Imported(extern)";
	if (mImportExport == NONE)
	{
		impExp = "None";
	}

	f << std::left << std::setw(15) << std::setfill(' ') << value;
	f << std::left << std::setw(15) << std::setfill(' ') << ((mVisibility == LOCAL) ? "LOCAL" : "GLOBAL");
	f << std::left << std::setw(15) << std::setfill(' ') << mSection;
	f << std::left << std::setw(15) << std::setfill(' ') << ((mType == SECTION) ? "SECTION" : "OTHER") ;
	f << std::left << std::setw(10) << std::setfill(' ') << impExp << "\n";
}

void SymbolTableEntry::ReadObj(std::istream& f)
{
	// Read name
	//
	size_t size;
	char* data;
	f.read((char*)&size, sizeof(size_t));
	data = new char[size];
	f.read((char*)data, size);
	data[size] = '\0';
	mName = data;
	//delete data;

	// Read visibility
	//
	f.read((char*)&mVisibility, sizeof(Visibility));

	// Read value
	//
	f.read((char*)&mValue, sizeof(uint16_t));

	// Read section
	//
	f.read((char*)&mSection, sizeof(uint16_t));

	// Read ImportExport
	//
	f.read((char*)&mImportExport, sizeof(ImportExport));

	// Read symbol type
	//
	f.read((char*)&mType, sizeof(SymbolType));
}

void SymbolTable::AppendSymbol(SymbolTableEntry* entry)
{
	mSymbols.push_back(entry);
}

SymbolTableEntry* SymbolTable::FindSymbol(std::string name)
{
	for (auto entry : mSymbols)
	{
		if (entry->GetName() == name)
		{
			return entry;
		}
	}
	return nullptr;
}

std::vector<SymbolTableEntry*> SymbolTable::GetSymbols() const
{
	return mSymbols;
}

SectionHeaderTable::SectionHeaderTable()
{
	AppendSection(new SectionHeaderTableEntry("UND", 0));
}

void SectionHeaderTable::AppendSection(SectionHeaderTableEntry* entry)
{
	mSections.push_back(entry);
}

SectionHeaderTableEntry* SectionHeaderTable::FindSection(std::string name)
{
	for (auto entry : mSections)
	{
		if (entry->GetName() == name)
		{
			return entry;
		}
	}
	return nullptr;
}

SectionHeaderTableEntry* SectionHeaderTable::FindSection(uint16_t sectionIndex)
{
	if (sectionIndex < 0 || sectionIndex >= mSections.size())
	{
		return nullptr;
	}

	return mSections[sectionIndex];
}

uint16_t SectionHeaderTable::GetSectionIndex(std::string name) const
{
	uint16_t entryIndex = 0;
	for (auto entry : mSections)
	{
		if (entry->GetName() == name)
		{
			return entryIndex;
		}
		entryIndex++;
	}
	return entryIndex;
}

std::vector<SectionHeaderTableEntry*> SectionHeaderTable::GetSections()
{
	return mSections;
}

SectionHeaderTableEntry::SectionHeaderTableEntry(std::string name, uint16_t length): mName(name), mLength(length)
{
}

std::string SectionHeaderTableEntry::GetName() const
{
	return mName;
}

uint16_t SectionHeaderTableEntry::GetLength() const
{
	return mLength;
}

uint16_t SectionHeaderTableEntry::GetLoadAddress() const
{
	return mLoadAddress;
}

std::string SectionHeaderTableEntry::GetSourceFile() const
{
	return sourceFile;
}

void SectionHeaderTableEntry::SetSourceFile(std::string src)
{
	sourceFile = src;
}

void SectionHeaderTableEntry::SetLoadAddres(uint16_t addr)
{
	mLoadAddress = addr;
}

void SectionHeaderTableEntry::SetName(std::string name)
{
	mName = name;
}

void SectionHeaderTableEntry::SetLength(uint16_t length)
{
	mLength = length;
}

void SectionHeaderTableEntry::IncreaseLength(uint16_t addend)
{
	mLength += addend;
}

void SectionHeaderTableEntry::WriteObj(std::ostream& f)
{
	// Write name
	//
	size_t size;
	size = mName.size();
	f.write((char*)&size, sizeof(size_t));
	f.write((char*)mName.c_str(), size);

	// Write length
	//
	f.write((char*)&mLength, sizeof(uint16_t));
}

void SectionHeaderTableEntry::WriteTxt(std::ostream& f, int idx)
{
	f << std::left << std::setw(10) << std::setfill(' ') << idx;
	f << std::left << std::setw(10) << std::setfill(' ') << mName;

	char hex_string[20];
	sprintf(hex_string, "%.2X", mLength);
	std::string value = hex_string;
	value = value.substr(value.size() - 2);
	value.insert(0, "0x");

	f << std::left << std::setw(10) << std::setfill(' ') << value <<" bytes\n";
}

void SectionHeaderTableEntry::ReadObj(std::istream& f)
{
	// Read name
	//
	size_t size;
	char* data;
	f.read((char*)&size, sizeof(size_t));
	data = new char[size];
	f.read((char*)data, size);
	data[size] = '\0';
	mName = data;
	//delete data;

	// Read length
	//
	f.read((char*)&mLength, sizeof(uint16_t));
}

RelocationTableEntry::RelocationTableEntry(std::string section, uint16_t offset, RelocationType type, std::string symbol, uint16_t addend) :
	mSection(section), mOffset(offset), mType(type), mSymbol(symbol), mAddend(addend)
{
}

void RelocationTableEntry::SetSection(std::string section)
{
	mSection = section;
}

void RelocationTableEntry::SetOffset(uint16_t offset)
{
	mOffset = offset;
}

void RelocationTableEntry::SetRelocationType(RelocationType type)
{
	mType = type;
}

void RelocationTableEntry::SetSymbol(std::string symbol)
{
	mSymbol = symbol;
}

void RelocationTableEntry::SetAddend(int16_t addend)
{
	mAddend = addend;
}

void RelocationTableEntry::SetSectionIndex(uint16_t sectionIndex)
{
	mSectionIndex = sectionIndex;
}

uint16_t RelocationTableEntry::GetSectionIndex() const
{
	return mSectionIndex;
}

int16_t RelocationTableEntry::GetAddend() const
{
	return mAddend;
}

std::string RelocationTableEntry::GetSection() const
{
	return mSection;
}

uint16_t RelocationTableEntry::GetOffset() const
{
	return mOffset;
}

RelocationType RelocationTableEntry::GetRelocationType() const
{
	return mType;
}

std::string RelocationTableEntry::GetSymbol() const
{
	return mSymbol;
}

void RelocationTableEntry::WriteObj(std::ostream& f)
{
	// Write mSection
	//
	size_t size;
	size = mSection.size();
	f.write((char*)&size, sizeof(size_t));
	f.write((char*)mSection.c_str(), size);

	// Write mOffset
	//
	f.write((char*)&mOffset, sizeof(uint16_t));

	// Write mType
	//
	f.write((char*)&mType, sizeof(RelocationType));

	// Write mSymbol
	//
	size_t size1;
	size1 = mSymbol.size();
	f.write((char*)&size1, sizeof(size_t));
	f.write((char*)mSymbol.c_str(), size1);

	// Write mAddend
	//
	f.write((char*)&mAddend, sizeof(int16_t));
}

void RelocationTableEntry::WriteTxt(std::ostream& f,int idx)
{
	f << std::left << std::setw(15) << std::setfill(' ') << idx;
	f << std::left << std::setw(15) << std::setfill(' ') << mSection;
	char hex_string[20];
	sprintf(hex_string, "%.2X", mOffset);
	std::string offset = hex_string;
	offset = offset.substr(offset.size() - 2);
	offset.insert(0, "0x");
	f << std::left << std::setw(15) << std::setfill(' ') << offset;
	f << std::left << std::setw(15) << std::setfill(' ') << ((mType == REL_16) ? "REL_16" : "REL_16_PC");
	f << std::left << std::setw(15) << std::setfill(' ') << mSymbol;
	f << std::left << std::setw(15) << std::setfill(' ') << mAddend << "\n";
}

void RelocationTableEntry::ReadObj(std::istream& f)
{
	// Read mSection
	//
	size_t size;
	char* data;
	f.read((char*)&size, sizeof(size_t));
	data = new char[size];
	f.read((char*)data, size);
	data[size] = '\0';
	mSection = data;
	//delete data;


	// Read mOffset
	//
	f.read((char*)&mOffset, sizeof(uint16_t));

	// Read mType
	//
	f.read((char*)&mType, sizeof(RelocationType));

	// Read mSymbol
	//
	char* data1;
	f.read((char*)&size, sizeof(size_t));
	data1 = new char[size];
	f.read((char*)data1, size);
	data1[size] = '\0';
	mSymbol = data1;
	//delete data;

	// Read mAddend
	//
	f.read((char*)&mAddend, sizeof(int16_t));
}

void RelocationTable::AppendRelocation(RelocationTableEntry* entry)
{
	mRelocations.push_back(entry);
}

std::vector<RelocationTableEntry*> RelocationTable::GetRelocations()
{
	return mRelocations;
}
