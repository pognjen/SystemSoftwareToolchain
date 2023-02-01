#include "parser.h"
//#include "assembler.h"
#include <unordered_map>
#include "error.h"
#include "assembler.h"
#include <fstream>
#include <iomanip>

SymbolTable symtab;
RelocationTable reltab;
SectionHeaderTable sctHdrTab;

std::unordered_map<std::string , std::vector<int8_t>> sectionsContent;

Assembler::Assembler()
{
	LC = 0;
	currentSection = "";
	currentSectionIndex = 0;
}

void Assembler::Assemble(std::vector<Line*> lines)
{
	for (Line* line : lines)
	{
		AssembleLine(line);
		if (line->GetDirective() != nullptr && line->GetDirective()->IsEnd())
		{
			sctHdrTab.FindSection(currentSectionIndex)->IncreaseLength(GetLC());
			break;
		}
	}

	ExecuteBackpatching();
}

void Assembler::ExecuteBackpatching()
{
	for (SymbolTableEntry* symbol : symtab.GetSymbols())
	{
		if (symbol->GetDefined() == false && symbol->GetImportExport() != IMPORTED)
		{
			// Symbol used but not defined or declared as "EXTERN"
			//
			RaiseError("Symbol " + symbol->GetName() + " used but not defined or declares as EXTERN");
		}
		else if (symbol->GetDefined() == false && symbol->GetImportExport() == EXPORTED)
		{
			// Symbol declared as GLOBAL but not defined
			//
			RaiseError("Symbol " + symbol->GetName() + " declared as GLOBAL but not defined");
		}
		else if (symbol->GetDefined() == true && symbol->GetImportExport() == IMPORTED)
		{
			// Symbol defined but also declared as EXTERN
			//
			RaiseError("Symbol " + symbol->GetName() + " defined but also declared as EXTERN");
		}
		else if (symbol->GetDefined() == true || symbol->GetImportExport() == IMPORTED)
		{
			// If symbol is global, then it could've happened that .global is encountered after symbol definition and that there
			// are some relocation entries that are local, so we need to convert them to global relocations
			// i.e.
			// 
			// label : .word 50
			// ...
			// ...
			// mov R0, label -> here we would add local relocation(relocation to section + offset)
			// ...
			// ...
			// .global label
			//
			if (symbol->GetVisibility() == GLOBAL)
			{
				for (RelocationTableEntry* rel : reltab.GetRelocations())
				{
					if (rel->GetSymbol() == symbol->GetName())
					{
						if (rel->GetRelocationType() == REL_16)
						{
							rel->SetAddend(0);
							rel->SetSymbol(symbol->GetName());
						}
						else
						{
							if (rel->GetSectionIndex() != symbol->GetSection())
							{
								rel->SetAddend(-2);
								rel->SetSymbol(symbol->GetName());	
							}
							else
							{
								uint16_t offset = symbol->GetValue() - rel->GetOffset() - 2;

								sectionsContent[rel->GetSection()][rel->GetOffset()] = (uint8_t)(offset & 0x00FF);
								sectionsContent[rel->GetSection()][rel->GetOffset() + 1] = (uint8_t) (offset << 8);
							}
						}
					}
				}
			}

			for (ForwardReferenceTableEntry* entry : symbol->GetForwardReferenceTable())
			{
				int8_t addrMode = 0;
				
				if (entry->instruction)
				{
					addrMode = sectionsContent[entry->section][entry->patch - 1];
				}

				switch (addrMode)
				{
				case MEMDIR_SYMBOL_PCREL:
				case IMMEDIATE_SYMBOL_VALUE_PCREL_JMP:
					if (sctHdrTab.FindSection(symbol->GetSection())->GetName() != entry->section)
					{
						if (symbol->GetVisibility() == GLOBAL)
						{
							reltab.AppendRelocation(new RelocationTableEntry(entry->section, entry->patch, REL_PC_16, symbol->GetName(), -2));
						}
						else
						{
							SectionHeaderTableEntry* sctHdrTabEntry = sctHdrTab.FindSection(symbol->GetSection());

							SymbolTableEntry* sectionInSymtab = symtab.FindSymbol(sctHdrTabEntry->GetName());
							reltab.AppendRelocation(new RelocationTableEntry(entry->section, entry->patch, REL_PC_16, sectionInSymtab->GetName(), -2 + symbol->GetValue()));
						}
					}
					else
					{
								uint16_t offset = symbol->GetValue() - entry->patch - 2;

								sectionsContent[entry->section][entry->patch] = (uint8_t)(offset & 0x00FF);
								sectionsContent[entry->section][entry->patch + 1] = (uint8_t) (offset << 8);	
					}
					break;
				default:
					if (symbol->GetVisibility() == GLOBAL) // For global symbol add relocation for that symbol
					{
						reltab.AppendRelocation(new RelocationTableEntry(entry->section, entry->patch, REL_16, symbol->GetName(), 0));
					}
					else // For local symbol add relocation for section in which symbol is defined with addend=symbol value(offset in section)
					{
						SectionHeaderTableEntry* sctHdrTabEntry = sctHdrTab.FindSection(symbol->GetSection());

						SymbolTableEntry* sectionInSymtab = symtab.FindSymbol(sctHdrTabEntry->GetName());
						reltab.AppendRelocation(new RelocationTableEntry(entry->section, entry->patch, REL_16, sectionInSymtab->GetName(), symbol->GetValue()));
					}
					break;
				}
				
			}
		}
	}
}

