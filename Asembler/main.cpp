#include <iostream>
#include "lexer.h"
#include "error.h"
#include "parser.h"
#include "assembler.h"
#include <unordered_map>

extern SymbolTable symtab;
extern RelocationTable reltab;
extern SectionHeaderTable sctHdrTab;
extern std::unordered_map<std::string, std::vector<int8_t>> sectionsContent;

int DEBUG_main();

void ReadCmdArguments(int argc, char* argv[], std::string& inputFile, std::string& outputFile);

int main(int argc, char* argv[])
{
	std::string inputFile = "";
	std::string outputFile = "";

	ReadCmdArguments(argc, argv, inputFile, outputFile);
	//DEBUG_main();

	//---LEXER---//
	Lexer lexer(inputFile);

	auto tokens = lexer.GetTokenList();

	//---PARSER---//
	Parser parser(tokens);

	auto lines = parser.GetLineList();

	//---ASM---//
	Assembler as;

	as.Assemble(lines);
	as.Dump(outputFile);
}

void ReadCmdArguments(int argc, char* argv[], std::string& inputFile, std::string& outputFile)
{
	// FORMAT:
	// ./asembler -o izlaz.o ulaz.s
	//

	bool expectingOutput = false;
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

		if (arg == "-o")
		{
			expectingOutput = true;
		}
		else
		{
			if (expectingOutput)
			{
				expectingOutput = false;
				outputFile += arg;
			}
			else
			{
				inputFile += arg;
			}
		}
	}
}

int DEBUG_main()
{
	std::cout << "CALL TO LEXER\n------\n";

	Lexer lexer("/home/ss/Desktop/test/main.s");
	auto tokens = lexer.GetTokenList();

	for (auto i : tokens)
	{
		if (i->GetTokenType() == TokenType::EOLN)
		{
			std::cout << "EOLN\n";
		}
		else if (i->GetTokenType() == TokenType::EOFL)
		{
			std::cout << "EOF\n";
		}
		else
		{
			std::cout << i->GetTokenString() << "\n";
		}
	}
	std::cout << "------\nLEXER FINISHED\n";


	std::cout << "\nCALL TO PARSER\n------\n";

	Parser parser(tokens);

	auto lines = parser.GetLineList();

	for (auto line : lines)
	{
		if (!line->GetLabel().empty())
		{
			std::cout << line->GetLabel() << ": ";
		}

		if (line->GetInstruction() != nullptr)
		{
			std::cout << line->GetInstruction()->GetInstructionString() << " ";
			std::string comma = "";
			for (auto op : line->GetInstruction()->GetOperands())
			{
				std::cout << comma;
				std::cout << op;
				comma = ", ";
			}
		}
		else if (line->GetDirective() != nullptr)
		{
			std::cout << line->GetDirective()->GetDirectiveString() << " ";
			std::string comma = "";
			for (auto op : line->GetDirective()->GetOperands())
			{
				std::cout << comma;
				std::cout << op;
				comma = ", ";
			}
		}

		if (line->IsLastLine())
		{
			std::cout << " EOF\n";
		}
		else
		{
			std::cout << " EOLN\n";
		}
	}
	std::cout << "------\nPARSER FINISHED";

	std::cout << "\nCALL TO ASSEMBLER\n------\n";

	Assembler as;
	as.Assemble(lines);

	for (auto section : sctHdrTab.GetSections())
	{
		std::cout << "\n\nSECTION " << section->GetName() << "\n---------------\n";
		std::vector<int8_t> content = sectionsContent[section->GetName()];

		for (int i = 0; i < content.size(); i++)
		{
			std::cout << static_cast<uint16_t>(content[i]) << ' ';
		}
	}
	std::cout << "\n\n\ SYMTAB \n---------------\n";
	for (auto sym : symtab.GetSymbols())
	{
		std::cout << sym->GetName() << ", " << sctHdrTab.FindSection(sym->GetSection())->GetName() << ", " << (sym->GetVisibility() == LOCAL ? "LOCAL" : "GLOBAL") << "," << sym->GetValue() << "\n";
	}


	std::cout << "\n\n\ RELOCATIONS \n---------------\n";
	for (auto relocation : reltab.GetRelocations())
	{
		std::cout << relocation->GetSection() << " : " << relocation->GetOffset() << ", " << relocation->GetSymbol() << ", "<<relocation->GetAddend()<<"\n";
	}

	std::cout << "\n------\nASSEMBLER FINISHED";

	as.Dump("/home/ss/Desktop/output.o");
	return 0;
}