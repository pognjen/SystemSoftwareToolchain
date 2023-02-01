#include "linker.h"
#include <fstream>
#include "error.h"
#include <unordered_set>
#include <iomanip>

// Maping binary file name -> binary file content
//
std::vector<std::pair<std::string, ObjectFileContent*>> filesContent;

// Symtab used for resolving symbols
//
SymbolTable globalSymtab;

SectionHeaderTable globalSctHdrTab;

RelocationTable globalReltab;

// Mapping section name -> content of section
//
std::unordered_map<std::string, std::vector<int8_t>> outputMap;

// Final output
//
std::vector<int8_t> output;

// Change symbols indices to sections so that they point to correct indices in globalSctHdrTab
//
void Linker::FixSymbolsSectionIndices()
{
	for (auto fileContent : filesContent)
	{
		for (auto symbol : fileContent.second->symtab.GetSymbols())
		{
			SectionHeaderTableEntry* section = fileContent.second->sctHdrTab.FindSection(symbol->GetSection());
			symbol->SetSection(FindProperSectionIndexFromGlobalTable(section));
		}
	}
}

// Change relocations indices to sections so that they point to correct indices in globalSctHdrTab
//
void Linker::FixReltabSectionIndices()
{
	for (auto fileContent : filesContent)
	{
		for (auto rel : fileContent.second->reltab.GetRelocations())
		{
			SectionHeaderTableEntry* section = fileContent.second->sctHdrTab.FindSection(rel->GetSection());
			rel->SetSectionIndex(FindProperSectionIndexFromGlobalTable(section));

			SectionHeaderTableEntry* relSection = fileContent.second->sctHdrTab.FindSection(rel->GetSymbol());

			if (relSection != nullptr)
			{
				// This means that we are relocation against section(relocation of local symbol)
				//
				rel->mSectionIndexSectionRelocation = FindProperSectionIndexFromGlobalTable(relSection);
			}
		}
	}
}

uint16_t Linker::FindProperSectionIndexFromGlobalTable(SectionHeaderTableEntry* section)
{
	if (section->GetName() == "UND")
	{
		return 0;
	}

	uint16_t index = 0;
	for (auto sectionInGlobalTable : globalSctHdrTab.GetSections())
	{
		if (sectionInGlobalTable->GetName() == section->GetName() && sectionInGlobalTable->GetSourceFile() == section->GetSourceFile())
		{
			break;
		}
		index++;
	}

	return index;
}

void Linker::SetLoadAddressForSameNameSections(std::string name, uint16_t loadAddress)
{
	for (auto section : globalSctHdrTab.GetSections())
	{
		if (section->GetName() == name)
		{
			section->SetLoadAddres(loadAddress);
		}
	}
}

// Push all section header entries to one global table
//
void Linker::MergeSectionHeaderTables()
{
	for (auto fileContent : filesContent)
	{
		for (auto section : fileContent.second->sctHdrTab.GetSections())
		{
			if (section->GetName() != "UND")
			{
				section->sectionsContent = fileContent.second->sectionsContent;
				globalSctHdrTab.AppendSection(section);
				
			}
		}
	}
}

void Linker::MergeRelocationTables()
{
	for (auto fileContent : filesContent)
	{
		for (auto rel : fileContent.second->reltab.GetRelocations())
		{
			globalReltab.AppendRelocation(rel);
		}
	}
}

// Put all symbols in global symtab and connect each symbol(except sections) with exactly one definition
//
void Linker::ResolveSymbols()
{
	// Put all symbols in global symtab and find exactly one definition for each symbol
	//
	for (auto fileContent : filesContent)
	{
		for (auto symbolFromFile : fileContent.second->symtab.GetSymbols())
		{
			SymbolTableEntry* globalSymbol = globalSymtab.FindSymbol(symbolFromFile->GetName());

			if (globalSymbol == nullptr || globalSymbol->GetType() == SECTION)
			{
				globalSymtab.AppendSymbol(symbolFromFile);
			}
			else
			{
				if (globalSymbol->GetImportExport() == IMPORTED && symbolFromFile->GetImportExport() == EXPORTED)
				{
					globalSymbol->SetSection(symbolFromFile->GetSection());
					globalSymbol->SetValue(symbolFromFile->GetValue());
					globalSymbol->SetImportExport(EXPORTED);
				}
				else if (globalSymbol->GetImportExport() == EXPORTED && symbolFromFile->GetImportExport() == EXPORTED)
				{
					RaiseError("Symbol " + globalSymbol->GetName() + " multiple definitions");
				}
			}
		}
	}

	for (auto symbol : globalSymtab.GetSymbols())
	{
		if (symbol->GetImportExport() != EXPORTED && symbol->GetType() != SECTION)
		{
			RaiseError("Symbol " + symbol->GetName() + " not defined");
		}
	}
}

