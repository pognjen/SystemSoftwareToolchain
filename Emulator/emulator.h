#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <string>
#include <vector>

class Operand;
class Instruction;

class Emulator
{
public:
	void ReadMemoryContent(std::string& inputFile);

	void Init();

	void Run();

	void OutputResult();
private:
	Instruction* ReadInstruction();
};

enum OperandType
{
	// Only for ldr/strinstructions
	//
	IMMEDIATE, // $<literal> -> value <literal>
	IMMEDIATE_SYMBOL_VALUE, // $<symbol> -> address of symbol
	MEMDIR_LITERAL, // <literal> -> MEM[literal]
	MEMDIR_SYMBOL_ABS, // <symbol> -> MEM[address of symbol]
	MEMDIR_SYMBOL_PCREL, // %<symbol> -> MEM[PC-address of symbol]
	REGDIR, // <reg> -> value from register
	REGIND, // [<reg>] -> MEM[value from reg]
	REGIND_LITERAL, // [<reg> + <literal>] -> MEM[value from reg + literal]
	REGIND_SYMBOL, // [<reg> + <symbol>] -> MEM[value from reg + address of symbol]


	// Only for jump instructions
	//
	IMMEDIATE_JMP, // <literal> -> value <literal>
	IMMEDIATE_SYMBOL_VALUE_ABS_JMP, // <symbol> -> address of symbol with absolute adressing
	IMMEDIATE_SYMBOL_VALUE_PCREL_JMP, // %<symbol> -> address of symbol with pc relative adressing
	MEMDIR_LITERAL_JMP, // *<literal> -> MEM[literal]
	MEMDIR_SYMBOL_JMP, // *<symbol> -> MEM[address of symbol]
	REGDIR_JMP, // *<reg> -> value from register
	REGIND_JMP, // *[<reg>] -> MEM[value from reg]
	REGIND_LITERAL_JMP, // *[<reg> + <literal>] -> MEM[value from reg + literal]
	REGIND_SYMBOL_JMP, // *[<reg> + <symbol>] -> MEM[value from reg + address of symbol]
};

class Instruction
{
public:
	virtual void Execute() = 0;
	void AppendOperand(Operand* operand);
protected:
	std::vector<Operand*> operands;
};

class Operand
{
public:
	Operand();

	uint16_t GetLiteral() const;
	std::string GetSymbol() const;
	OperandType GetType() const;
	uint16_t GetRegister() const;
	uint16_t GetOperandValue() const;

	void SetType(OperandType type);
	void SetLiteral(uint16_t literal);
	void SetSymbol(std::string symbol);
	void SetRegister(uint16_t reg);
	void SetOperandValue(uint16_t value);
private:
	OperandType mOperandType;
	uint16_t mLiteral;
	std::string mSymbol;
	uint16_t mRegister;

	// Operand value, regardless of type, will be placed here
	//
	uint16_t operandValue;
};

class Halt : public Instruction
{
public:
	void Execute() override;
};

class Int : public Instruction
{
public:
	void Execute() override;
};

class Iret : public Instruction
{
public:
	void Execute() override;
};

class Call : public Instruction
{
public:
	void Execute() override;
};

class Ret : public Instruction
{
public:
	void Execute() override;
};

class Jmp : public Instruction
{
public:
	void Execute() override;
};

class Jeq : public Instruction
{
public:
	void Execute() override;
};

class Jne : public Instruction
{
public:
	void Execute() override;
};

class Jgt : public Instruction
{
public:
	void Execute() override;
};

class Push : public Instruction
{
public:
	void Execute() override;
};

class Pop : public Instruction
{
public:
	void Execute() override;
};

class Xchg : public Instruction
{
public:
	void Execute() override;
};

class Add : public Instruction
{
public:
	void Execute() override;
};

class Sub : public Instruction
{
public:
	void Execute() override;
};

class Mul : public Instruction
{
public:
	void Execute() override;
};

class Div : public Instruction
{
public:
	void Execute() override;
};

class Cmp : public Instruction
{
public:
	void Execute() override;
};

class Not : public Instruction
{
public:
	void Execute() override;
};

class And : public Instruction
{
public:
	void Execute() override;
};

class Or : public Instruction
{
public:
	void Execute() override;
};

class Xor : public Instruction
{
public:
	void Execute() override;
};

class Test : public Instruction
{
public:
	void Execute() override;
};

class Shl : public Instruction
{
public:
	void Execute() override;
};

class Shr : public Instruction
{
public:
	void Execute() override;
};

class Ldr : public Instruction
{
public:
	void Execute() override;
};

class Str : public Instruction
{
public:
	void Execute() override;
};
#endif