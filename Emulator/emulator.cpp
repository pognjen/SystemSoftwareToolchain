#include "emulator.h"
#include "error.h"
#include <fstream>
#include <bitset>
#include <iomanip>

#define PC 7
#define SP 6
#define PSW 8

#define Z 0
#define O 1
#define C 2
#define N 3

#define I 15

int8_t memory[65536];


// [0-5] -> r0-r5
// [6] -> pc
// [7] -> sp
// [8] -> psw
uint16_t regs[9] = { 0,0,0,0,0,0,0,0,0 };

bool haltInstruction = false;

void CheckPC()
{
	if (regs[PC] == 0xFFFF)
	{
		RaiseError("Cannot increment PC anymore - overflow");
	}
}

Operand* ReadSecondRegister()
{
	Operand* op = nullptr;
	//CheckPC();
	uint8_t regsDesc = memory[regs[PC] - 1];

	uint16_t registerNumber = (regsDesc) & 0x0F;

	if (registerNumber <= 8)
	{
		op = new Operand();
		op->SetType(REGDIR);
		op->SetRegister(registerNumber);
		op->SetOperandValue(regs[registerNumber]);
	}

	return op;
}


Operand* ReadSecondOperand(bool isStr = false)
{
	Operand* op = nullptr;
	CheckPC();
	uint8_t addrMode = memory[regs[PC]++];
	uint8_t dataLow = 0, dataHigh = 0;
	uint8_t reg = 0;

	switch (addrMode)
	{
		// Address/literal is in instruction payload
		//
	case  IMMEDIATE:
	case  IMMEDIATE_SYMBOL_VALUE:
	case  IMMEDIATE_JMP:
	case  IMMEDIATE_SYMBOL_VALUE_ABS_JMP:
	case  IMMEDIATE_SYMBOL_VALUE_PCREL_JMP:
		CheckPC();
		dataLow = memory[regs[PC]++];
		CheckPC();
		dataHigh = memory[regs[PC]++];
		op = new Operand();
		op->SetType((OperandType)addrMode);
		op->SetOperandValue(((uint16_t)dataHigh << 8) | ((uint16_t)dataLow & 0x00FF));
		break;
	case  MEMDIR_LITERAL:
	case  MEMDIR_SYMBOL_ABS:
	case  MEMDIR_SYMBOL_PCREL:
	case  MEMDIR_LITERAL_JMP:
	case  MEMDIR_SYMBOL_JMP:
		CheckPC();
		dataLow = memory[regs[PC]++];
		CheckPC();
		dataHigh = memory[regs[PC]++];
		if (isStr)
		{
			// In case of str, we store address as operand value
			//
			op = new Operand();
			op->SetType((OperandType)addrMode);
			op->SetOperandValue(((uint16_t)dataHigh << 8) | ((uint16_t)dataLow & 0x00FF));
		}
		else
		{
			// In other cases, we store what it is on that address in memory operand value
			//
			uint16_t address = ((uint16_t)(dataHigh << 8)) | ((uint16_t)dataLow & 0x00FF);
			
			if (addrMode == MEMDIR_SYMBOL_PCREL)
			{
				address = regs[PC] + address;
			}
			uint16_t lower = ((uint16_t)memory[address] & 0x00FF);
			uint16_t higher = ((uint16_t)memory[address + 1] & 0x00FF) << 8;

			uint16_t value = lower | higher;
			op = new Operand();
			op->SetType((OperandType)addrMode);
			op->SetOperandValue(value);
		}
		break;
	case  REGDIR:
	case  REGDIR_JMP:
		reg = memory[regs[PC] - 2] & 0x0F;
		op = new Operand();
		op->SetType((OperandType)addrMode);
		op->SetRegister(reg);
		op->SetOperandValue(regs[reg]);
		break;

	case  REGIND:
	case  REGIND_JMP:
		reg = memory[regs[PC] - 2] & 0x0F;
		if (isStr)
		{
			op = new Operand();
			op->SetType((OperandType)addrMode);
			op->SetRegister(reg);
			op->SetOperandValue(regs[reg]);
		}
		else
		{
			if (regs[reg] == 0xFFFF)
			{
				RaiseError("Cannot read memory at address 0xFFFF - overflow");
			}

			uint16_t lower = ((uint16_t)memory[regs[reg]] & 0x00FF);
			uint16_t higher = ((uint16_t)memory[regs[reg] + 1] & 0x00FF) << 8;

			uint16_t value = lower | higher;
			op = new Operand();
			op->SetType((OperandType)addrMode);
			op->SetRegister(reg);
			op->SetOperandValue(value);
		}
		break;
	case  REGIND_LITERAL:
	case  REGIND_SYMBOL:
	case  REGIND_LITERAL_JMP:
	case  REGIND_SYMBOL_JMP:
		reg = memory[regs[PC] - 2] & 0x0F;
		CheckPC();
		dataLow = memory[regs[PC]++];
		CheckPC();
		dataHigh = memory[regs[PC]++];
		if (isStr)
		{
			op = new Operand();
			op->SetRegister(reg);
			op->SetType((OperandType)addrMode);
			uint16_t address = ((uint16_t)dataHigh << 8) | ((uint16_t)dataLow) + regs[reg];
			op->SetOperandValue(address);
		}
		else
		{
			op = new Operand();
			op->SetRegister(reg);
			op->SetType((OperandType)addrMode);
			uint16_t address = ((uint16_t)dataHigh << 8) | ((uint16_t)dataLow)  + regs[reg];

			uint16_t lower = ((uint16_t)memory[address] & 0x00FF);
			uint16_t higher = ((uint16_t)memory[address + 1] & 0x00FF) << 8;

			uint16_t value = lower | higher;
			if (address == 0xFFFF)
			{
				RaiseError("Cannot read memory at address 0xFFFF - overflow");
			}
			op->SetOperandValue(value);
		}
		break;
	}

	return op;
}

