#include "error.h"
#include "assembler.h"
#include <unordered_map>
#include "parser.h"
//#include "assembler.cpp"

#define UNEXPECTED_END_ERR_CHECK if (IsEnd()) \
								 { \
								 RaiseError("Unexpected end of file on line " + std::to_string(nextLine->GetLineNumber())); \
								 }

int Parser::LinesSoFar = 1;

extern SymbolTable symtab;
extern RelocationTable reltab;
extern SectionHeaderTable sctHdrTab;
extern std::unordered_map<std::string, std::vector<int8_t>> sectionsContent;

Str::Str(int expectedOperandNumber) : Instruction(expectedOperandNumber, "str")
{
}

void Str::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	EncodeDataMovementInstruction(lineNumber, assm, 0xB0);
}

Ldr::Ldr(int expectedOperandNumber) : Instruction(expectedOperandNumber, "ldr")
{
}

void Ldr:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	EncodeDataMovementInstruction(lineNumber, assm, 0xA0);
}

Shr::Shr(int expectedOperandNumber) : Instruction(expectedOperandNumber, "shr")
{
}

void Shr::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x91;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
	
}

Shl::Shl(int expectedOperandNumber) : Instruction(expectedOperandNumber, "shl")
{
}

void Shl::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x90;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Test::Test(int expectedOperandNumber) : Instruction(expectedOperandNumber, "test")
{
}

void Test:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x84;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + +2);
}

Xor::Xor(int expectedOperandNumber) : Instruction(expectedOperandNumber, "xor")
{
}

void Xor:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x83;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Or::Or(int expectedOperandNumber) : Instruction(expectedOperandNumber, "or")
{
}

void Or::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x82;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

And::And(int expectedOperandNumber) : Instruction(expectedOperandNumber, "and")
{
}

void And::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x81;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Not::Not(int expectedOperandNumber) : Instruction(expectedOperandNumber, "not")
{
}

void Not::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x80;

	if (mOperands[0].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | 0x0F;

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);

}

Cmp::Cmp(int expectedOperandNumber) : Instruction(expectedOperandNumber, "cmp")
{
}

void Cmp::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x74;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Div::Div(int expectedOperandNumber) : Instruction(expectedOperandNumber, "div")
{
}

void Div::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x73;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Mul::Mul(int expectedOperandNumber) : Instruction(expectedOperandNumber, "mul")
{
}

void Mul:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x72;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Sub::Sub(int expectedOperandNumber) : Instruction(expectedOperandNumber, "sub")
{
}

void Sub::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x71;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Add::Add(int expectedOperandNumber) : Instruction(expectedOperandNumber, "add")
{
}

void Add::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x70;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Xchg::Xchg(int expectedOperandNumber) : Instruction(expectedOperandNumber, "xchg")
{
}

void Xchg:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x60;

	if (mOperands[0].GetType() != REGDIR && mOperands[1].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Pop::Pop(int expectedOperandNumber) : Instruction(expectedOperandNumber, "pop")
{
}

void Pop:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0xF0;

	if (mOperands[0].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4);

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Push::Push(int expectedOperandNumber) : Instruction(expectedOperandNumber, "push")
{
}

void Push:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0xE0;

	if (mOperands[0].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | 0x0F;

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Jgt::Jgt(int expectedOperandNumber) : Instruction(expectedOperandNumber, "jgt")
{
}

void Jgt:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	EncodeJumpInstruction(lineNumber, assm, 0x53);
}

Jne::Jne(int expectedOperandNumber) : Instruction(expectedOperandNumber, "jne")
{
}

void Jne:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	EncodeJumpInstruction(lineNumber, assm, 0x52);
}

Jeq::Jeq(int expectedOperandNumber) : Instruction(expectedOperandNumber, "jeq")
{
}

void Jeq:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	EncodeJumpInstruction(lineNumber, assm, 0x51);
}

Jmp::Jmp(int expectedOperandNumber) : Instruction(expectedOperandNumber, "jmp")
{
}

void Jmp:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	EncodeJumpInstruction(lineNumber, assm, 0x50);
}

Ret::Ret(int expectedOperandNumber) : Instruction(expectedOperandNumber, "ret")
{
}

void Ret:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x40;

	sectionsContent[currentSection].push_back(opCode);

	assm->SetLC(assm->GetLC() + 1);
}

Call::Call(int expectedOperandNumber) : Instruction(expectedOperandNumber, "call")
{
}

void Call::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	EncodeJumpInstruction(lineNumber, assm, 0x30);
}

Iret::Iret(int expectedOperandNumber) : Instruction(expectedOperandNumber, "iret")
{
}

void Iret::EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x20;

	sectionsContent[currentSection].push_back(opCode);

	assm->SetLC(assm->GetLC() + 1);
}

Int::Int(int expectedOperandNumber) : Instruction(expectedOperandNumber, "int")
{
}

void Int:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x10;

	if (mOperands[0].GetType() != REGDIR)
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}

	// 0b DDDD 1111(D->Register in instruction)
	// 
	int8_t regByte = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | 0x0F;

	sectionsContent[currentSection].push_back(opCode);
	sectionsContent[currentSection].push_back(regByte);

	assm->SetLC(assm->GetLC() + 2);
}

Halt::Halt(int expectedOperandNumber) : Instruction(expectedOperandNumber, "halt")
{

}

void Halt:: EncodeInstruction(uint16_t lineNumber, Assembler* assm)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();

	int8_t opCode = 0x00;

	sectionsContent[currentSection].push_back(opCode);

	assm->SetLC(assm->GetLC() + 1);
}

