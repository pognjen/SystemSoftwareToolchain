#include <iostream>
#include "emulator.h"

void ReadCmdArguments(int argc, char* argv[], std::string& inputFile)
{
	if (argc < 2)
	{
		return;
	}

	inputFile = argv[1];
}

int main(int argc, char* argv[])
{
	std::string inputFile = "";

	ReadCmdArguments(argc, argv, inputFile);

	Emulator emulator;

	emulator.ReadMemoryContent(inputFile);

	emulator.Init();

	emulator.Run();

	emulator.OutputResult();
}