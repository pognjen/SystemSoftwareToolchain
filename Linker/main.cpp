#include <iostream>
#include <fstream>
#include "linker.h"

void ReadCmdArguments(int argc, char* argv[], std::vector<std::string>& inputFiles, std::string& outputFile, bool& isHexSpecified);

int main(int argc, char* argv[])
{
	std::vector<std::string> inputFiles = {};

	std::string outputFile = "";

	bool isHexSpecified = false;

	ReadCmdArguments(argc, argv, inputFiles, outputFile, isHexSpecified);

	Linker linker;

	linker.ReadBinaryFiles(inputFiles);

	linker.MergeSectionHeaderTables();

	linker.FixSymbolsSectionIndices();

	linker.FixReltabSectionIndices();

	linker.MergeRelocationTables();

	linker.ResolveSymbols();

	linker.MergeSections();

	linker.Patch();

	if (isHexSpecified)
	{
		linker.Dump(outputFile);
	}
}

void ReadCmdArguments(int argc, char* argv[], std::vector<std::string>& inputFiles, std::string& outputFile, bool& isHexSpecified)
{
	// FORMAT:
	// ./linker [-hex] -o izlaz.hex ulaz1.o ulaz2.o ulaz3.o
	//

	bool expectingOutput = false;
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

		if (arg == "-o")
		{
			expectingOutput = true;
		}
		else if (arg == "-hex")
		{
			isHexSpecified = true;
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
				inputFiles.push_back(arg);
			}
		}
	}
}