Operand::Operand()
{
}

uint16_t Operand::GetLiteral() const
{
	return mLiteral;
}

std::string Operand::GetSymbol() const
{
	return mSymbol;
}

OperandType Operand::GetType() const
{
	return mOperandType;
}

std::string Operand::GetRegister() const
{
	return mRegister;
}

void Operand::SetType(OperandType type)
{
	mOperandType = type;
}

void Operand::SetLiteral(uint16_t literal)
{
	mLiteral = literal;
}

void Operand::SetSymbol(std::string symbol)
{
	mSymbol = symbol;
}

void Operand::SetRegister(std::string registerString)
{
	mRegister = registerString;
}


Directive::Directive(std::string directive)
{
	this->directive = directive;
	mOperands = {};
}

void Directive::AppendOperand(Operand operand)
{
	mOperands.push_back(operand);
}

Directive* Directive::CreateDirectiveBasedOnString(std::string directive)
{
	if (directive == ".global") return new Global();
	if (directive == ".extern")return new Extern();
	if (directive == ".section")return new Section();
	if (directive == ".word")return new Word();
	if (directive == ".skip")return new Skip();
	if (directive == ".end")return new End();
	return nullptr;
}

std::string Directive::GetDirectiveString() const
{
	return directive;
}

std::vector<Operand> Directive::GetOperands() const
{
	return mOperands;
}

bool Directive::IsEnd() const
{
	return isEnd;
}

Instruction::Instruction(int expectedOperandsNumber, std::string instructionString)
{
	mExpectedOperandsNumber = expectedOperandsNumber;
	instruction = instructionString;
	mOperands = {};
}

bool Instruction::AppendOperand(Operand operand)
{
	if (mExpectedOperandsNumber < mOperands.size())
	{
		return false;
	}
	
	mOperands.push_back(operand);
	return true;
}

Instruction* Instruction::CreateInstructionBasedOnString(std::string instructionString)
{
	if (instructionString == "halt") return new Halt(0);
	if (instructionString == "int") return new Int(1);
	if (instructionString == "iret") return new Iret(0);
	if (instructionString == "call") return new Call(1);
	if (instructionString == "ret") return new Ret(0);
	if (instructionString == "jmp") return new Jmp(1);
	if (instructionString == "jeq") return new Jeq(1);
	if (instructionString == "jne") return new Jne(1);
	if (instructionString == "jgt") return new Jgt(1);
	if (instructionString == "push") return new Push(1);
	if (instructionString == "pop") return new Pop(1);
	if (instructionString == "xchg") return new Xchg(2);
	if (instructionString == "add") return new Add(2);
	if (instructionString == "sub") return new Sub(2);
	if (instructionString == "mul") return new Mul(2);
	if (instructionString == "div") return new Div(2);
	if (instructionString == "cmp") return new Cmp(2);
	if (instructionString == "not") return new Not(1);
	if (instructionString == "and") return new And(2);
	if (instructionString == "xor") return new Xor(2);
	if (instructionString == "test") return new Test(2);
	if (instructionString == "shl") return new Shl(2);
	if (instructionString == "shr") return new Shr(2);
	if (instructionString == "ldr") return new Ldr(2);
	if (instructionString == "str") return new Str(2);
	return nullptr;
}

bool Instruction::IsJumpInstruction() const
{
	return (instruction == "call" || instruction == "jmp" || instruction == "jeq" || instruction == "jne" || instruction == "jgt");
}

std::string Instruction::GetInstructionString() const
{
	return instruction;
}

std::vector<Operand> Instruction::GetOperands() const
{
	return mOperands;
}

int Instruction::GetExpectedNumberOfOperands() const
{
	return mExpectedOperandsNumber;
}