Operand* ReadFirstOperand()
{
	Operand* op = nullptr;
	CheckPC();
	uint8_t regsDesc = memory[regs[PC]++]; 

	uint16_t registerNumber = (regsDesc >> 4) & 0x0F;

	if (registerNumber <= 8)
	{
		op = new Operand();
		op->SetType(REGDIR);
		op->SetRegister(registerNumber);
		op->SetOperandValue(regs[registerNumber]);
	}

	return op;
}

void SetPSWFlag(int bit)
{
	regs[PSW] |= (1 << bit);
}

void UnsetPSWFlag(int bit)
{
	regs[PSW] &= ~(1 << bit);
}

bool TestPSWFlag(int bit)
{
	return (regs[PSW] & (1 << bit)) != 0;
}

void Emulator::ReadMemoryContent(std::string& inputFile)
{
	std::ifstream input;
	input.open(inputFile, std::ios::binary | std::ios::in);

	if (!input.is_open() || !input.good())
	{
		RaiseError("Error opening " + inputFile);
	}

	size_t size;
	input.read((char*)&size, sizeof(size_t));

	for (int i = 0; i < size; i++)
	{
		uint16_t addr;
		uint8_t byte;
		input.read((char*)&addr, sizeof(uint16_t));
		input.read((char*)&byte, sizeof(uint8_t));

		memory[addr] = byte;
	}
}

void Emulator::Init()
{
	regs[PC] = ((uint16_t)memory[0] & 0x00FF) | ((((uint16_t)memory[1]) << 8) & 0xFF00);
}

void Emulator::Run()
{
	while (!haltInstruction)
	{
		Instruction* instruction = ReadInstruction();
		
		// In case of wrong opcode
		//
		if (instruction == nullptr)
		{
			regs[PC] = ((uint16_t)memory[2] & 0x00FF) | ((((uint16_t)memory[3]) << 8) & 0xFF00);
		}
		else
		{
			// At this moment, PC points to next instruction
			//
			instruction->Execute();
		}
	}
}

