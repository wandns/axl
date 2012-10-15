// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2010. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_LLK_NODE_H

#include "axl_rtl_Array.h"
#include "axl_llk_Ast.h"

namespace axl {
namespace llk {
	
//.............................................................................

enum ENode
{
	ENode_Undefined = 0,

	ENode_Token,
	ENode_Symbol,
	ENode_Sequence,
	ENode_Action,	
	ENode_Argument,	
	ENode_LaDfa,	
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

enum ENodeFlag
{
	ENodeFlag_IsLocator = 0x01, // used to locate AST / token from actions (applies to token & symbol nodes)
	ENodeFlag_IsMatched = 0x02, // applies to token & symbol & argument nodes
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CNode: public rtl::TListLink
{
public:
	ENode m_Kind;
	int m_Flags;
	size_t m_Index;

public:
	CNode ()
	{
		m_Kind = ENode_Undefined;
		m_Flags = 0;
		m_Index = -1;
	}

	virtual
	~CNode ()
	{
	}
};

//.............................................................................

template <class TToken>
class CTokenNodeT: public CNode
{
public:
	typedef TToken CToken;

public:
	CToken m_Token;

public:
	CTokenNodeT ()
	{
		m_Kind = ENode_Token;
	}
};

//.............................................................................

enum ESymbolNodeFlag
{
	ESymbolNodeFlag_IsStacked = 0x0010,
	ESymbolNodeFlag_IsNamed   = 0x0020,
	ESymbolNodeFlag_IsPragma  = 0x0040,
	ESymbolNodeFlag_HasEnter  = 0x0100,
	ESymbolNodeFlag_HasLeave  = 0x0200,
	ESymbolNodeFlag_KeepAst   = 0x0400,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <class TAst>
class CSymbolNodeT: public CNode
{
public:
	typedef TAst CAst;

public:
	CAst* m_pAst;

	rtl::CStdListT <CNode> m_LocatorList;
	rtl::CArrayT <CNode*> m_LocatorArray;

public:
	CSymbolNodeT ()
	{
		m_Kind = ENode_Symbol;
		m_pAst = NULL;
	}

	virtual
	~CSymbolNodeT ()
	{
		if (m_pAst && !(m_Flags & ESymbolNodeFlag_KeepAst))
			AXL_MEM_DELETE (m_pAst);
	}
};

//.............................................................................

enum ELaDfaNodeFlag
{
	ELaDfaNodeFlag_IsPreResolver      = 0x0010,
	ELaDfaNodeFlag_HasChainedResolver = 0x0020,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <class TToken>
class CLaDfaNodeT: public CNode
{
public:
	typedef TToken CToken;

public:
	size_t m_ResolverThenIndex;
	size_t m_ResolverElseIndex;

	rtl::CBoxListT <CToken> m_DfaTokenList;
	rtl::CBoxListT <CToken> m_ResolverTokenList;

public:
	CLaDfaNodeT ()
	{
		m_Kind = ENode_LaDfa;
		m_ResolverThenIndex = -1;
		m_ResolverElseIndex = -1;
	}
};

//.............................................................................

} // namespace llk
} // namespace axl