void Instruction::EncodeJumpInstruction(uint16_t lineNumber, Assembler* assm, int8_t opcode)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();
	uint16_t currentSectionIndex = assm->GetCurrentSectionIndex();

	if (mOperands[0].GetType() == IMMEDIATE_JMP || mOperands[0].GetType() == MEMDIR_LITERAL_JMP) // <literal> or *<literal>
	{
		int8_t regsDesc = 0xFF;
		int8_t addrMode = (int8_t)(mOperands[0].GetType());
		int8_t dataHigh = (int8_t)(mOperands[0].GetLiteral() >> 8);
		int8_t dataLow = (int8_t)(mOperands[0].GetLiteral());

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);
		sectionsContent[currentSection].push_back(dataLow);
		sectionsContent[currentSection].push_back(dataHigh);
		
		assm->SetLC(assm->GetLC() + 5);
	}
	else if (mOperands[0].GetType() == IMMEDIATE_SYMBOL_VALUE_ABS_JMP || mOperands[0].GetType() == MEMDIR_SYMBOL_JMP) // <symbol> or *<symbol>
	{
		SymbolTableEntry* symbol = symtab.FindSymbol(mOperands[0].GetSymbol());

		if (symbol == nullptr)
		{
			symtab.AppendSymbol(new SymbolTableEntry(mOperands[0].GetSymbol(), LOCAL, 0x00, currentSectionIndex, false));
			symbol = symtab.FindSymbol(mOperands[0].GetSymbol());
		}

		assm->AddRelocationEntry(symbol, assm->GetLC() + 3);

		int8_t regsDesc = 0xFF;
		int8_t addrMode = (int8_t)(mOperands[0].GetType());
		int8_t dataHigh = 0;
		int8_t dataLow = 0;

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);
		sectionsContent[currentSection].push_back(dataLow);
		sectionsContent[currentSection].push_back(dataHigh);
		
		assm->SetLC(assm->GetLC() + 5);
	}
	else if (mOperands[0].GetType() == IMMEDIATE_SYMBOL_VALUE_PCREL_JMP) // %<symbol> (PC relative addressing)
	{
		SymbolTableEntry* symbol = symtab.FindSymbol(mOperands[0].GetSymbol());

		if (symbol == nullptr)
		{
			symtab.AppendSymbol(new SymbolTableEntry(mOperands[0].GetSymbol(), LOCAL, 0x00, currentSectionIndex, false));
			symbol = symtab.FindSymbol(mOperands[0].GetSymbol());
		}

		int8_t regsDesc = 0xFF;
		int8_t addrMode = (int8_t)(mOperands[0].GetType());
		int8_t dataHigh = 0;
		int8_t dataLow = 0;

		if (symbol->GetDefined() == false)
		{
			symbol->InsertForwardReferenceEntry(currentSection, assm->GetLC() + 3);
		}
		else if (symbol->GetSection() != currentSectionIndex)
		{
			if (symbol->GetVisibility() == GLOBAL)
			{
				reltab.AppendRelocation(new RelocationTableEntry(currentSection, assm->GetLC() + 3, REL_PC_16, symbol->GetName(), -2));
			}
			else
			{
				SectionHeaderTableEntry* sctHdrTabEntry = sctHdrTab.FindSection(symbol->GetSection());

				SymbolTableEntry* sectionInSymtab = symtab.FindSymbol(sctHdrTabEntry->GetName());
				reltab.AppendRelocation(new RelocationTableEntry(currentSection, assm->GetLC() + 3, REL_PC_16, sectionInSymtab->GetName(), -2 + symbol->GetValue()));
			}
		}
		else
		{
			uint16_t pcOffset = symbol->GetValue() - (assm->GetLC() + 5);
			dataHigh = (int8_t)(pcOffset >> 8);
			dataLow = (int8_t) pcOffset;
		}

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);
		sectionsContent[currentSection].push_back(dataLow);
		sectionsContent[currentSection].push_back(dataHigh);

		assm->SetLC(assm->GetLC() + 5);
	}
	else if (mOperands[0].GetType() == REGDIR_JMP || mOperands[0].GetType() == REGIND_JMP) // *<reg> or *[<reg>]
	{
		int8_t regsDesc = (assm->ExtractRegisterNumber(mOperands[0].GetRegister())) | 0xF0;
		int8_t addrMode = (int8_t)(mOperands[0].GetType());

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);

		assm->SetLC(assm->GetLC() + 3);
	}
	else if (mOperands[0].GetType() == REGIND_LITERAL_JMP || mOperands[0].GetType() == REGIND_SYMBOL_JMP) // *[<reg> + <literal>] or *[<reg> + <symbol>]
	{
		int8_t regsDesc = (assm->ExtractRegisterNumber(mOperands[0].GetRegister())) | 0xF0;
		int8_t addrMode = (int8_t)(mOperands[0].GetType());
		int8_t dataHigh = (int8_t)(mOperands[0].GetLiteral() >> 8);
		int8_t dataLow = (int8_t)(mOperands[0].GetLiteral());

		if (mOperands[0].GetType() == REGIND_SYMBOL_JMP)
		{
			SymbolTableEntry* symbol = symtab.FindSymbol(mOperands[0].GetSymbol());

			if (symbol == nullptr)
			{
				symtab.AppendSymbol(new SymbolTableEntry(mOperands[0].GetSymbol(), LOCAL, 0x00, currentSectionIndex, false));
				symbol = symtab.FindSymbol(mOperands[0].GetSymbol());
			}

			assm->AddRelocationEntry(symbol, assm->GetLC() + 3);
		}

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);
		sectionsContent[currentSection].push_back(dataLow);
		sectionsContent[currentSection].push_back(dataHigh);

		assm->SetLC(assm->GetLC() + 5);
	}
	else
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}
}