void Emulator::OutputResult()
{
	std::bitset<16> pswRepresentation = regs[PSW];
	std::cout<<"\n---------------------------------------------------------\n";
	std::cout<<"Emulated processor executed halt instruction\n";
	std::cout<<"Emulated processor state: psw=0b"<<pswRepresentation<<"\n";

	for (int i=0;i<4;i++)
	{
				std::cout<<"r"<<i<<"=";
				char hex_string[20];
				sprintf(hex_string, "%.4X", regs[i]);
				std::string regHex = hex_string;
				regHex = regHex.substr(regHex.size() - 4);
				regHex.insert(0, "0x");
				std::cout << std::setw(4) << regHex <<"\t";
	}

	std::cout<<'\n';

		for (int i=4;i<8;i++)
	{
				std::cout<<"r"<<i<<"=";
				char hex_string[20];
				sprintf(hex_string, "%.4X", regs[i]);
				std::string regHex = hex_string;
				regHex = regHex.substr(regHex.size() - 4);
				regHex.insert(0, "0x");
				std::cout << std::setw(4) << regHex <<"\t";
	}

	std::cout<<'\n';
}

Instruction* Emulator::ReadInstruction()
{
	CheckPC();
	uint8_t opCode = memory[regs[PC]++];
	Instruction* instruction = nullptr;
	Operand* operand1 = nullptr;
	Operand* operand2 = nullptr;
	Operand* operand = nullptr;

	switch (opCode)
	{
	case 0xB0://str rx, operand
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondOperand(true);
		if (operand1 && operand2 && operand2->GetType() != IMMEDIATE && operand2->GetType() != IMMEDIATE_SYMBOL_VALUE)
		{
			instruction = new Str();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0xA0://ldr
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondOperand();
		if (operand1 && operand2)
		{
			instruction = new Ldr();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x91://shr
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Shr();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x90://shl
		 operand1 = ReadFirstOperand();
		 operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Shl();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x84://test
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Test();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x83://xor
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Xor();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x82://or
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Or();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x81://and
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new And();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x80://not
		operand1 = ReadFirstOperand();
		if (operand1)
		{
			instruction = new Not();
			instruction->AppendOperand(operand1);
		}
		break;
	case 0x74://cmp
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Cmp();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x73://div
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Div();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x72://mul
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Mul();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x71://sub
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Sub();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x70://add
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Add();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0x60://xchg
		operand1 = ReadFirstOperand();
		operand2 = ReadSecondRegister();
		if (operand1 && operand2)
		{
			instruction = new Xchg();
			instruction->AppendOperand(operand1);
			instruction->AppendOperand(operand2);
		}
		break;
	case 0xF0://pop
		operand1 = ReadFirstOperand();
		if (operand1)
		{
			instruction = new Pop();
			instruction->AppendOperand(operand1);
		}
		break;
	case 0xE0://push
		operand1 = ReadFirstOperand();
		if (operand1)
		{
			instruction = new Push();
			instruction->AppendOperand(operand1);
		}
		break;
	case 0x53://jgt
		// Just to move PC
		//
		ReadFirstOperand();
		operand = ReadSecondOperand();
		if (operand && operand->GetType() >= 9)
		{
			instruction = new Jgt();
			instruction->AppendOperand(operand);
		}
		break;
	case 0x52://jne
		ReadFirstOperand();
		operand = ReadSecondOperand();
		if (operand && operand->GetType() >= 9)
		{
			instruction = new Jne();
			instruction->AppendOperand(operand);
		}
		break;
	case 0x51://jeq
		ReadFirstOperand();
		operand = ReadSecondOperand();
		if (operand && operand->GetType() >= 9)
		{
			instruction = new Jeq();
			instruction->AppendOperand(operand);
		}
		break;
	case 0x50://jmp
		ReadFirstOperand();
		operand = ReadSecondOperand();
		if (operand && operand->GetType() >= 9)
		{
			instruction = new Jmp();
			instruction->AppendOperand(operand);
		}
		break;
	case 0x40://ret
		instruction = new Ret();
		break;
	case 0x30://call
		ReadFirstOperand();
		operand = ReadSecondOperand();
		if (operand && operand->GetType() >= 9)
		{
			instruction = new Call();
			instruction->AppendOperand(operand);
		}
		break;
	case 0x20://iret
		instruction = new Iret();
		break;
	case 0x10://int
		operand = ReadFirstOperand();
		if (operand)
		{
			instruction = new Int();
			instruction->AppendOperand(operand);
		}
		break;
	case 0x00://halt
		instruction = new Halt();
		break;
	}

	return instruction;
}


void Operand::SetRegister(uint16_t reg)
{
	mRegister = reg;
}

void Operand::SetOperandValue(uint16_t value)
{
	operandValue = value;
}

Operand::Operand()
{
	mLiteral = 0;
	mSymbol = "";
	mRegister = 0;
	operandValue = 0;
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

uint16_t Operand::GetRegister() const
{
	return mRegister;
}

uint16_t Operand::GetOperandValue() const
{
	return operandValue;
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

void Instruction::AppendOperand(Operand* operand)
{
	operands.push_back(operand);
}

void Halt::Execute()
{
	// End execution
	//
	haltInstruction = true;
}

void Add::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	regs[regD] = operands[0]->GetOperandValue() + operands[1]->GetOperandValue();
}

void Sub::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	regs[regD] = operands[0]->GetOperandValue() - operands[1]->GetOperandValue();
}

void Mul::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	regs[regD] = operands[0]->GetOperandValue() * operands[1]->GetOperandValue();
}

void Div::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	regs[regD] = operands[0]->GetOperandValue() / operands[1]->GetOperandValue();
}

