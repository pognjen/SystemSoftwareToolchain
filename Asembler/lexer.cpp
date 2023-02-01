#include "lexer.h"
#include <vector>
#include <algorithm>
#include "error.h"
#include <fstream>

const std::unordered_set<std::string> Lexer::DIRECTIVES
{
	".global",
	".extern",
	".section",
	".word",
	".skip",
	".end"
};

const std::unordered_set<std::string> Lexer::INSTRUCTIONS
{
	"halt",
	"int",
	"iret",
	"call",
	"ret",
	"jmp",
	"jeq",
	"jne",
	"jgt",
	"push",
	"pop",
	"xchg",
	"add",
	"sub",
	"mul",
	"div",
	"cmp",
	"not",
	"and",
	"xor",
	"or",
	"test",
	"shl",
	"shr",
	"ldr",
	"str"
};

const std::unordered_set<std::string> Lexer::REGISTERS
{
	"r0",
	"r1",
	"r2",
	"r3",
	"r4",
	"r5",
	"r6",
	"r7",
	"R0",
	"R1",
	"R2",
	"R3",
	"R4",
	"R5",
	"R6",
	"R7",
	"sp",
	"SP",
	"pc",
	"PC",
	"psw",
	"PSW"
};

Lexer::Lexer(std::string filePath)
{
	mFilePath = filePath;
	mFileContentIndex = 0;
	mSourceFileLineNumber = 1;
}

std::vector<Token*> Lexer::GetTokenList()
{
	std::vector<Token*> resolvedTokens = {};
	GetFileContent();

	Token* nextToken = nullptr;

	while (nextToken == nullptr || nextToken->GetTokenType() != TokenType::EOFL)
	{
		nextToken = GetNextToken();

		if (nextToken->GetTokenType() == TokenType::UNKNOWN)
		{
			RaiseError("Line number " + std::to_string(mSourceFileLineNumber) + ": " + "Unknown token : " + nextToken->GetTokenString());
		}

		if (nextToken->GetTokenType() == TokenType::EOLN)
		{
			mSourceFileLineNumber++;
		}

		if (nextToken->GetTokenType() != TokenType::COMMENT)
		{
			resolvedTokens.push_back(nextToken);

			// Don't lex file after .end directive
			//
			if (nextToken->GetTokenType() == TokenType::DIRECTIVE && nextToken->GetTokenString() == ".end")
			{
				resolvedTokens.push_back(new Token(TokenType::EOFL, ""));
				break;
			}
		}
		else
		{
			delete nextToken;
			nextToken = nullptr;
		}
	}

	return resolvedTokens;
}

void Lexer::GetFileContent()
{
	std::ifstream sourceFile;
	sourceFile.open(mFilePath);

	if (sourceFile.is_open())
	{
		char c = sourceFile.get();
		while (sourceFile.good())
		{
			mFileContent += c;
			c = sourceFile.get();
		}
	}
	else
	{
		RaiseError("Error opening file: " + mFilePath);
	}
}

Token* Lexer::GetNextToken()
{
	Token* nextToken = new Token(TokenType::UNKNOWN, "");

	if (mFileContentIndex >= mFileContent.size())
	{
		nextToken->SetTokenInfo(TokenType::EOFL, "");
		return nextToken;
	}

	while (mFileContent[mFileContentIndex] == ' ' || mFileContent[mFileContentIndex] == '\t')
	{
		mFileContentIndex++;
	}

	switch (mFileContent[mFileContentIndex])
	{
	case EOF:
		nextToken->SetTokenInfo(TokenType::EOFL, "");
		mFileContentIndex++;
		break;
	case '\n':
		nextToken->SetTokenInfo(TokenType::EOLN, "\n");
		mFileContentIndex++;
		break;
	case '#':
		nextToken->SetTokenInfo(TokenType::COMMENT, "#");
		while (mFileContentIndex < mFileContent.size() && mFileContent[mFileContentIndex] != '\n')
		{
			mFileContentIndex++;
		}
		break;
	case '\t':
		nextToken->SetTokenInfo(TokenType::COMMENT, "\t");
		mFileContentIndex++;
		break;
	case '$':
		nextToken->SetTokenInfo(TokenType::DOLLAR, "$");
		mFileContentIndex++;
		break;
	case ',':
		nextToken->SetTokenInfo(TokenType::COMMA, ",");
		mFileContentIndex++;
		break;
	case '"':		
		nextToken->SetTokenInfo(TokenType::QUOTATION, "\"");
		mFileContentIndex++;
		break;
	case '%':
		nextToken->SetTokenInfo(TokenType::PERCENT, "%");
		mFileContentIndex++;
		break;
	case '[':
		nextToken->SetTokenInfo(TokenType::SQUARE_BRACKET_OPEN, "[");
		mFileContentIndex++;
		break;
	case ']':
		nextToken->SetTokenInfo(TokenType::SQUARE_BRACKET_CLOSED, "]");
		mFileContentIndex++;
		break;
	case '*':
		nextToken->SetTokenInfo(TokenType::ASTERISK, "*");
		mFileContentIndex++;
		break;
	case '+':
		nextToken->SetTokenInfo(TokenType::PLUS, "+");
		mFileContentIndex++;
		break;
	case '-':
		nextToken->SetTokenInfo(TokenType::MINUS, "-");
		mFileContentIndex++;
		break;
	case ':':
		nextToken->SetTokenInfo(TokenType::COLON, ":");
		mFileContentIndex++;
		break;
	default:
		GetNextMulticharToken(nextToken);
		break;
	}

	return nextToken;
}