void Instruction::EncodeDataMovementInstruction(uint16_t lineNumber, Assembler* assm, int8_t opcode)
{
	assm->CheckInstructionInSectionAndExpectedOperands(lineNumber, this);

	std::string currentSection = assm->GetCurrentSection();
	uint16_t currentSectionIndex = assm->GetCurrentSectionIndex();

	if (mOperands[1].GetType() == IMMEDIATE || mOperands[1].GetType() == MEMDIR_LITERAL) // $<literal> or <literal>
	{
		if (mOperands[0].GetType() != REGDIR)
		{
			RaiseError("Wrong type of first operand on line " + std::to_string(lineNumber));
		}

		int8_t regsDesc = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | 0x0F;
		int8_t addrMode = (int8_t)(mOperands[1].GetType());
		int8_t dataHigh = (int8_t)(mOperands[1].GetLiteral() >> 8);
		int8_t dataLow = (int8_t)(mOperands[1].GetLiteral());

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);
		sectionsContent[currentSection].push_back(dataLow);
		sectionsContent[currentSection].push_back(dataHigh);

		assm->SetLC(assm->GetLC() + 5);
	}
	else if (mOperands[1].GetType() == IMMEDIATE_SYMBOL_VALUE || mOperands[1].GetType() == MEMDIR_SYMBOL_ABS) // $<symbol> or <symbol>
	{
		SymbolTableEntry* symbol = symtab.FindSymbol(mOperands[1].GetSymbol());

		if (symbol == nullptr)
		{
			symtab.AppendSymbol(new SymbolTableEntry(mOperands[1].GetSymbol(), LOCAL, 0x00, currentSectionIndex, false));
			symbol = symtab.FindSymbol(mOperands[1].GetSymbol());
		}

		assm->AddRelocationEntry(symbol, assm->GetLC() + 3);

		if (mOperands[0].GetType() != REGDIR)
		{
			RaiseError("Wrong type of first operand on line " + std::to_string(lineNumber));
		}

		int8_t regsDesc = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | 0x0F;
		int8_t addrMode = (int8_t)(mOperands[1].GetType());
		int8_t dataHigh = 0;
		int8_t dataLow = 0;

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);
		sectionsContent[currentSection].push_back(dataLow);
		sectionsContent[currentSection].push_back(dataHigh);

		assm->SetLC(assm->GetLC() + 5);
	}
	else if (mOperands[1].GetType() == MEMDIR_SYMBOL_PCREL) // %<symbol>
	{
		SymbolTableEntry* symbol = symtab.FindSymbol(mOperands[1].GetSymbol());

		if (symbol == nullptr)
		{
			symtab.AppendSymbol(new SymbolTableEntry(mOperands[1].GetSymbol(), LOCAL, 0x00, currentSectionIndex, false));
			symbol = symtab.FindSymbol(mOperands[1].GetSymbol());
		}

		if (mOperands[0].GetType() != REGDIR)
		{
			RaiseError("Wrong type of first operand on line " + std::to_string(lineNumber));
		}

		int8_t regsDesc = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | 0x0F;
		int8_t addrMode = (int8_t)(mOperands[1].GetType());
		int8_t dataHigh = (int8_t)(symbol->GetValue() >> 8);
		int8_t dataLow = (int8_t)(symbol->GetValue());

		if (symbol->GetDefined() == false)
		{
			symbol->InsertForwardReferenceEntry(currentSection, assm->GetLC() + 3);
		}
		else if (symbol->GetSection() != currentSectionIndex)
		{
			if (symbol->GetVisibility() == GLOBAL)
			{
				reltab.AppendRelocation(new RelocationTableEntry(currentSection, assm->GetLC() + 3, REL_PC_16, symbol->GetName(), -2));
			}
			else
			{
				SectionHeaderTableEntry* sctHdrTabEntry = sctHdrTab.FindSection(symbol->GetSection());

				SymbolTableEntry* sectionInSymtab = symtab.FindSymbol(sctHdrTabEntry->GetName());
				reltab.AppendRelocation(new RelocationTableEntry(currentSection, assm->GetLC() + 3, REL_PC_16, sectionInSymtab->GetName(), -2 + symbol->GetValue()));
			}
		}
		else
		{
			uint16_t pcOffset = assm->GetLC() + 5 - symbol->GetValue();
			dataHigh = (int8_t)(pcOffset >> 8);
			dataLow = (int8_t)pcOffset;
		}

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);
		sectionsContent[currentSection].push_back(dataLow);
		sectionsContent[currentSection].push_back(dataHigh);

		assm->SetLC(assm->GetLC() + 5);
	}
	else if (mOperands[1].GetType() == REGDIR || mOperands[1].GetType() == REGIND) // <reg> or [<reg>]
	{
		if (mOperands[0].GetType() != REGDIR)
		{
			RaiseError("Wrong type of first operand on line " + std::to_string(lineNumber));
		}

		int8_t regsDesc = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));
		int8_t addrMode = (int8_t)(mOperands[1].GetType());

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);

		assm->SetLC(assm->GetLC() + 3);
	}
	else if (mOperands[1].GetType() == REGIND_LITERAL || mOperands[1].GetType() == REGIND_SYMBOL) // [<reg> + <literal>] or [<reg> + <symbol>]
	{
		if (mOperands[0].GetType() != REGDIR)
		{
			RaiseError("Wrong type of first operand on line " + std::to_string(lineNumber));
		}

		int8_t regsDesc = (assm->ExtractRegisterNumber(mOperands[0].GetRegister()) << 4) | (assm->ExtractRegisterNumber(mOperands[1].GetRegister()));
		int8_t addrMode = (int8_t)(mOperands[1].GetType());
		int8_t dataHigh = (int8_t)(mOperands[1].GetLiteral() >> 8);
		int8_t dataLow = (int8_t)(mOperands[1].GetLiteral());

		if (mOperands[1].GetType() == REGIND_SYMBOL)
		{
			SymbolTableEntry* symbol = symtab.FindSymbol(mOperands[1].GetSymbol());

			if (symbol == nullptr)
			{
				symtab.AppendSymbol(new SymbolTableEntry(mOperands[1].GetSymbol(), LOCAL, 0x00, currentSectionIndex, false));
				symbol = symtab.FindSymbol(mOperands[1].GetSymbol());
			}

			assm->AddRelocationEntry(symbol, assm->GetLC() + 3);

			dataHigh = 0;
			dataLow = 0;
		}

		sectionsContent[currentSection].push_back(opcode);
		sectionsContent[currentSection].push_back(regsDesc);
		sectionsContent[currentSection].push_back(addrMode);
		sectionsContent[currentSection].push_back(dataLow);
		sectionsContent[currentSection].push_back(dataHigh);

		assm->SetLC(assm->GetLC() + 5);
	}
	else
	{
		RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
	}
}

Line::Line()
{
	mInstruction = nullptr;
	mDirective = nullptr;
	mLabel = "";
	mLineNumber = -1;
	mLastLine = false;
}

Line::Line(Instruction* instruction, Directive* directive, std::string label, int lineNumber)
{
	mInstruction = instruction;
	mDirective = directive;
	mLabel = label;
	mLineNumber = lineNumber;
	mLastLine = instruction != nullptr && instruction->IsJumpInstruction();
}