void Not::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	regs[regD] = ~operands[0]->GetOperandValue();
}

void And::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	regs[regD] = operands[0]->GetOperandValue() & operands[1]->GetOperandValue();
}

void Or::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	regs[regD] = operands[0]->GetOperandValue() | operands[1]->GetOperandValue();
}

void Xor::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	regs[regD] = operands[0]->GetOperandValue() ^ operands[1]->GetOperandValue();
}

void Xchg::Execute()
{
	uint8_t regD = operands[0]->GetRegister();
	uint8_t regS = operands[1]->GetRegister();

	uint8_t temp = regs[regD];
	regs[regD] = regs[regS];
	regs[regS] = temp;
}

void Push::Execute()
{
	regs[SP] -= 2;

	uint8_t low = operands[0]->GetOperandValue() & 0x00FF;
	uint8_t high = (operands[0]->GetOperandValue() >> 8) & 0x00FF;

	memory[regs[SP]] = low;
	memory[regs[SP] + 1] = high;
}

void Pop::Execute()
{
	uint16_t regD = operands[0]->GetRegister();

	uint16_t value = (((uint16_t)(memory[regs[SP]])) & 0x00FF) | (((uint16_t)memory[regs[SP] + 1] << 8)& 0xFF00);
	
	regs[regD] = value;

	regs[SP] += 2;
}

void Test::Execute()
{
	uint16_t value = (operands[0]->GetOperandValue()) & (operands[1]->GetOperandValue());

	if (value == 0)
	{
		SetPSWFlag(Z);
	}
	else
	{
		UnsetPSWFlag(Z);
	}

	if (value & ((uint16_t)1 << 15))
	{
		SetPSWFlag(N);
	}
	else
	{
		UnsetPSWFlag(N);
	}
}

void Shl::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	uint16_t value = operands[0]->GetOperandValue() << operands[1]->GetOperandValue();

	regs[regD] = value;

	if (value == 0)
	{
		SetPSWFlag(Z);
	}
	else
	{
		UnsetPSWFlag(Z);
	}

	if (value & ((uint16_t)1 << 15))
	{
		SetPSWFlag(N);
	}
	else
	{
		UnsetPSWFlag(N);
	}

	if (operands[1]->GetOperandValue() == 0 || operands[1]->GetOperandValue() > 16)
	{
		UnsetPSWFlag(C);
	}

	if (operands[0]->GetOperandValue() & (1 << (16 - operands[1]->GetOperandValue())))
	{
		SetPSWFlag(C);
	}
	else
	{
		UnsetPSWFlag(C);
	}

	regs[regD] = value;
}