// Merge sections into array of bytes and perform patching of symbols
//
void Linker::MergeSections()
{
	std::unordered_set<std::string> outputed;

	for (int i = 1; i < globalSctHdrTab.GetSections().size(); i++)
	{
		std::string sectionName = globalSctHdrTab.GetSections()[i]->GetName();

		if (outputed.find(sectionName) != outputed.end())
		{
			continue;
		}

		outputed.insert(sectionName);

		// Traverse all sections from all files
		//
		for (auto file : filesContent)
		{
			auto localSection = file.second->sctHdrTab.FindSection(sectionName);
			if (localSection != nullptr)
			{
				localSection->SetLoadAddres(output.size());
			}

			for (auto byte : file.second->sectionsContent[sectionName])
			{
				output.push_back(byte);
			}
		}
	}
}

// Perform patching
//
void Linker::Patch()
{
	for (auto rel : globalReltab.GetRelocations())
	{
		SymbolTableEntry* symbol = globalSymtab.FindSymbol(rel->GetSymbol());
		uint16_t patchPlace = globalSctHdrTab.FindSection(rel->GetSectionIndex())->GetLoadAddress() + rel->GetOffset();
		if (rel->GetRelocationType() == REL_16)
		{
			// output[patchPlace] = S + A
			//
			uint16_t patch = symbol->GetValue() + globalSctHdrTab.FindSection(symbol->GetSection())->GetLoadAddress() + rel->GetAddend();

						if (symbol->GetType() == SECTION)
			{
				patch = globalSctHdrTab.FindSection(rel->mSectionIndexSectionRelocation)->GetLoadAddress() + rel->GetAddend(); 
			}

			uint8_t patchLow = (uint8_t)(patch & 0x00FF);
			uint8_t patchHigh = (uint8_t)((patch >> 8) & 0x00FF);

			output[patchPlace] = patchLow;
			output[patchPlace + 1] = patchHigh;
		}
		else
		{
			// output[patchPlace] = S + A - P
			//
			int16_t patch = symbol->GetValue()+ globalSctHdrTab.FindSection(symbol->GetSection())->GetLoadAddress() - patchPlace + rel->GetAddend();

			if (symbol->GetType() == SECTION)
			{
				patch = globalSctHdrTab.FindSection(rel->mSectionIndexSectionRelocation)->GetLoadAddress() - patchPlace + rel->GetAddend(); 
			}

			int8_t patchLow = (int8_t)(patch & 0x00FF);
			int8_t patchHigh = (int8_t)((patch >> 8) & 0x00FF);

			output[patchPlace] = patchLow;
			output[patchPlace + 1] = patchHigh;
		}
	}
}