Instruction* Line::GetInstruction() const
{
	return mInstruction;
}

Directive* Line::GetDirective() const
{
	return mDirective;
}

std::string Line::GetLabel() const
{
	return mLabel;
}

int Line::GetLineNumber() const
{
	return mLineNumber;
}

bool Line::IsLastLine() const
{
	return mLastLine;
}

void Line::MarkLastLine()
{
	mLastLine = true;
}

void Line::SetInstruction(Instruction* instruction)
{
	mInstruction = instruction;
}

void Line::SetDirective(Directive* directive)
{
	mDirective = directive;
}

void Line::SetLabel(std::string label)
{
	mLabel = label;
}

void Line::SetLineNumber(int lineNumber)
{
	mLineNumber = lineNumber;
}

Parser::Parser(std::vector<Token*> tokens)
{
	mTokens = tokens;
	mTokenIterator = 0;
}

std::vector<Line*> Parser::GetLineList()
{
	std::vector<Line*> lineList = {};

	while (!IsEnd())
	{
		Line* nextLine = GetNextLine();
		lineList.push_back(nextLine);

		if (nextLine->IsLastLine())
		{
			break;
		}
	}

	return lineList;
}

Line* Parser::GetNextLine()
{
	Line* nextLine = new Line();
	nextLine->SetLineNumber(LinesSoFar++);

	ReadLabel(nextLine);

	ReadCommand(nextLine);

	ReadOperandList(nextLine);

	ReadEOLN(nextLine);

	return nextLine;
}

void Parser::ReadLabel(Line* nextLine)
{
	if (!IsEnd() && mTokens[mTokenIterator]->GetTokenType() == TokenType::IDENTIFIER)
	{
		std::string symbol = mTokens[mTokenIterator]->GetTokenString();
		mTokenIterator++;

		if (IsEnd() || mTokens[mTokenIterator]->GetTokenType() != TokenType::COLON)
		{
			RaiseError("Unknown syntax on line " + std::to_string(nextLine->GetLineNumber()));
		}
		else
		{
			mTokenIterator++;
			nextLine->SetLabel(symbol);
		}
	}
}

void Parser::ReadCommand(Line* nextLine)
{
	if (!IsEnd() && mTokens[mTokenIterator]->GetTokenType() == TokenType::INSTRUCTION)
	{
		nextLine->SetInstruction(Instruction::CreateInstructionBasedOnString(mTokens[mTokenIterator]->GetTokenString()));
		mTokenIterator++;
	}
	else if (!IsEnd() && mTokens[mTokenIterator]->GetTokenType() == TokenType::DIRECTIVE)
	{
		nextLine->SetDirective(Directive::CreateDirectiveBasedOnString(mTokens[mTokenIterator]->GetTokenString()));
		mTokenIterator++;
	}
	else if (IsEnd())
	{
		RaiseError("Unexpected end of file on line " + std::to_string(nextLine->GetLineNumber()));
	}
}

void Parser::ReadOperandList(Line* nextLine)
{
	if (IsEnd())
	{
		return;
	}
	
	mType = mTokens[mTokenIterator]->GetTokenType();
	bool isJMPInstruction = nextLine->GetInstruction() != nullptr && nextLine->GetInstruction()->IsJumpInstruction();

	if (mType == TokenType::DOLLAR || mType == TokenType::PERCENT || mType == TokenType::IDENTIFIER ||
		mType == TokenType::SQUARE_BRACKET_OPEN || mType == TokenType::ASTERISK || mType == TokenType::REGISTER ||
		mType == TokenType::NUMERIC_LITERAL_HEX || mType == TokenType::NUMERIC_LITERAL_DEC)
	{
		while (1)
		{
			Operand* operand = new Operand();

			if (mType == TokenType::PLUS || mType == TokenType::MINUS ||
				mType == TokenType::NUMERIC_LITERAL_DEC || mType == TokenType::NUMERIC_LITERAL_HEX) // <literal>
			{
				operand->SetType(isJMPInstruction ? OperandType::IMMEDIATE_JMP : OperandType::MEMDIR_LITERAL);

				ParseLiteral(nextLine, operand);
			}
			else if (mType == TokenType::DOLLAR) //  $<literal> or $<symbol>
			{
				if (isJMPInstruction)
				{
					RaiseError("Unallowed syntax for JMP type of instruction on line " + std::to_string(nextLine->GetLineNumber()));
				}

				NextToken(nextLine);

				if (mType == TokenType::NUMERIC_LITERAL_DEC || mType == TokenType::NUMERIC_LITERAL_HEX)
				{
					ParseLiteral(nextLine, operand);
					operand->SetType(OperandType::IMMEDIATE);
				}
				else if (mType == TokenType::IDENTIFIER)
				{
					ParseSymbol(nextLine, operand);
					operand->SetType(OperandType::IMMEDIATE_SYMBOL_VALUE);
				}
			}
			else if (mType == TokenType::IDENTIFIER) // <symbol>
			{
				ParseSymbol(nextLine, operand);
				operand->SetType(isJMPInstruction? OperandType::IMMEDIATE_SYMBOL_VALUE_ABS_JMP : OperandType::MEMDIR_SYMBOL_ABS);
			}
			else if (mType == TokenType::PERCENT) // %<symbol>
			{
				NextToken(nextLine);

				ParseSymbol(nextLine, operand);
				operand->SetType(isJMPInstruction ? OperandType::IMMEDIATE_SYMBOL_VALUE_PCREL_JMP : OperandType::MEMDIR_SYMBOL_PCREL);
			}
			else if (mType == TokenType::REGISTER) // <reg>
			{
				ParseRegister(nextLine, operand);
				operand->SetType(OperandType::REGDIR);
			}
			else if (mType == TokenType::SQUARE_BRACKET_OPEN) // [<reg> + <literal>] or [<reg> + <symbol>]
			{
				if (isJMPInstruction)
				{
					RaiseError("Unallowed syntax for JMP type of instruction on line " + std::to_string(nextLine->GetLineNumber()));
				}

				ParseRegisterExpression(nextLine, operand);
			}
			else if (mType == TokenType::ASTERISK) // *<reg> or *[<reg> + <literal>] or *[<reg> + <symbol>]
			{
				if (!isJMPInstruction)
				{
					RaiseError("Unallowed syntax for data movement type of instruction on line " + std::to_string(nextLine->GetLineNumber()));
				}
				NextToken(nextLine);

				if (mType == TokenType::REGISTER)
				{
					ParseRegister(nextLine, operand);
					operand->SetType(OperandType::REGDIR_JMP);
				}
				else if (mType == TokenType::NUMERIC_LITERAL_DEC || mType == TokenType::NUMERIC_LITERAL_HEX)
				{
					ParseLiteral(nextLine, operand);
					operand->SetType(OperandType::MEMDIR_LITERAL_JMP);
				}
				else if (mType == TokenType::IDENTIFIER)
				{
					ParseSymbol(nextLine, operand);
					operand->SetType(OperandType::MEMDIR_SYMBOL_JMP);
				}
				else if (mType == TokenType::SQUARE_BRACKET_OPEN)
				{
					ParseRegisterExpression(nextLine, operand);
				}
				else
				{
					RaiseError("Incorrect syntax on line " + std::to_string(nextLine->GetLineNumber()));
				}
			}

			if (nextLine->GetInstruction() != nullptr)
			{
				nextLine->GetInstruction()->AppendOperand(*operand);
			}
			else if (nextLine->GetDirective() != nullptr)
			{
				nextLine->GetDirective()->AppendOperand(*operand);
			}

			NextToken(nextLine);

			if (mType != TokenType::COMMA)
			{
				break;
			}

			NextToken(nextLine);
		}
	}
}