void Shr::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	uint16_t value = operands[0]->GetOperandValue() >> operands[1]->GetOperandValue();
	
	regs[regD] = value;

	if (value == 0)
	{
		SetPSWFlag(Z);
	}
	else
	{
		UnsetPSWFlag(Z);
	}

	if (value & ((uint16_t)1 << 15))
	{
		SetPSWFlag(N);
	}
	else
	{
		UnsetPSWFlag(N);
	}

	if (operands[1]->GetOperandValue() == 0)
	{
		UnsetPSWFlag(C);
	}

	if (operands[1]->GetOperandValue() > 16)
	{
		if (operands[0]->GetOperandValue() & 0x8000)
		{
			SetPSWFlag(C);
		}
		else
		{
			UnsetPSWFlag(C);
		}
	}
	
	if (operands[0]->GetOperandValue() & (1 << (operands[1]->GetOperandValue() - 1)))
	{
		SetPSWFlag(C);
	}
	else
	{
		UnsetPSWFlag(C);
	}
}

void Cmp::Execute()
{
	int16_t regD = operands[0]->GetOperandValue();
	int16_t regS = operands[1]->GetOperandValue();
	int16_t temp = regD - regS;

	// Update Z
	//
	if (temp == 0)
	{
		SetPSWFlag(Z);
	}
	else
	{
		UnsetPSWFlag(Z);
	}

	//Update N
	// 
	if (temp < 0)
	{
		SetPSWFlag(N);
	}
	else
	{
		UnsetPSWFlag(N);
	}

	// Update O
	//
	if ((regD < 0 && -regS < 0 && temp > 0) || (regD > 0 && -regS > 0 && temp < 0))
	{
		SetPSWFlag(O);
	}
	else
	{
		UnsetPSWFlag(O);
	}

	// Update carry
	//
	int16_t carry_bit = 0;
	for (int i = 0; i < 16; i++)
	{
		int16_t regDbit = regD & (1 << i);
		int16_t regSbit = regS & (1 << i);
		if (regDbit + regSbit + carry_bit >= 2)
		{
			carry_bit = 1;
		}
		else
		{
			carry_bit = 0;
		}
	}

	if (carry_bit == 1)
	{
		SetPSWFlag(C);
	}
	else
	{
		UnsetPSWFlag(C);
	}
}

void Iret::Execute()
{
	// Pop PC
	//
	uint16_t value = ((uint16_t)(memory[regs[SP]]) & 0x00FF) | ((uint16_t)memory[regs[SP] + 1] << 8);

	regs[PC] = value;

	regs[SP] += 2;

	{
		// Pop PSW
		//
		uint16_t regD = 6;

		uint16_t value = ((uint16_t)(memory[regs[SP]])) | ((uint16_t)memory[regs[SP] + 1] << 8);

		regs[regD] = value;

		regs[PSW] += 2;

		UnsetPSWFlag(I);
	}
}

void Int::Execute()
{
	// Push PSW
	//
	regs[SP] -= 2;

	uint8_t low = regs[PSW] & 0x00FF;
	uint8_t high = (regs[PSW] >> 8) & 0x00FF;

	memory[regs[SP]] = low;
	memory[regs[SP] + 1] = high;

	{
		// Push PC
		//
		regs[SP] -= 2;

		uint8_t low = regs[PC] & 0x00FF;
		uint8_t high = (regs[PC] >> 8) & 0x00FF;

		memory[regs[SP]] = low;
		memory[regs[SP] + 1] = high;

		SetPSWFlag(I);

		uint16_t dLow = (uint16_t)(memory[(operands[0]->GetOperandValue() % 8) * 2]) & 0x00FF;
		uint16_t dHigh = ((uint16_t)(memory[(operands[0]->GetOperandValue() % 8) * 2 + 1]) << 8) & 0xFF00;

		regs[PC] = dLow | dHigh;
	}
}

void Call::Execute()
{
	// Push PC
	//
	regs[SP] -= 2;

	uint8_t low = regs[PC];
	uint8_t high = regs[PC] >> 8;

	memory[regs[SP]] = low;
	memory[regs[SP] + 1] = high;

	if (operands[0]->GetType() == IMMEDIATE_SYMBOL_VALUE_PCREL_JMP)
	{
		uint32_t res = ((uint32_t)regs[PC] + (uint32_t)operands[0]->GetOperandValue()) % 65536;
		regs[PC] = (uint16_t)res;
	}
	else
	{
		regs[PC] = operands[0]->GetOperandValue();
	}
}