void Assembler::SetLC(uint16_t LC)
{
	this->LC = LC;
}

void Assembler::SetCurrentSection(std::string section)
{
	currentSection = section;
}

void Assembler::SetCurrentSectionIndex(uint16_t index)
{
	currentSectionIndex = index;
}

int8_t Assembler::ExtractRegisterNumber(std::string reg)
{
	if (reg == "PC" || reg == "pc")
	{
		return 7;
	}

	if (reg == "SP" || reg == "sp")
	{
		return 6;
	}

	return int8_t(reg[1] - '0');
}

void Assembler::AddRelocationEntry(SymbolTableEntry* symbol, uint16_t patch, bool isInstruction)
{
	if (symbol->GetDefined() == true)
	{
		if (symbol->GetVisibility() == GLOBAL) // For global symbol add relocation for that symbol
		{
			reltab.AppendRelocation(new RelocationTableEntry(currentSection, patch, REL_16, symbol->GetName(), 0));
		}
		else // For local symbol add relocation for section in which symbol is defined with addend=symbol value(offset in section)
		{
			SectionHeaderTableEntry* sctHdrTabEntry = sctHdrTab.FindSection(symbol->GetSection());

			SymbolTableEntry* sectionInSymtab = symtab.FindSymbol(sctHdrTabEntry->GetName());
			reltab.AppendRelocation(new RelocationTableEntry(currentSection, patch, REL_16, sectionInSymtab->GetName(), symbol->GetValue()));
		}
	}
	else
	{
		// Relocation struct will be added during backpatching
		//
		symbol->InsertForwardReferenceEntry(currentSection, patch, isInstruction);
	}
}

void Assembler::CheckInstructionInSectionAndExpectedOperands(uint16_t lineNumber, Instruction* instruction)
{
	if (currentSectionIndex == 0)
	{
		RaiseError("Instruction not in section on line " + std::to_string(lineNumber));
	}

	if (instruction->GetOperands().size() != instruction->GetExpectedNumberOfOperands())
	{
		RaiseError("Wrong number of operands in instruction on line " + std::to_string(lineNumber));
	}
}

uint16_t Assembler::GetCurrentSectionIndex() const
{
	return currentSectionIndex;
}

std::string Assembler::GetCurrentSection() const
{
	return currentSection;
}

uint16_t Assembler::GetLC() const
{
	return LC;
}