void Parser::ReadEOLN(Line* nextLine)
{
	if (!IsEnd() && mTokens[mTokenIterator]->GetTokenType() == TokenType::EOLN)
	{
		mTokenIterator++;
	}
	else if (!IsEnd() && mTokens[mTokenIterator]->GetTokenType() == TokenType::EOFL)
	{
		mTokenIterator++;
		nextLine->MarkLastLine();
	}
	else if (IsEnd())
	{
		RaiseError("Unexpected end of file on line " + std::to_string(nextLine->GetLineNumber()));
	}
	else if (mTokens[mTokenIterator]->GetTokenType() == TokenType::EOLN)
	{
		RaiseError("Unexpected syntax on line" + std::to_string(nextLine->GetLineNumber()));
	}
}

void Parser::ParseLiteral(Line* nextLine, Operand* operand)
{
	int sign = (mType == TokenType::MINUS) ? -1 : 1;

	if (mType == TokenType::PLUS || mType == TokenType::MINUS)
	{
		NextToken(nextLine);
	}

	if (mType == TokenType::NUMERIC_LITERAL_DEC)
	{
		operand->SetLiteral(sign * ReadDecLiteral(mTokens[mTokenIterator]->GetTokenString()));
	}
	else if (mType == TokenType::NUMERIC_LITERAL_HEX)
	{
		operand->SetLiteral(sign * ReadHexLiteral(mTokens[mTokenIterator]->GetTokenString()));
	}
	else
	{
		RaiseError("Expected literal on line " + std::to_string(nextLine->GetLineNumber()));
	}
}

void Parser::ParseSymbol(Line* nextLine, Operand* operand)
{
	TokenType type = mTokens[mTokenIterator]->GetTokenType();

	if (type == TokenType::IDENTIFIER)
	{
		operand->SetSymbol(mTokens[mTokenIterator]->GetTokenString());
	}
	else
	{
		RaiseError("Expected literal or symbol on line " + std::to_string(nextLine->GetLineNumber()));
	}
}

void Parser::ParseRegister(Line* nextLine, Operand* operand)
{
	bool isJMPInstruction = nextLine->GetInstruction() != nullptr && nextLine->GetInstruction()->IsJumpInstruction();
	TokenType type = mTokens[mTokenIterator]->GetTokenType();

	if (type != TokenType::REGISTER)
	{
		RaiseError("Expected register on line " + std::to_string(nextLine->GetLineNumber()));
	}
	operand->SetRegister(mTokens[mTokenIterator]->GetTokenString());
}

void Parser::ParseRegisterExpression(Line* nextLine, Operand* operand)
{
	bool isJMPInstruction = nextLine->GetInstruction() != nullptr && nextLine->GetInstruction()->IsJumpInstruction();

	NextToken(nextLine);
	ParseRegister(nextLine, operand);
	NextToken(nextLine);

	if (mType == TokenType::SQUARE_BRACKET_CLOSED)
	{
		operand->SetType(isJMPInstruction? OperandType::REGIND_JMP : OperandType::REGIND);
		return;
	}
	else if (mType == TokenType::PLUS)
	{
		NextToken(nextLine);
	}
	else
	{
		RaiseError("Expected '+' on line " + std::to_string(nextLine->GetLineNumber()));
	}

	if (mType == TokenType::NUMERIC_LITERAL_DEC || mType == TokenType::NUMERIC_LITERAL_HEX)
	{
		ParseLiteral(nextLine, operand);
		operand->SetType(isJMPInstruction ? REGIND_LITERAL_JMP : REGIND_LITERAL);
	}
	else
	{
		ParseSymbol(nextLine, operand);
		operand->SetType(isJMPInstruction ? REGIND_SYMBOL_JMP : REGIND_SYMBOL);
	}

	NextToken(nextLine);

	if (mType != TokenType::SQUARE_BRACKET_CLOSED)
	{
		RaiseError("Expected ']' on line " + std::to_string(nextLine->GetLineNumber()));
	}
}


