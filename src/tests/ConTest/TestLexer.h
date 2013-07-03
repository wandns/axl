#pragma once

//.............................................................................

enum EToken
{
	EToken_Eof = 0,
	EToken_Error = -1,
	EToken_Identifier = 256,
	EToken_Integer,
	EToken_Double,
	EToken_Literal,
	EToken_Comment,
	EToken_Ne,
	EToken_Le,
	EToken_Ge,

	// keywords

	EToken_Message,

	EToken_STRING,
	EToken_WSTRING,
	EToken_BOOLEAN,
	EToken_BYTE,
	EToken_CHAR,
	EToken_WORD,
	EToken_SHORT,
	EToken_DWORD,
	EToken_LONG,
	EToken_REAL,
	EToken_ENUM,
	EToken_STRUCT,
	EToken_DOEVENTS,
	EToken_CONST,
	EToken_DIM,
	EToken_AS,
	EToken_END,
	EToken_DECLARE,
	EToken_FUNCTION,
	EToken_SUB,
	EToken_GOTO,
	EToken_GOSUB,
	EToken_RETURN,
	EToken_SELECT,
	EToken_CASE,
	EToken_IF,
	EToken_THEN,
	EToken_ELSE,
	EToken_FOR,
	EToken_TO,
	EToken_STEP,
	EToken_NEXT,
	EToken_EXIT,
	EToken_WHILE,
	EToken_WEND,
	EToken_DO,
	EToken_LOOP,
	EToken_UNTIL,
	EToken_BYREF,
	EToken_BYVAL,
	EToken_XOR,
	EToken_OR,
	EToken_AND,
	EToken_NOT,
	EToken_MOD,
	EToken_OBJECT,
	EToken_SYSCALL,
	EToken_EVENT,
	EToken_PROPERTY,
	EToken_GET,
	EToken_SET,
	EToken_REF,
	EToken_SIZEOF,
	EToken_INCLUDE,
	EToken_PUBLIC,
	EToken_STATIC,
	EToken_LIBRARY,
	
	EToken_PPLINE,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

AXL_PRS_BEGIN_TOKEN_NAME_MAP (CTokenName)
	AXL_PRS_TOKEN_NAME (EToken_Eof,        "eof")
	AXL_PRS_TOKEN_NAME (EToken_Error,      "error")
	AXL_PRS_TOKEN_NAME (EToken_Identifier, "identifier")
	AXL_PRS_TOKEN_NAME (EToken_Integer,    "integer-constant")
	AXL_PRS_TOKEN_NAME (EToken_Double,     "double-constant")
	AXL_PRS_TOKEN_NAME (EToken_Literal,    "string-literal")	
	AXL_PRS_TOKEN_NAME (EToken_Comment,    "comment")
	AXL_PRS_TOKEN_NAME (EToken_Ne,         "<>")
	AXL_PRS_TOKEN_NAME (EToken_Le,         "<=")
	AXL_PRS_TOKEN_NAME (EToken_Ge,         ">=")
	AXL_PRS_TOKEN_NAME (EToken_Message,    "message")

