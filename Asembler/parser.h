#ifndef _PARSER_H
#define _PARSER_H

#include "lexer.h"

class Line;
class Instruction;
class Directive;
class Operand;
class Assembler;

class Parser
{
public:
	Parser(std::vector<Token*> tokens);
	std::vector<Line*> GetLineList();
private:
	Line* GetNextLine();
	
	void ReadLabel(Line* nextLine);
	void ReadCommand(Line* nextLine);
	void ReadOperandList(Line* nextLine);
	void ReadEOLN(Line* nextLine);

	void ParseLiteral(Line* nextLine, Operand* operand);
	void ParseSymbol(Line* nextLine, Operand* operand);
	void ParseRegister(Line* nextLine, Operand* operand);
	void ParseRegisterExpression(Line* nextLine, Operand* operand);

	void NextToken(Line* nextLine);
	uint16_t ReadDecLiteral(std::string literal);
	uint16_t ReadHexLiteral(std::string literal);
	bool IsEnd() const;
	static int LinesSoFar;
	int mTokenIterator;
	TokenType mType;
	std::vector<Token*> mTokens;
};

class Line
{
public:
	Line();
	Line(Instruction* instruction, Directive* directive, std::string label, int lineNumber);

	Instruction* GetInstruction() const;
	Directive* GetDirective() const;
	std::string GetLabel() const;
	int GetLineNumber() const;
	bool IsLastLine() const;

	void MarkLastLine();
	void SetInstruction(Instruction* instruction);
	void SetDirective(Directive* directive);
	void SetLabel(std::string label);
	void SetLineNumber(int lineNumber);
private:
	Instruction* mInstruction;
	Directive* mDirective;
	std::string mLabel;
	int mLineNumber;
	bool mLastLine;
};

class Instruction
{
public:
	Instruction(int expectedOperandsNumber, std::string instructionString);
	virtual void EncodeInstruction(uint16_t lineNumber, Assembler* assm) = 0;
	bool AppendOperand(Operand operand);
	static Instruction* CreateInstructionBasedOnString(std::string);
	bool IsJumpInstruction() const;
	std::string GetInstructionString() const;
	std::vector<Operand> GetOperands() const;
	int GetExpectedNumberOfOperands() const;
	void EncodeJumpInstruction(uint16_t lineNumber, Assembler* assm, int8_t opcode);
	void EncodeDataMovementInstruction(uint16_t lineNumber, Assembler* assm, int8_t opcode);
protected:
	std::vector<Operand> mOperands;
	std::string instruction;
	int mExpectedOperandsNumber;
};

class Directive
{
public:
	Directive(std::string directive);
	void AppendOperand(Operand operand);
	virtual void ExecuteDirective(uint16_t lineNumber, Assembler* assm) = 0;
	static Directive* CreateDirectiveBasedOnString(std::string);
	std::string GetDirectiveString() const;
	std::vector<Operand> GetOperands() const;
	bool IsEnd() const;
protected:
	bool isEnd = false;
	std::vector<Operand> mOperands;
	std::string directive;
};

class Global : public Directive
{
public:
	Global() : Directive(".global"){}
	void ExecuteDirective(uint16_t lineNumber, Assembler* assm) override;
};

class Extern : public Directive
{
public:
	Extern() : Directive(".extern") {}
	void ExecuteDirective(uint16_t lineNumber, Assembler* assm) override;
};

class Section : public Directive
{
public:
	Section() : Directive(".section") {}
	void ExecuteDirective(uint16_t lineNumber, Assembler* assm) override;
};

class Word : public Directive
{
public:
	Word() : Directive(".word") {}
	void ExecuteDirective(uint16_t lineNumber, Assembler* assm) override;
};

class Skip : public Directive
{
public:
	Skip() : Directive(".skip") {}
	void ExecuteDirective(uint16_t lineNumber, Assembler* assm) override;
};

class End : public Directive
{
public:
	End() : Directive(".end") {}
	void ExecuteDirective(uint16_t lineNumber, Assembler* assm) override;
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

class Operand
{
public:
	Operand();

	uint16_t GetLiteral() const;
	std::string GetSymbol() const;
	OperandType GetType() const;
	std::string GetRegister() const;

	void SetType(OperandType type);
	void SetLiteral(uint16_t literal);
	void SetSymbol(std::string symbol);
	void SetRegister(std::string registerString);

	friend std::ostream& operator<<(std::ostream& os, const Operand& op);
private:
	OperandType mOperandType;
	uint16_t mLiteral;
	std::string mSymbol;
	std::string mRegister; // register string will be placed here
};

class Halt : public Instruction
{
public:
	Halt(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Int : public Instruction
{
public:
	Int(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Iret : public Instruction
{
public:
	Iret(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Call : public Instruction
{
public:
	Call(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Ret : public Instruction
{
public:
	Ret(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Jmp : public Instruction
{
public:
	Jmp(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Jeq : public Instruction
{
public:
	Jeq(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Jne : public Instruction
{
public:
	Jne(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Jgt : public Instruction
{
public:
	Jgt(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Push : public Instruction
{
public:
	Push(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Pop : public Instruction
{
public:
	Pop(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Xchg : public Instruction
{
public:
	Xchg(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Add : public Instruction
{
public:
	Add(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Sub : public Instruction
{
public:
	Sub(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Mul : public Instruction
{
public:
	Mul(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Div : public Instruction
{
public:
	Div(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Cmp : public Instruction
{
public:
	Cmp(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Not : public Instruction
{
public:
	Not(int expectedOperandNumber);
	void  EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class And : public Instruction
{
public:
	And(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Or : public Instruction
{
public:
	Or(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Xor : public Instruction
{
public:
	Xor(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Test : public Instruction
{
public:
	Test(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Shl : public Instruction
{
public:
	Shl(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Shr : public Instruction
{
public:
	Shr(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Ldr : public Instruction
{
public:
	Ldr(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

class Str : public Instruction
{
public:
	Str(int expectedOperandNumber);
	void EncodeInstruction(uint16_t lineNumber, Assembler* assm) override;
};

#endif