void Linker::Dump(std::string outputFileName)
{
	std::ofstream outputFile;
	std::ofstream outputTxt;
	
	std::ofstream symbolsOutputTxt;
	
	symbolsOutputTxt.open(outputFileName+"_symbols.txt");
	outputTxt.open(outputFileName + ".txt");
	outputFile.open(outputFileName, std::ios::binary | std::ios::ate | std::ios::out);

	if (!outputFile.is_open() || !outputFile.good())
	{
		RaiseError("Error with opening " + outputFileName);
	}

	if (!outputTxt.is_open() || !outputTxt.good())
	{
		RaiseError("Error with opening " + outputFileName);
	}

	if (!symbolsOutputTxt.is_open() || !symbolsOutputTxt.good())
	{
		RaiseError("Error with opening symbols.txt");
	}

	for (auto symbol : globalSymtab.GetSymbols())
	{
			symbolsOutputTxt << symbol->GetName() << " : ";
			char hex_string[20];
			snprintf(hex_string, 20, "%.4X", symbol->GetValue() + globalSctHdrTab.FindSection(symbol->GetSection())->GetLoadAddress());
			std::string symbolValue = hex_string;
			symbolValue = symbolValue.substr(symbolValue.size() - 4);
			symbolValue.insert(0, "0x");
			symbolsOutputTxt << std::setw(4) << symbolValue << "\n";
	}

	uint16_t addr = 0;

	// .txt output
	//
	for (int i = 0; i < output.size(); i++)
	{
		if (i % 8 == 0)
		{
			outputTxt << "\n";
			char hex_string[20];
			snprintf(hex_string, 20, "%.4X", addr);
			std::string addrStringHex = hex_string;
			addrStringHex = addrStringHex.substr(addrStringHex.size() - 4);
			addrStringHex.insert(0, "0x");
			outputTxt << std::setw(4) << addrStringHex << ": ";
		}

		char hex_string[20];
		snprintf(hex_string, 20, "%.2X", (int)output[i]);
		std::string byte = hex_string;
		byte = byte.substr(byte.size() - 2);
		byte.insert(0, "0x");
		outputTxt << std::setw(4) << byte << " ";

		addr++;
	}

	// .obj output
	// Format
	// <number_of_bytes> <addr> <byte> <addr> <byte>......
	//
	size_t sz = output.size();
	outputFile.write((char*)&sz, sizeof(size_t));
	addr = 0x0000;
	for (int i = 0; i < output.size(); i++, addr++)
	{
		outputFile.write((char*)&addr, sizeof(uint16_t));
		outputFile.write((char*)&(output[i]), sizeof(uint8_t));
	}
}

ObjectFileContent::ObjectFileContent(ELFHeader hdr, SectionHeaderTable sct, SymbolTable symtab, RelocationTable rel) :
	header(hdr), sctHdrTab(sct), symtab(symtab), reltab(rel)
{
}

void Linker::ReadBinaryFiles(std::vector<std::string> files)
{
	for (auto fileName : files)
	{
		for (int i = 0; i < filesContent.size(); i++)
		{
			if (filesContent[i].first == fileName)
			{
				RaiseError("File " + fileName + " occurs multiple times in command line arguments");
			}
		}

		ObjectFileContent* cont = new ObjectFileContent();

		std::ifstream input;
		input.open(fileName, std::ios::binary | std::ios::in);

		if (!input.is_open() || !input.good())
		{
			RaiseError("Error opening " + fileName);
		}

		ELFHeader elf;
		input.read((char*)&elf, sizeof(ELFHeader));

		cont->header = elf;

		for (int i = 0; i < elf.SectionHeaderTableCount; i++)
		{
			SectionHeaderTableEntry* sct = new SectionHeaderTableEntry();
			sct->ReadObj(input);
			sct->SetSourceFile(fileName);
			
			if (i == 0)
			{
				delete sct;
			}
			else
			{
				cont->sctHdrTab.AppendSection(sct);
			}
		}

		for (int i = 0; i < elf.SymbolTableCount; i++)
		{
			SymbolTableEntry* sym = new SymbolTableEntry();
			sym->ReadObj(input);
			cont->symtab.AppendSymbol(sym);
		}

		for (int i = 0; i < elf.RelocationTableCount; i++)
		{
			RelocationTableEntry* rel = new RelocationTableEntry();
			rel->ReadObj(input);
			cont->reltab.AppendRelocation(rel);
		}

		for (auto section : cont->sctHdrTab.GetSections())
		{
			size_t size;
			int8_t* data;
			input.read((char*)&size, sizeof(size_t));
			data = new int8_t[size];
			input.read((char*)data, size);
			for (int i = 0; i < size; i++)
			{
				cont->sectionsContent[section->GetName()].push_back(data[i]);
			}
		}

		filesContent.push_back({ fileName, cont });

		input.close();
	}
}
