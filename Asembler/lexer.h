#ifndef _LEXER_H_
#define _LEXER_H_

#include <string>
#include <unordered_set>
#include <vector>

enum TokenType
{
	EOFL,
	EOLN,
	COMMENT,
	TAB,
	NUMERIC_LITERAL_HEX,
	NUMERIC_LITERAL_DEC,
	DOLLAR,
	IDENTIFIER,
	COMMA,
	COLON,
	QUOTATION,
	INSTRUCTION,
	DIRECTIVE,
	PERCENT,
	SQUARE_BRACKET_OPEN,
	SQUARE_BRACKET_CLOSED,
	ASTERISK,
	PLUS,
	MINUS,
	REGISTER,
	UNKNOWN
};

class Token;

class Lexer
{
public:
	Lexer(std::string filePath);
	std::vector<Token*> GetTokenList();

	static const std::unordered_set<std::string> DIRECTIVES;
	static const std::unordered_set<std::string> INSTRUCTIONS;
	static const std::unordered_set<std::string> REGISTERS;

private:
	void GetFileContent();
	Token* GetNextToken();
	void GetNextMulticharToken(Token* token);
	void DeduceTokenType(Token* token);
	bool CheckIdentifierFormat(std::string tokenString);

	uint32_t mFileContentIndex;
	uint32_t mSourceFileLineNumber;
	std::string mFilePath;
	std::string mFileContent = "";
};

class Token
{
public:
	Token(TokenType tokenType, std::string tokenString);
	
	TokenType GetTokenType() const;
	std::string GetTokenString() const;

	void AppenCharToTokenString(char c);
	void SetTokenType(TokenType tokenType);
	void SetTokenString(std::string tokenString);
	void SetTokenInfo(TokenType tokenType, std::string tokenString);
private:
	TokenType mTokenType;
	std::string mTokenString;
};


#endif