void Lexer::GetNextMulticharToken(Token* token)
{
	// If token doesn't start with alphanumeric, _(underscore) or .(dot), it has UNKNOWN type. 
	//
	if (!isalnum(mFileContent[mFileContentIndex]) && mFileContent[mFileContentIndex] != '_'
		&& mFileContent[mFileContentIndex] != '.')
	{
		return;
	}

	// Collect string of token and deduce type based on that string
	//
	while (mFileContentIndex < mFileContent.size() && (isalnum(mFileContent[mFileContentIndex])
		|| mFileContent[mFileContentIndex] == '_' || mFileContent[mFileContentIndex] == '.'))
	{
		token->AppenCharToTokenString(mFileContent[mFileContentIndex]);
		mFileContentIndex++;
	}

	DeduceTokenType(token);
}

void Lexer::DeduceTokenType(Token* token)
{
	int currentIndex = 0;
	bool isHexNumericLiteral = false;
	bool isDecNumericLiteral = true;

	std::string tokenString = token->GetTokenString();

	if (tokenString.empty())
	{
		return;
	}

	// Check for "0x" prefix
	//
	if (tokenString.size() >= 2 && tokenString[0] == '0' && tokenString[1] == 'x')
	{
		isHexNumericLiteral = true;
		currentIndex += 2;
	}

	// Check for numeric or alphanumeric(Hex digits a,b,c,d,e,f) suffix
	while (currentIndex != tokenString.size())
	{
		if (!isdigit(tokenString[currentIndex]) && !(isHexNumericLiteral && isxdigit(tokenString[currentIndex])))
		{
			isHexNumericLiteral = false;
			isDecNumericLiteral = false;
			break;
		}
		currentIndex++;
	}

	if (isHexNumericLiteral)
	{
		token->SetTokenType(TokenType::NUMERIC_LITERAL_HEX);
	}
	else if (isDecNumericLiteral)
	{
		token->SetTokenType(TokenType::NUMERIC_LITERAL_DEC);
	}
	else if (REGISTERS.find(token->GetTokenString()) != REGISTERS.end())
	{
		token->SetTokenType(TokenType::REGISTER);
	}
	else if (INSTRUCTIONS.find(token->GetTokenString()) != INSTRUCTIONS.end())
	{
		token->SetTokenType(TokenType::INSTRUCTION);
	}
	else if (DIRECTIVES.find(token->GetTokenString()) != DIRECTIVES.end())
	{
		token->SetTokenType(TokenType::DIRECTIVE);
	}
	else if (CheckIdentifierFormat(tokenString))
	{
		token->SetTokenType(TokenType::IDENTIFIER);
	}
}

// Identifier must start with letter and can contain any alphanumeric symbol or _(underscore)
//
bool Lexer::CheckIdentifierFormat(std::string tokenString)
{
	if (tokenString.empty() || !isalpha(tokenString[0]))
	{
		return false;
	}
	
	for (int i = 1; i < tokenString.size(); i++)
	{
		if (!isalnum(tokenString[i]) && tokenString[i] != '_')
		{
			return false;
		}
	}

	return true;
}

Token::Token(TokenType tokenType, std::string tokenString)
{
	mTokenType = tokenType;
	mTokenString = tokenString;
}

TokenType Token::GetTokenType() const
{
	return mTokenType;
}

std::string Token::GetTokenString() const
{
	return mTokenString;
}

void Token::SetTokenType(TokenType tokenType)
{
	mTokenType = tokenType;
}

void Token::SetTokenString(std::string tokenString)
{
	mTokenString = tokenString;
}

void Token::SetTokenInfo(TokenType tokenType, std::string tokenString)
{
	mTokenType = tokenType;
	mTokenString = tokenString;
}

void Token::AppenCharToTokenString(char c)
{
	mTokenString += c;
}