	AXL_PRS_TOKEN_NAME (EToken_STRING,     "string")
	AXL_PRS_TOKEN_NAME (EToken_WSTRING,    "wstring")
	AXL_PRS_TOKEN_NAME (EToken_BOOLEAN,    "boolean")
	AXL_PRS_TOKEN_NAME (EToken_BYTE,       "byte")
	AXL_PRS_TOKEN_NAME (EToken_CHAR,       "char")
	AXL_PRS_TOKEN_NAME (EToken_WORD,       "word")
	AXL_PRS_TOKEN_NAME (EToken_SHORT,      "short")
	AXL_PRS_TOKEN_NAME (EToken_DWORD,      "dword")
	AXL_PRS_TOKEN_NAME (EToken_LONG,       "long")
	AXL_PRS_TOKEN_NAME (EToken_REAL,       "real")
	AXL_PRS_TOKEN_NAME (EToken_ENUM,       "enum")
	AXL_PRS_TOKEN_NAME (EToken_STRUCT,     "struct")
	AXL_PRS_TOKEN_NAME (EToken_DOEVENTS,   "doevents")
	AXL_PRS_TOKEN_NAME (EToken_CONST,      "const")
	AXL_PRS_TOKEN_NAME (EToken_DIM,        "dim")
	AXL_PRS_TOKEN_NAME (EToken_AS,         "as")
	AXL_PRS_TOKEN_NAME (EToken_END,        "end")
	AXL_PRS_TOKEN_NAME (EToken_DECLARE,    "declare")
	AXL_PRS_TOKEN_NAME (EToken_FUNCTION,   "function")
	AXL_PRS_TOKEN_NAME (EToken_SUB,        "sub")
	AXL_PRS_TOKEN_NAME (EToken_GOTO,       "goto")
	AXL_PRS_TOKEN_NAME (EToken_GOSUB,      "gosub")
	AXL_PRS_TOKEN_NAME (EToken_RETURN,     "return")
	AXL_PRS_TOKEN_NAME (EToken_SELECT,     "select")
	AXL_PRS_TOKEN_NAME (EToken_CASE,       "case")
	AXL_PRS_TOKEN_NAME (EToken_IF,         "if")
	AXL_PRS_TOKEN_NAME (EToken_THEN,       "then")
	AXL_PRS_TOKEN_NAME (EToken_ELSE,       "else")
	AXL_PRS_TOKEN_NAME (EToken_FOR,        "for")
	AXL_PRS_TOKEN_NAME (EToken_TO,         "to")
	AXL_PRS_TOKEN_NAME (EToken_STEP,       "step")
	AXL_PRS_TOKEN_NAME (EToken_NEXT,       "next")
	AXL_PRS_TOKEN_NAME (EToken_EXIT,       "exit")
	AXL_PRS_TOKEN_NAME (EToken_WHILE,      "while")
	AXL_PRS_TOKEN_NAME (EToken_WEND,       "wend")
	AXL_PRS_TOKEN_NAME (EToken_DO,         "do")
	AXL_PRS_TOKEN_NAME (EToken_LOOP,       "loop")
	AXL_PRS_TOKEN_NAME (EToken_UNTIL,      "until")
	AXL_PRS_TOKEN_NAME (EToken_BYREF,      "byref")
	AXL_PRS_TOKEN_NAME (EToken_BYVAL,      "byval")
	AXL_PRS_TOKEN_NAME (EToken_XOR,        "xor")
	AXL_PRS_TOKEN_NAME (EToken_OR,         "or")
	AXL_PRS_TOKEN_NAME (EToken_AND,        "and")
	AXL_PRS_TOKEN_NAME (EToken_NOT,        "not")
	AXL_PRS_TOKEN_NAME (EToken_MOD,        "mod")
	AXL_PRS_TOKEN_NAME (EToken_OBJECT,     "object")
	AXL_PRS_TOKEN_NAME (EToken_SYSCALL,    "syscall")
	AXL_PRS_TOKEN_NAME (EToken_EVENT,      "event")
	AXL_PRS_TOKEN_NAME (EToken_PROPERTY,   "property")
	AXL_PRS_TOKEN_NAME (EToken_GET,        "get")
	AXL_PRS_TOKEN_NAME (EToken_SET,        "set")
	AXL_PRS_TOKEN_NAME (EToken_REF,        "ref")
	AXL_PRS_TOKEN_NAME (EToken_SIZEOF,     "sizeof")
	AXL_PRS_TOKEN_NAME (EToken_INCLUDE,    "incldue")
	AXL_PRS_TOKEN_NAME (EToken_PUBLIC,     "public")
	AXL_PRS_TOKEN_NAME (EToken_STATIC,     "static")
	AXL_PRS_TOKEN_NAME (EToken_LIBRARY,    "library")	
	AXL_PRS_TOKEN_NAME (EToken_PPLINE,     "line")

AXL_PRS_END_TOKEN_NAME_MAP ()

//.............................................................................

typedef axl::lex::CRagelTokenT <EToken, CTokenName> CToken;

class CLexer: public axl::lex::CRagelLexerT <CLexer, CToken>
{
	friend class axl::lex::CRagelLexerT <CLexer, CToken>;

protected:
	CToken*
	CreateIntegerToken (
		int Begin, 
		int Base
		)
	{
		CToken* pToken = CreateToken (EToken_Integer);
		pToken->m_Data.m_Integer = strtoul(ts + Begin, NULL, Base);
		return pToken;
	}

	CToken*
	CreateDoubleToken ()
	{
		CToken* pToken = CreateToken (EToken_Double);
		pToken->m_Data.m_Double = strtod (ts, NULL);
		return pToken;
	}

	CToken*
	CreateStringToken(
		int Begin,
		int End,
		EToken Token,
		size_t Channel = 0
		)
	{
		CToken* pToken = CreateToken (Token, Channel);
		pToken->m_Data.m_String.Copy (ts + Begin, pToken->m_Pos.m_Length - Begin - End);
		return pToken;
	}

	uchar_t
	GetHexValue (char c)
	{
		return 
			c >= '0' && c <= '9' ? c - '0' :
			c >= 'a' && c <= 'f' ? 10 + c - 'a' :
			c >= 'A' && c <= 'F' ? 10 + c - 'A' : 0;
	}

	CToken*
	CreateCharConstToken(
		int Begin,
		int End	
		)
	{
		CToken* pToken = CreateToken (EToken_Integer);
		pToken->m_Data.m_Int64 = 0;

		if (!pToken->m_Pos.m_Length)
			return pToken;

		size_t Length = pToken->m_Pos.m_Length - Begin - End;
		if (Length > 16) // int64
			Length = 16;
		
		char* p = ts + Begin;
		int Base = 1 << (Length - 1) * 4;

		for (; Base; p++, Base >>= 4)
			pToken->m_Data.m_Int64 += Base * GetHexValue (*p);

		return pToken;
	}

	CToken*
	CreateIntegerConstToken (int Value)
	{
		CToken* pToken = CreateToken (EToken_Integer);
		pToken->m_Data.m_Integer = Value;
		return pToken;
	}

	// implemented in TestLexer.rl

	void 
	Init ();

	bool
	Exec ();
};

//.............................................................................