void Ret::Execute()
{
	uint16_t value = ((uint16_t)(memory[regs[SP]]) & 0x00FF) | ((uint16_t)memory[regs[SP] + 1] << 8);

	regs[PC] = value;

	regs[SP] += 2;
}

void Jmp::Execute()
{
	if (operands[0]->GetType() == IMMEDIATE_SYMBOL_VALUE_PCREL_JMP )
	{
		uint32_t res = ((uint32_t)regs[PC] + (uint32_t)operands[0]->GetOperandValue()) % 65536;
		regs[PC] = (uint16_t)res;
	}
	else
	{
		regs[PC] = operands[0]->GetOperandValue();
	}
}

void Jeq::Execute()
{
	if (TestPSWFlag(Z))
	{
		if (operands[0]->GetType() == IMMEDIATE_SYMBOL_VALUE_PCREL_JMP)
		{
			uint32_t res = ((uint32_t)regs[PC] + (uint32_t)operands[0]->GetOperandValue()) % 65536;
			regs[PC] = (uint16_t)res;
		}
		else
		{
			regs[PC] = operands[0]->GetOperandValue();
		}
	}
}

void Jne::Execute()
{
	if (!TestPSWFlag(Z))
	{
		if (operands[0]->GetType() == IMMEDIATE_SYMBOL_VALUE_PCREL_JMP)
		{
			uint32_t res = ((uint32_t)regs[PC] + (uint32_t)operands[0]->GetOperandValue()) % 65536;
			regs[PC] = (uint16_t)res;
		}
		else
		{
			regs[PC] = operands[0]->GetOperandValue();
		}
	}
}

void Jgt::Execute()
{
	if (!TestPSWFlag(Z) && !TestPSWFlag(N))
	{
		if (operands[0]->GetType() == IMMEDIATE_SYMBOL_VALUE_PCREL_JMP)
		{
			uint32_t res = ((uint32_t)regs[PC] + (uint32_t)operands[0]->GetOperandValue()) % 65536;
			regs[PC] = (uint16_t)res;
		}
		else
		{
			regs[PC] = operands[0]->GetOperandValue();
		}
	}
}

void Ldr::Execute()
{
	uint8_t regD = operands[0]->GetRegister();
	uint8_t addrMode = operands[1]->GetType();

	if (addrMode == MEMDIR_SYMBOL_PCREL)
	{
		regs[regD] = operands[1]->GetOperandValue();//ipsravi ovo
	}
	else
	{
		regs[regD] = operands[1]->GetOperandValue();
	}
}

void Str::Execute()
{
	uint8_t regD = operands[0]->GetRegister();

	uint8_t addrMode = operands[1]->GetType();

	// Store regD to memory
	//
	if (addrMode == MEMDIR_SYMBOL_PCREL)
	{
		uint8_t dataLow = regs[regD] & 0x00FF;
		uint8_t dataHigh = (regs[regD] >> 8) & 0x00FF;
		memory[(regs[PC] + operands[1]->GetOperandValue()) % 65536] = dataLow;
		memory[(regs[PC] + operands[1]->GetOperandValue() + 1) % 65536] = dataHigh;
	}
	else if (addrMode == MEMDIR_LITERAL || addrMode == MEMDIR_SYMBOL_ABS || addrMode == MEMDIR_SYMBOL_PCREL || addrMode == REGIND ||
		addrMode == REGIND_LITERAL || addrMode == REGIND_SYMBOL)
	{
		uint8_t dataLow = regs[regD] & 0x00FF;
		uint8_t dataHigh = (regs[regD] >> 8) & 0x00FF;
		memory[(operands[1]->GetOperandValue() % 65536)] = dataLow;
		memory[(operands[1]->GetOperandValue() + 1) % 65536] = dataHigh;
	}
	else if (addrMode == REGDIR)
	{
		regs[operands[1]->GetRegister()] = regs[regD];
	}
}