void Assembler::Dump(std::string outputFileName)
{
	std::ofstream output;
	std::ofstream outputTxt;
	output.open(outputFileName, std::ios::binary | std::ios::ate | std::ios::out);

	if (!output.is_open() || !output.good())
	{
		RaiseError("Error with opening " + outputFileName);
	}

	std::string outputFileNameTxt = outputFileName + ".txt";

	outputTxt.open(outputFileNameTxt);

	if (!outputTxt.is_open() || !outputTxt.good())
	{
		RaiseError("Error with opening " + outputFileNameTxt);
	}

	// Calculate number of global symbols + sections
	//
	uint16_t symCount = 0;
	for (auto sym : symtab.GetSymbols())
	{
		if (sym->GetType() == SECTION || sym->GetVisibility() == GLOBAL)
		{
			symCount++;
		}
	}

	ELFHeader elf{ sctHdrTab.GetSections().size(), symCount, reltab.GetRelocations().size()};

	// ELF Header
	//
	output.write((char*)&elf, sizeof(ELFHeader));

	// Section header table
	//
	outputTxt << std::setw(10) << "SECTION HEADER TABLE\n";
	outputTxt << std::left << std::setw(10) << std::setfill(' ') << "index";
	outputTxt << std::left << std::setw(10) << std::setfill(' ') << "name";
	outputTxt << std::left << std::setw(10) << std::setfill(' ') << "length";
	outputTxt << "\n";
	int i = 0;
	for (auto sctHdrTabEntry : sctHdrTab.GetSections())
	{
		sctHdrTabEntry->WriteObj(output);
		sctHdrTabEntry->WriteTxt(outputTxt, i++);
	}

	// Symbol table
	//
	outputTxt << "\n\n";
	outputTxt << std::setw(10) << "SYMBOL TABLE\n";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "index";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "name";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "value";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "visibility";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "section";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "type";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "Imported/Exported(extern/global)";
	outputTxt << "\n";
	i = 0;
	for (auto symbol : symtab.GetSymbols())
	{
		if (symbol->GetVisibility() == GLOBAL || symbol->GetType() == SECTION)
		{
			symbol->WriteObj(output);
		}
		symbol->WriteTxt(outputTxt, i++);
	}

	// Relocation table
	//
	outputTxt << "\n\n";
	outputTxt << std::setw(10) << "RELOCATION TABLE\n";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "idx";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "section";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "offset";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "type";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "symbol";
	outputTxt << std::left << std::setw(15) << std::setfill(' ') << "addend" << "\n";
	i = 0;
	for (auto reltabEntry : reltab.GetRelocations())
	{
		reltabEntry->WriteObj(output);
		reltabEntry->WriteTxt(outputTxt, i++);
	}

	i = 0;
	uint16_t addr = 0;
	// Contents of sections
	//
	for (auto sctHdrTabEntry : sctHdrTab.GetSections())
	{
		// Write to obj
		//
		size_t size;
		size = sectionsContent[sctHdrTabEntry->GetName()].size();
		output.write((char*)&size, sizeof(size_t));
		output.write((char*)sectionsContent[sctHdrTabEntry->GetName()].data(), size);

		// Write to txt
		//
		addr = 0;
		outputTxt << "\n\n";
		outputTxt << std::setw(4) << "Section " <<sctHdrTabEntry->GetName()<<"\n";

		for (int j = 0; j < size; j++)
		{
			if (j % 8 == 0)
			{
				outputTxt <<"\n";
				char hex_string[20];
				sprintf(hex_string, "%.4X", addr);
				std::string addrStringHex = hex_string;
				addrStringHex = addrStringHex.substr(addrStringHex.size() - 4);
				addrStringHex.insert(0, "0x");
				outputTxt << std::setw(4) << addrStringHex <<": ";
			}

			char hex_string[20];
			sprintf(hex_string, "%.2X", (int)sectionsContent[sctHdrTabEntry->GetName()][j]);
			std::string byte = hex_string;
			byte = byte.substr(byte.size() - 2);
			byte.insert(0, "0x");
			outputTxt << std::setw(4) << byte << " ";

			addr++;
		}
	}

	output.close();
}

void Assembler::AssembleLine(Line* line)
{
	LabelHandler(line);

	DirectiveHandler(line);

	InstructionHandler(line);
}

void Assembler::LabelHandler(Line* line)
{
	if (!line->GetLabel().empty())
	{
		if (currentSectionIndex == 0)
		{
			RaiseError("Symbol " + line->GetLabel() + " not defined in a section on line " + std::to_string(line->GetLineNumber()));
		}

		SymbolTableEntry* symbol = symtab.FindSymbol(line->GetLabel());

		if (symbol == nullptr) // Symbol is not in symtab, we encountered definition->Ok
		{
			symtab.AppendSymbol(new SymbolTableEntry(line->GetLabel(), LOCAL, LC, currentSectionIndex, true));
		}
		else
		{
			if (symbol->GetDefined() == true) // Symbol is in symtab and we already encountered its definition->Error
			{
				RaiseError("Symbol " + line->GetLabel() + " multiple definition");
			}
			else if (symbol->GetImportExport() == EXPORTED) // Symbol is in symtab but found as .global <symbol> earlier->Ok
			{
				symbol->SetDefined(true);
				symbol->SetSection(currentSectionIndex);
				symbol->SetValue(LC);
			}
			else if (symbol->GetImportExport() == IMPORTED)// Symbol is in symtab but found as .extern <symbol> earlier->Error
			{
				RaiseError("Symbol " + line->GetLabel() + " defined and marked as external symbol");
			}
			else
			{
				// Symbol is undefined
				//
				symbol->SetDefined(true);
				symbol->SetSection(currentSectionIndex);
				symbol->SetValue(LC);
			}
		}
	}
}

void Assembler::DirectiveHandler(Line* line)
{
	if (line->GetDirective() != nullptr)
	{
		line->GetDirective()->ExecuteDirective(line->GetLineNumber(), this);
	}
}

void Assembler::InstructionHandler(Line* line)
{
	if (line->GetInstruction() != nullptr)
	{
		line->GetInstruction()->EncodeInstruction(line->GetLineNumber(), this);
	}
}