void Parser::NextToken(Line* nextLine)
{
	mTokenIterator++;
	UNEXPECTED_END_ERR_CHECK
	mType = mTokens[mTokenIterator]->GetTokenType();
}

uint16_t Parser::ReadDecLiteral(std::string literal)
{
	int retVal = stoi(literal, 0, 10);
	if (retVal > UINT16_MAX)
	{
		RaiseError("Wrong width of literal " + literal);
	}
	return (uint16_t)retVal;
}

uint16_t Parser::ReadHexLiteral(std::string literal)
{
	int retVal = stoi(literal, 0, 16);
	if (retVal > UINT16_MAX)
	{
		RaiseError("Wrong width of literal " + literal);
	}
	return (uint16_t)retVal;
}

bool Parser::IsEnd() const
{
	return mTokenIterator >= mTokens.size();
}

std::ostream& operator<<(std::ostream& os, const Operand& op)
{
	switch(op.GetType())
	{
	// Only for data movement instructions
	//
	case IMMEDIATE :
		return os << "$" << op.GetLiteral(); break; // $<literal> -> value <literal>
	case IMMEDIATE_SYMBOL_VALUE:
		return os << "$" << op.GetSymbol(); break;// $<symbol> -> address of symbol
	case MEMDIR_LITERAL:
		return os << op.GetLiteral(); break;// <literal> -> MEM[literal]
	case MEMDIR_SYMBOL_ABS:
		return os << op.GetSymbol(); break;// <symbol> -> MEM[address of symbol]
	case MEMDIR_SYMBOL_PCREL:
		return os << "%" << op.GetSymbol(); break;// %<symbol> -> MEM[PC-address of symbol]
	case REGDIR:
		return os << op.GetRegister(); break;// <reg> -> value from register
	case REGIND:
		return os << "[" << op.GetRegister() << "]"; break;// [<reg>] -> MEM[value from reg]
	case REGIND_LITERAL:
		return os << "[" << op.GetRegister() << " + "<<op.GetLiteral()<<"]"; break;// [<reg> + <literal>] -> MEM[value from reg + literal]
	case REGIND_SYMBOL:
		return os << "[" << op.GetRegister() << " + " << op.GetSymbol() << "]"; break;// [<reg> + <symbol>] -> MEM[value from reg + address of symbol]
	case IMMEDIATE_JMP:
		return os << op.GetLiteral(); break;// <literal> -> value <literal>
	case IMMEDIATE_SYMBOL_VALUE_ABS_JMP:
		return os << op.GetSymbol(); break;// <symbol> -> address of symbol with absolute adressing
	case IMMEDIATE_SYMBOL_VALUE_PCREL_JMP:
		return os << "%" << op.GetSymbol(); break;// %<symbol> -> address of symbol with pc relative adressing
	case MEMDIR_LITERAL_JMP:
		return os << "*" << op.GetLiteral(); break;// *<literal> -> MEM[literal]
	case MEMDIR_SYMBOL_JMP:
		return os << "*" << op.GetSymbol(); break;// *<symbol> -> MEM[address of symbol]
	case REGDIR_JMP:
		return os << "*" << op.GetRegister(); break;// *<reg> -> value from register
	case REGIND_JMP:
		return os << "*[" << op.GetRegister() << "]"; break;// *[<reg>] -> MEM[value from reg]
	case REGIND_LITERAL_JMP:
		return os << "*[" << op.GetRegister() << " + "<<op.GetLiteral()<< "]"; break;// *[<reg> + <literal>] -> MEM[value from reg + literal]
	case REGIND_SYMBOL_JMP:
		return os << "*[" << op.GetRegister() << " + " << op.GetSymbol() << "]"; break;// *[<reg> + <symbol>] -> MEM[value from reg + address of symbol]
	}
	return os<<"";
}


void Skip::ExecuteDirective(uint16_t lineNumber, Assembler* assm)
{
	if (mOperands.size() != 1)
	{
		RaiseError("Wrong number of operands for .skip directive on line " + std::to_string(lineNumber));
	}

	auto operand = mOperands[0];

	if (operand.GetType() != MEMDIR_LITERAL)
	{
		RaiseError("Wrong type of operands for .skip directive on line " + std::to_string(lineNumber));
	}

	assm->SetLC(assm->GetLC() + operand.GetLiteral());

	std::string currentSection = assm->GetCurrentSection();
	uint16_t currentSectionIndex = assm->GetCurrentSectionIndex();

	if (currentSectionIndex == 0)
	{
		RaiseError(".skip directive not in section on line " + std::to_string(lineNumber));
	}

	for (uint16_t i = 0; i < operand.GetLiteral(); i++)
	{
		sectionsContent[currentSection].push_back(0);
	}
}

