#include "stdafx.h"
#include "axl_pg_Parser.h"

namespace axl {
namespace pg {

//.............................................................................

bool
CParser::ParseFile (
	CModule* pModule,
	CConfig* pConfig,
	const rtl::CString& FilePath
	)
{
	bool Result;

	io::CMappedFile SrcFile;

	Result = SrcFile.Open (FilePath, io::CFile::EFile_ReadOnly);
	if (!Result)
	{
		err::SetFormatStringError (_T("cannot open '%s': %s"), FilePath, err::GetError ()->GetDescription ());
		return false;
	}

	size_t Size = (size_t) SrcFile.GetSize ();
	char* p = (char*) SrcFile.View (0, Size);
	if (!p)
	{
		err::SetFormatStringError (_T("cannot open '%s': %s"), FilePath, err::GetError ()->GetDescription ());
		return false;
	}

	return Parse (pModule, pConfig, FilePath, p, Size);
}

bool
CParser::Parse (
	CModule* pModule,
	const CConfig* pConfig,
	const rtl::CString& FilePath,
	const char* pSource,
	size_t Length
	)
{
	bool Result;

	m_pModule = pModule;
	m_pConfig = pConfig;
	m_DirName = io::GetDirName (FilePath);

	m_DefaultProductionSpecifiers.Reset ();

	CLexer::Create (FilePath, pSource, Length);

	Result = Program ();
	if (!Result)
	{
		EnsureSrcPosError ();
		return false;
	}

	return true;
}

bool
CParser::Program ()
{
	bool Result;

	for (;;)
	{
		const CToken* pToken = GetToken ();
		const CToken* pNextToken;

		if (!pToken->m_Token)
			break;

		switch (pToken->m_Token)
		{
		case EToken_LL:
			Result = LLStatement ();
			if (!Result)
				return false;

			break;

		case EToken_Import:
			Result = ImportStatement ();
			if (!Result)
				return false;

			break;

		case EToken_Using:
			Result = UsingStatement ();
			if (!Result)
				return false;

			break;

		case ';':
			NextToken ();
			break;

		case EToken_Identifier:
			pNextToken = GetToken (1);
			if (pNextToken->m_Token == '{' || pNextToken->m_Token == '=')
			{
				Result = DefineStatement ();
				if (!Result)
					return false;

				break;
			}

			// fall through

		default:
			Result = DeclarationStatement ();
			if (!Result)
				return false;
		}
	}

	return true;
}

bool
CParser::LLStatement ()
{
	const CToken* pToken = GetToken ();
	ASSERT (pToken->m_Token == EToken_LL);

	NextToken ();

	pToken = ExpectToken ('(');
	if (!pToken)
		return false;

	NextToken ();

	pToken = ExpectToken (EToken_Integer);
	if (!pToken)
		return false;

	if (m_pModule->m_LookaheadLimit && m_pModule->m_LookaheadLimit != pToken->m_Data.m_Integer)
	{
		err::SetFormatStringError (_T("redefinition of lookahead limit (previously seen as %d)"), m_pModule->m_LookaheadLimit);
		return false;
	}

	m_pModule->m_LookaheadLimit = pToken->m_Data.m_Integer;

	NextToken ();

	pToken = ExpectToken (')');
	if (!pToken)
		return false;

	NextToken ();

	pToken = ExpectToken (';');
	if (!pToken)
		return false;

	NextToken ();
	return true;
}

bool
CParser::ImportStatement ()
{
	const CToken* pToken = GetToken ();
	ASSERT (pToken->m_Token == EToken_Import);

	NextToken ();

	pToken = ExpectToken (EToken_Literal);
	if (!pToken)
		return false;

	rtl::CString FilePath = io::FindFilePath (
		pToken->m_Data.m_String,
		m_DirName,
		m_pConfig ? &m_pConfig->m_ImportDirList : NULL,
		true
		);

	if (FilePath.IsEmpty ())
	{
		err::SetFormatStringError (_T("cannot find import file '%s'"), pToken->m_Data.m_String);
		return false;
	}

	m_pModule->m_ImportList.InsertTail (FilePath);

	NextToken ();

	pToken = ExpectToken (';');
	if (!pToken)
		return false;

	NextToken ();
	return true;
}

bool
CParser::DeclarationStatement ()
{
	bool Result;

	CProductionSpecifiers Specifiers = m_DefaultProductionSpecifiers;
	Result = ProductionSpecifiers (&Specifiers);
	if (!Result)
		return false;

	const CToken* pToken = GetToken ();
	if (pToken->m_Token == EToken_Identifier)
	{
		Result = Production (&Specifiers);
		if (!Result)
			return false;
	}

	pToken = ExpectToken (';');
	if (!pToken)
		return false;

	NextToken ();
	return true;
}

bool
CParser::ProductionSpecifiers (CProductionSpecifiers* pSpecifiers)
{
	CClass* pClass = NULL;
	int SymbolFlags = 0;

	bool IsClassSpecified = false;
	bool NoMoreSpecifiers = false;

	do
	{
		const CToken* pToken = GetToken ();
		const CToken* pNextToken;

		switch (pToken->m_Token)
		{
		case EToken_Class:
			if (IsClassSpecified)
			{
				err::SetStringError (_T("multiple class specifiers"));
				return false;
			}

			pClass = ClassSpecifier ();
			if (!pClass)
				return false;

			IsClassSpecified = true;
			break;

		case EToken_Default:
			if (IsClassSpecified)
			{
				err::SetStringError (_T("multiple class specifiers"));
				return false;
			}

			NextToken ();

			pClass = NULL;
			IsClassSpecified = true;
			break;

		case EToken_NoAst:
			if (IsClassSpecified) 
			{
				err::SetStringError (_T("multiple class specifiers"));
				return false;
			}

			NextToken ();

			SymbolFlags |= ESymbolNodeFlag_IsNoAst;
			IsClassSpecified = true;
			break;

		case EToken_Pragma:
			if (SymbolFlags & ESymbolNodeFlag_IsPragma)
			{
				err::SetStringError (_T("multiple 'pragma' specifiers"));
				return false;
			}

			NextToken ();

			SymbolFlags |= ESymbolNodeFlag_IsPragma;
			break;

		case EToken_Start:
			if (SymbolFlags & ESymbolNodeFlag_IsStart)
			{
				err::SetStringError (_T("multiple 'start' specifiers"));
				return false;
			}

			NextToken ();

			SymbolFlags |= ESymbolNodeFlag_IsStart;
			break;

		case EToken_Identifier:
			pNextToken = GetToken (1);
			switch (pNextToken->m_Token)
			{
			case EToken_Class:
			case EToken_Default:
			case EToken_NoAst:
			case EToken_Pragma: 
			case EToken_Identifier:  
				if (IsClassSpecified)
				{
					err::SetStringError (_T("multiple class specifiers"));
					return false;
				}

				pClass = m_pModule->m_ClassMgr.GetClass (pToken->m_Data.m_String);
				NextToken ();
				IsClassSpecified = true;
				break;

			default:
				NoMoreSpecifiers = true;
			}
			
			break;

		default:
			NoMoreSpecifiers = true;
		}
	} while (!NoMoreSpecifiers);

	pSpecifiers->m_SymbolFlags = SymbolFlags;

	if (IsClassSpecified)
		pSpecifiers->m_pClass = pClass;

	return true;
}

CClass*
CParser::ClassSpecifier ()
{
	bool Result;

	const CToken* pToken = GetToken ();
	ASSERT (pToken->m_Token == EToken_Class);

	NextToken ();

	CClass* pClass;

	pToken = GetToken ();

	if (pToken->m_Token != EToken_Identifier)
	{
		pClass = m_pModule->m_ClassMgr.CreateUnnamedClass ();
	}
	else
	{
		pClass = m_pModule->m_ClassMgr.GetClass (pToken->m_Data.m_String);
		NextToken ();

		pToken = GetToken ();
		if (pToken->m_Token != '{' && pToken->m_Token != ':')
			return pClass;

		if (pClass->m_Flags & EClassFlag_IsDefined)
		{
			err::SetFormatStringError (_T("redefinition of class '%s'"), pClass->m_Name);
			return NULL;
		}
	}

	if (pToken->m_Token == ':')
	{
		NextToken ();

		pToken = ExpectToken (EToken_Identifier);
		if (!pToken)
			return NULL;

		pClass->m_pBaseClass = m_pModule->m_ClassMgr.GetClass (pToken->m_Data.m_String);
		pClass->m_pBaseClass->m_Flags |= EClassFlag_IsUsed;

		NextToken ();
	}

	Result = UserCode ('{', &pClass->m_Members);
	if (!Result)
		return NULL;

	pClass->m_Flags |= EClassFlag_IsDefined;
	return pClass;
}

bool
CParser::DefineStatement ()
{
	const CToken* pToken = ExpectToken (EToken_Identifier);
	if (!pToken)
		return false;

	CDefine* pDefine = m_pModule->m_DefineMgr.GetDefine (pToken->m_Data.m_String);

	NextToken ();

	pToken = GetToken ();
	switch (pToken->m_Token)
	{
	case '{':
		return UserCode ('{', &pDefine->m_StringValue);

	case '=':
		NextToken ();

		pToken = GetToken ();
		switch (pToken->m_Token)
		{
		case EToken_Identifier:
		case EToken_Literal:
			pDefine->m_StringValue = pToken->m_Data.m_String;
			NextToken ();
			break;

		case EToken_Integer:
			pDefine->m_Kind = EDefine_Integer;
			pDefine->m_IntegerValue = pToken->m_Data.m_Integer;
			NextToken ();
			break;

		case '{':
			return UserCode ('{', &pDefine->m_StringValue);

		default:
			err::SetFormatStringError (_T("invalid define value for '%s'"), pDefine->m_Name);
			return false;
		}

		break;

	default:
		err::SetFormatStringError (_T("invalid define syntax for '%s'"), pDefine->m_Name);
		return false;
	}

	pToken = ExpectToken (';');
	if (!pToken)
		return false;

	NextToken ();
	return true;
}

bool
CParser::UsingStatement ()
{
	bool Result;

	const CToken* pToken = GetToken ();
	ASSERT (pToken->m_Token == EToken_Using);

	NextToken ();

	Result = ProductionSpecifiers (&m_DefaultProductionSpecifiers);
	if (!Result)
		return false;

	return true;
}

bool 
CParser::CustomizeSymbol (CSymbolNode* pNode)
{
	bool Result;

	const CToken* pToken = GetToken ();
	if (pToken->m_Token == '<')
	{
		Result = UserCode ('<', &pNode->m_Arg);
		if (!Result)
			return false;
	}

	for (;;)
	{
		pToken = GetToken ();

		rtl::CString* pString = NULL;

		switch (pToken->m_Token)
		{
		case EToken_Local:
			pString = &pNode->m_Local;
			break;

		case EToken_Enter:
			pString = &pNode->m_Enter;
			break;

		case EToken_Leave:
			pString = &pNode->m_Leave;
			break;
		}

		if (!pString)
			break;

		if (!pString->IsEmpty ())
		{
			err::SetFormatStringError (_T("redefinition of '%s'::%s"), pNode->m_Name, pToken->GetName ());
			return false;
		}

		NextToken ();
		Result = UserCode ('{', pString);
		if (!Result)
			return false;
	}

	if (!pNode->m_Arg.IsEmpty ())
	{
		Result = ProcessFormalArgList (pNode);
		if (!Result)
			return false;
	}

	if (!pNode->m_Local.IsEmpty ())
	{
		Result = ProcessLocalList (pNode);
		if (!Result)
			return false;
	}

	if (!pNode->m_Enter.IsEmpty ())
	{
		Result = ProcessSymbolEventHandler (pNode, &pNode->m_Enter);
		if (!Result)
			return false;
	}

	if (!pNode->m_Leave.IsEmpty ())
	{
		Result = ProcessSymbolEventHandler (pNode, &pNode->m_Leave);
		if (!Result)
			return false;
	}

	return true;
}

bool 
CParser::ProcessFormalArgList (CSymbolNode* pNode)
{
	const CToken* pToken;

	rtl::CString ResultString;

	CLexer Lexer;
	Lexer.Create (GetMachineState (ELexerMachine_UserCode2ndPass), _T("formal-arg-list"), pNode->m_Arg);

	const tchar_t* p = pNode->m_Arg;

	for (;;)
	{
		pToken = Lexer.ExpectToken (EToken_Identifier);
		if (!pToken)
			return false;

		rtl::CString Name = pToken->m_Data.m_String;

		ResultString.Append (p, pToken->m_Pos.m_p - p);
		ResultString.Append (Name);
		p = pToken->m_Pos.m_p + pToken->m_Pos.m_Length;

		Lexer.NextToken ();

		pNode->m_ArgNameList.InsertTail (Name);
		pNode->m_ArgNameSet.Goto (Name);

		pToken = Lexer.GetToken ();
		if (!pToken->m_Token)
			break;

		pToken = Lexer.ExpectToken (',');
		if (!pToken)
			return false;

		Lexer.NextToken ();
	}

	ASSERT (!pToken->m_Token);
	ResultString.Append (p, pToken->m_Pos.m_p - p);

	pNode->m_Arg = ResultString;
	return true;
}

bool 
CParser::ProcessLocalList (CSymbolNode* pNode)
{
	const CToken* pToken;

	rtl::CString ResultString;

	CLexer Lexer;
	Lexer.Create (GetMachineState (ELexerMachine_UserCode2ndPass), _T("local-list"), pNode->m_Local);

	const tchar_t* p = pNode->m_Local;

	for (;;)
	{
		pToken = Lexer.GetToken ();
		if (pToken->m_Token <= 0)
			break;
		
		if (pToken->m_Token != EToken_Identifier)
		{
			Lexer.NextToken ();
			continue;
		}

		rtl::CString Name = pToken->m_Data.m_String;

		ResultString.Append (p, pToken->m_Pos.m_p - p);
		ResultString.Append (Name);
		p = pToken->m_Pos.m_p + pToken->m_Pos.m_Length;

		Lexer.NextToken ();

		pNode->m_LocalNameList.InsertTail (Name);
		pNode->m_LocalNameSet.Goto (Name);
	}

	ASSERT (!pToken->m_Token);
	ResultString.Append (p, pToken->m_Pos.m_p - p);

	pNode->m_Local = ResultString;
	return true;
}

bool 
CParser::ProcessSymbolEventHandler (
	CSymbolNode* pNode,
	rtl::CString* pString
	)
{
	const CToken* pToken;

	rtl::CString ResultString;

	CLexer Lexer;
	Lexer.Create (GetMachineState (ELexerMachine_UserCode2ndPass), _T("event-handler"), *pString);

	const tchar_t* p = *pString;

	for (;;)
	{
		pToken = Lexer.GetToken ();
		if (pToken->m_Token <= 0)
			break;
		
		rtl::CHashTableIteratorT <const tchar_t*> It;

		switch (pToken->m_Token)
		{
		case EToken_Identifier:
			It = pNode->m_LocalNameSet.Find (pToken->m_Data.m_String);
			if (It)
			{
				ResultString.Append (p, pToken->m_Pos.m_p - p);
				ResultString.AppendFormat (_T("$local.%s"), pToken->m_Data.m_String);
				break;
			}

			It = pNode->m_ArgNameSet.Find (pToken->m_Data.m_String);
			if (It)
			{
				ResultString.Append (p, pToken->m_Pos.m_p - p);
				ResultString.AppendFormat (_T("$arg.%s"), pToken->m_Data.m_String);
				break;
			}

			err::SetFormatStringError (_T("undeclared identifier '%s'"), pToken->m_Token);
			return false;

		case EToken_Integer:
			if (pToken->m_Data.m_Integer != 0)
			{
				err::SetFormatStringError (_T("'enter' or 'leave' cannot have indexed references"));
				return false;
			}

			ResultString.Append (p, pToken->m_Pos.m_p - p);
			ResultString.Append (_T('$'));
			break;

		default:
			Lexer.NextToken ();
			continue;
		}

		p = pToken->m_Pos.m_p + pToken->m_Pos.m_Length;
		Lexer.NextToken ();
	}

	ASSERT (!pToken->m_Token);
	ResultString.Append (p, pToken->m_Pos.m_p - p);

	*pString = ResultString;
	return true;
}

bool 
CParser::ProcessActualArgList (
	CArgumentNode* pNode,
	const rtl::CString& String
	)
{
	const CToken* pToken;

	CLexer Lexer;
	Lexer.Create (GetMachineState (ELexerMachine_UserCode2ndPass), _T("actual-arg-list"), String);

	int Level = 0;

	const tchar_t* p = String;

	for (;;)
	{
		pToken = Lexer.GetToken ();
		if (pToken->m_Token <= 0)
			break;

		switch (pToken->m_Token)
		{
		case '(': 
		case '{':
		case '[':
			Level++;
			break;

		case ')': 
		case '}':
		case ']':
			Level--;
			break;

		case ',':
			if (Level == 0)
			{
				rtl::CString ValueString (p, pToken->m_Pos.m_p - p);
				pNode->m_ArgValueList.InsertTail (ValueString);

				p = pToken->m_Pos.m_p + pToken->m_Pos.m_Length;
			}
		};

		Lexer.NextToken ();
	}

	rtl::CString ValueString (p, pToken->m_Pos.m_p - p);
	pNode->m_ArgValueList.InsertTail (ValueString);

	ASSERT (!pToken->m_Token);
	return true;
}

void
CParser::SetGrammarNodeSrcPos (
	CGrammarNode* pNode,
	const lex::CLineCol& LineCol
	)
{
	pNode->m_SrcPos.m_FilePath = m_FilePath;
	pNode->m_SrcPos.m_Line = LineCol.m_Line;
	pNode->m_SrcPos.m_Col = LineCol.m_Col;
}

bool 
CParser::Production (const CProductionSpecifiers* pSpecifiers)
{
	bool Result;

	const CToken* pToken = GetToken ();
	ASSERT (pToken->m_Token == EToken_Identifier);

	CSymbolNode* pSymbol = m_pModule->m_NodeMgr.GetSymbolNode (pToken->m_Data.m_String);	
	if (!pSymbol->m_ProductionArray.IsEmpty ())
	{
		err::SetFormatStringError (_T("redefinition of symbol '%s'"), pSymbol->m_Name);
		return false;
	}

	SetGrammarNodeSrcPos (pSymbol);

	NextToken ();

	pSymbol->m_pClass = pSpecifiers->m_pClass;
	pSymbol->m_Flags |= pSpecifiers->m_SymbolFlags;

	if (pSymbol->m_pClass)
		pSymbol->m_pClass->m_Flags |= EClassFlag_IsUsed;

	if (pSymbol->m_Flags & ESymbolNodeFlag_IsPragma)
		m_pModule->m_NodeMgr.m_StartPragmaSymbol.m_ProductionArray.Append (pSymbol);

	if ((pSymbol->m_Flags & ESymbolNodeFlag_IsStart) && !m_pModule->m_NodeMgr.m_pPrimaryStartSymbol)
		m_pModule->m_NodeMgr.m_pPrimaryStartSymbol = pSymbol;

	Result = CustomizeSymbol (pSymbol);
	if (!Result)
		return false;

	pToken = ExpectToken (':');
	if (!pToken)
		return false;

	NextToken ();

	CGrammarNode* pRightSide = Alternative ();
	if (!pRightSide)
		return false;

	pSymbol->AddProduction (pRightSide);
	return true;
}

CGrammarNode* 
CParser::Alternative ()
{
	CGrammarNode* pNode = Sequence ();
	if (!pNode)
		return NULL;

	CSymbolNode* pTemp = NULL;

	for (;;)
	{
		const CToken* pToken = GetToken ();
		if (pToken->m_Token != '|')
			break;

		NextToken ();

		CGrammarNode* pNode2 = Sequence ();
		if (!pNode2)
			return false;

		if (!pTemp)
			if (pNode->m_Kind == ENode_Symbol && (pNode->m_Flags & ESymbolNodeFlag_IsNamed))
			{
				pTemp = (CSymbolNode*) pNode;
			}
			else
			{
				pTemp = m_pModule->m_NodeMgr.CreateTempSymbolNode ();
				pTemp->AddProduction (pNode);

				SetGrammarNodeSrcPos (pTemp, pNode->m_SrcPos);
				pNode = pTemp;
			}

		pTemp->AddProduction (pNode2);
	}

	return pNode;
}

static
inline
bool
IsFirstOfPrimary (int Token)
{
	switch (Token)
	{
	case EToken_Identifier:
	case EToken_Integer:
	case EToken_Any:
	case EToken_Resolver:
	case '{':
	case '(':
	case '.':
		return true;

	default:
		return false;
	}
}

CGrammarNode* 
CParser::Sequence ()
{
	CGrammarNode* pNode = Quantifier ();
	if (!pNode)
		return NULL;

	CSequenceNode* pTemp = NULL;

	for (;;)
	{
		const CToken* pToken = GetToken ();
		
		if (!IsFirstOfPrimary (pToken->m_Token))
			break;

		CGrammarNode* pNode2 = Quantifier ();
		if (!pNode2)
			return false;

		if (!pTemp)
			if (pNode->m_Kind == ENode_Sequence)
			{
				pTemp = (CSequenceNode*) pNode;
			}
			else
			{
				pTemp = m_pModule->m_NodeMgr.CreateSequenceNode ();
				pTemp->Append (pNode);

				SetGrammarNodeSrcPos (pTemp, pNode->m_SrcPos);
				pNode = pTemp;
			}

		pTemp->Append (pNode2);
	}

	return pNode;
}

CGrammarNode*
CParser::Quantifier ()
{
	CGrammarNode* pNode = Primary ();
	if (!pNode)
		return NULL;

	const CToken* pToken = GetToken ();
	if (pToken->m_Token != '?' &&
		pToken->m_Token != '*' &&
		pToken->m_Token != '+')
		return pNode;

	CGrammarNode* pTemp = m_pModule->m_NodeMgr.CreateQuantifierNode (pNode, pToken->m_Token);
	if (!pTemp)
		return NULL;

	SetGrammarNodeSrcPos (pTemp, pNode->m_SrcPos);

	NextToken ();
	return pTemp;
}

CGrammarNode*
CParser::Primary ()
{
	bool Result;

	CGrammarNode* pNode;
	CActionNode* pActionNode;

	const CToken* pToken = GetToken ();
	switch (pToken->m_Token)
	{
	case '.':
	case EToken_Any:
	case EToken_Integer:
		pNode = Beacon ();		
		if (!pNode)
			return NULL;

		break;

	case EToken_Epsilon:
		pNode = &m_pModule->m_NodeMgr.m_EpsilonNode;
		NextToken ();
		break;

	case ';':
	case '|':
		pNode = &m_pModule->m_NodeMgr.m_EpsilonNode; 
		// and don't swallow token
		break;

	case EToken_Identifier:
		pNode = Beacon ();
		if (!pNode)
			return NULL;

		pToken = GetToken ();
		if (pToken->m_Token == '<')
		{
			CBeaconNode* pBeacon = (CBeaconNode*) pNode;
			CArgumentNode* pArgument = m_pModule->m_NodeMgr.CreateArgumentNode ();
			CSequenceNode* pSequence = m_pModule->m_NodeMgr.CreateSequenceNode ();
			
			SetGrammarNodeSrcPos (pArgument, pToken->m_Pos);
			SetGrammarNodeSrcPos (pSequence, pNode->m_SrcPos);

			rtl::CString String;
			Result = UserCode ('<', &String);
			if (!Result)
				return NULL;

			Result = ProcessActualArgList (pArgument, String);
			if (!Result)
				return NULL;

			pBeacon->m_pArgument = pArgument;
			pArgument->m_pTargetSymbol = pBeacon->m_pTarget;

			pSequence->Append (pBeacon);
			pSequence->Append (pArgument);
			pNode = pSequence;
		}

		break;

	case '{':
		pActionNode = m_pModule->m_NodeMgr.CreateActionNode ();
		SetGrammarNodeSrcPos (pActionNode);

		Result = UserCode ('{', &pActionNode->m_UserCode);
		if (!Result)
			return NULL;

		pNode = pActionNode;
		break;

	case EToken_Resolver:
		pNode = Resolver ();
		if (!pNode)
			return NULL;

		break;

	case '(':
		NextToken ();

		pNode = Alternative ();
		if (!pNode)
			return NULL;

		pToken = ExpectToken (')');
		if (!pToken)
			return NULL;

		NextToken ();
		break;

	default:
		err::SetUnexpectedTokenError (pToken->GetName (), "primary");
		return NULL;
	}

	return pNode; 
}

CBeaconNode*
CParser::Beacon ()
{
	CSymbolNode* pNode;

	const CToken* pToken = GetToken ();
	switch (pToken->m_Token)
	{
	case EToken_Any:
	case '.':
		pNode = &m_pModule->m_NodeMgr.m_AnyTokenNode;
		break;

	case EToken_Identifier:
		pNode = m_pModule->m_NodeMgr.GetSymbolNode (pToken->m_Data.m_String);
		break;

	case EToken_Integer:
		if (!pToken->m_Data.m_Integer)
		{
			err::SetFormatStringError (_T("cannot use a reserved eof token \\00"));
			return NULL;
		}
		
		pNode = m_pModule->m_NodeMgr.GetTokenNode (pToken->m_Data.m_Integer);
		break;

	default:
		ASSERT (false);
	}

	CBeaconNode* pBeacon = m_pModule->m_NodeMgr.CreateBeaconNode (pNode);
	SetGrammarNodeSrcPos (pBeacon);

	NextToken ();

	pToken = GetToken ();
	if (pToken->m_Token != '$')	
		return pBeacon;

	NextToken ();
	pToken = ExpectToken (EToken_Identifier);
	if (!pToken)
		return NULL;

	pBeacon->m_Label = pToken->m_Data.m_String;
	NextToken ();

	return pBeacon;
}

CSymbolNode*
CParser::Resolver ()
{
	const CToken* pToken = GetToken ();
	ASSERT (pToken->m_Token == EToken_Resolver);

	lex::CRagelTokenPos Pos = pToken->m_Pos;

	NextToken ();

	pToken = ExpectToken ('(');
	if (!pToken)
		return NULL;

	NextToken ();

	CGrammarNode* pResolver = Alternative ();
	if (!pResolver)
		return NULL;

	pToken = ExpectToken (')');
	if (!pToken)
		return NULL;

	NextToken ();

	CGrammarNode* pProduction = Sequence ();
	if (!pProduction)
		return NULL;
	
	CSymbolNode* pTemp = m_pModule->m_NodeMgr.CreateTempSymbolNode ();
	pTemp->m_pResolver = pResolver;
	pTemp->AddProduction (pProduction);

	SetGrammarNodeSrcPos (pTemp, Pos);

	return pTemp;
}

bool
CParser::UserCode (
	int OpenBracket,
	rtl::CString* pString
	)
{
	const CToken* pToken = ExpectToken (OpenBracket);
	if (!pToken)
		return false;

	GotoState (GetMachineState (ELexerMachine_UserCode), pToken, EGotoState_ReparseToken);

	pToken = GetToken ();

	OpenBracket = pToken->m_Token; // could change! e.g. { vs {. and < vs <.

	int CloseBracket;

	switch (OpenBracket)
	{
	case '{':
		CloseBracket = '}';
		break;
		
	case '(':
		CloseBracket = ')';
		break;

	case '<':
		CloseBracket = '>';
		break;

	case EToken_OpenBrace:
		CloseBracket = EToken_CloseBrace;
		break;

	case EToken_OpenChevron:
		CloseBracket = EToken_CloseChevron;
		break;

	default:
		ASSERT (false);

		err::SetFormatStringError (_T("invalid user code opener '%s'"), CToken::GetName (OpenBracket));
		return false;
	}

	const char* pBegin = pToken->m_Pos.m_p + pToken->m_Pos.m_Length;

	NextToken ();
	
	int Level = 1;

	for (;;)
	{
		pToken = GetToken ();

		if (pToken->m_Token == EToken_Eof)
		{
			err::SetUnexpectedTokenError (_T("eof"), _T("user-code"));
			return false;
		} 
		else if (pToken->m_Token == EToken_Error)
		{
			err::SetError (pToken->m_Data.m_Error);
			return false;
		}
		else if (pToken->m_Token == OpenBracket)
		{
			Level++;
		}
		else if (pToken->m_Token == CloseBracket)
		{
			Level--;
		
			if (Level <= 0)
				break;
		}
		
		NextToken ();
	}

	size_t End = pToken->m_Pos.m_Offset;

	pToken = GetToken ();
	ASSERT (pToken->m_Token == CloseBracket);

	*pString = rtl::CStringA (pBegin, pToken->m_Pos.m_p - pBegin);

	GotoState (GetMachineState (ELexerMachine_Main), pToken, EGotoState_EatToken);

	return true;
}

//.............................................................................

} // namespace pg {
} // namespace axl {