void Word::ExecuteDirective(uint16_t lineNumber, Assembler* assm)
{
	if (mOperands.size() == 0)
	{
		RaiseError("Too few operands for .word directive on line " + std::to_string(lineNumber));
	}

	std::string currentSection = assm->GetCurrentSection();
	uint16_t currentSectionIndex = assm->GetCurrentSectionIndex();

	if (currentSectionIndex == 0)
	{
		RaiseError(".word directive not in section on line " + std::to_string(lineNumber));
	}

	for (auto operand : mOperands)
	{
		if (operand.GetType() == OperandType::MEMDIR_LITERAL)
		{
			int8_t lowerByte = (int8_t)(operand.GetLiteral() & (uint16_t)0x00FF);
			int8_t higherByte = (int8_t)((operand.GetLiteral() >> 8) & (uint16_t)0x00FF);

			sectionsContent[currentSection].push_back(lowerByte);
			sectionsContent[currentSection].push_back(higherByte);
		}
		else if(operand.GetType() == OperandType::MEMDIR_SYMBOL_ABS)
		{
			SymbolTableEntry* symbol = symtab.FindSymbol(operand.GetSymbol());

			if (symbol == nullptr)
			{
				symtab.AppendSymbol(new SymbolTableEntry(operand.GetSymbol(), LOCAL, 0, 0, false));
			}

			symbol = symtab.FindSymbol(operand.GetSymbol());
			
			sectionsContent[currentSection].push_back(0);
			sectionsContent[currentSection].push_back(0);

			assm->AddRelocationEntry(symbol, assm->GetLC(), false);
		}
		else 
		{
			RaiseError("Wrong addressing of operand in .word directive on line " + std::to_string(lineNumber));
		}

		assm->SetLC(assm->GetLC() + 2);
	}
}

void Section::ExecuteDirective(uint16_t lineNumber, Assembler* assm)
{
	if (mOperands.size() != 1)
	{
		RaiseError("Wrong number of operands for .section directive on line " + std::to_string(lineNumber));
	}
	auto operand = mOperands[0];

	if (operand.GetType() != MEMDIR_SYMBOL_ABS)
	{
		RaiseError("Wrong type of operands for .section directive on line " + std::to_string(lineNumber));
	}

	std::string currentSection = assm->GetCurrentSection();
	uint16_t currentSectionIndex = assm->GetCurrentSectionIndex();

	if (currentSectionIndex > 0)
	{
		sctHdrTab.FindSection(currentSectionIndex)->IncreaseLength(assm->GetLC());
	}

	SymbolTableEntry* sectionInSymtab = symtab.FindSymbol(operand.GetSymbol());

	if (sectionInSymtab == nullptr)
	{
		symtab.AppendSymbol(new SymbolTableEntry(operand.GetSymbol(), LOCAL, 0, currentSectionIndex + 1, true, SECTION));
		sctHdrTab.AppendSection(new SectionHeaderTableEntry(operand.GetSymbol(), 0));

		assm->SetCurrentSection(operand.GetSymbol());
		assm->SetCurrentSectionIndex(currentSectionIndex + 1);
	}
	else
	{
		assm->SetCurrentSection(operand.GetSymbol());
		assm->SetCurrentSectionIndex(sctHdrTab.GetSectionIndex(operand.GetSymbol()));
	}

	assm->SetLC(0);
}

void Extern::ExecuteDirective(uint16_t lineNumber, Assembler* assm)
{
	if (mOperands.size() == 0)
	{
		RaiseError("Too few operands for .global directive on line " + std::to_string(lineNumber));
	}

	for (auto operand : mOperands)
	{
		if (operand.GetType() != MEMDIR_SYMBOL_ABS)
		{
			RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
		}

		SymbolTableEntry* symbol = symtab.FindSymbol(operand.GetSymbol());

		if (symbol == nullptr) // First encounter of symbol->Ok
		{
			symtab.AppendSymbol(new SymbolTableEntry(operand.GetSymbol(), GLOBAL, 0, 0, false, OTHER, IMPORTED));
		}
		else if (symbol->GetImportExport() == EXPORTED) // Symbol defined both as extern and global->Error
		{
			RaiseError("External symbol " + operand.GetSymbol() + " defined as global on line" + std::to_string(lineNumber));
		}
		else if (symbol->GetDefined() == true)
		{
			RaiseError("External symbol " + operand.GetSymbol() + " should not be defined in this file");
		}
		else
		{
			symbol->SetImportExport(IMPORTED);
			symbol->SetVisibility(GLOBAL);
		}
	}
}

void Global::ExecuteDirective(uint16_t lineNumber, Assembler* assm)
{
	if (mOperands.size() == 0)
	{
		RaiseError("Too few operands for .global directive on line " + std::to_string(lineNumber));
	}

	for (auto operand : mOperands)
	{
		if (operand.GetType() != MEMDIR_SYMBOL_ABS)
		{
			RaiseError("Wrong type of addressing on line " + std::to_string(lineNumber));
		}

		SymbolTableEntry* symbol = symtab.FindSymbol(operand.GetSymbol());

		if (symbol == nullptr) // First encounter of symbol->Ok
		{
			symtab.AppendSymbol(new SymbolTableEntry(operand.GetSymbol(), GLOBAL, 0, 0, false, OTHER, EXPORTED));
		}
		else if (symbol->GetImportExport() == IMPORTED) // Symbol defined both as extern and global->Error
		{
			RaiseError("External symbol " + operand.GetSymbol()+" defined as global on line" + std::to_string(lineNumber));
		}
		else // Symbol have definition->Ok
		{
			symbol->SetImportExport(EXPORTED);
			symbol->SetVisibility(GLOBAL);
		}
	}
}

void End::ExecuteDirective(uint16_t lineNumber, Assembler* assm)
{
	isEnd = true;
}
