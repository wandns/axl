// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_pg_NodeMgr.h"
#include "axl_pg_Lexer.h"

namespace axl {
namespace pg {

//.............................................................................

// processes user code within actions & arguments
// removes unnecessary locators,
// assigns slot indexes 

class CProductionBuilder: public CLexer
{
protected:
	enum EVariable
	{
		EVariable_Undefined = 0,
		EVariable_TokenBeacon,
		EVariable_SymbolBeacon,
		EVariable_This,
		EVariable_Arg,
		EVariable_Local,
	};

protected:
	CNodeMgr* m_pNodeMgr;
	
	CSymbolNode* m_pSymbol;
	CGrammarNode* m_pProduction;
	CDispatcherNode* m_pDispatcher;
	CGrammarNode* m_pResolver;

	rtl::CArrayT <CActionNode*> m_ActionArray;
	rtl::CArrayT <CArgumentNode*> m_ArgumentArray;
	rtl::CArrayT <CBeaconNode*> m_BeaconArray;
	rtl::CArrayT <CBeaconNode*> m_BeaconDeleteArray;
	rtl::CHashTableMapT <const tchar_t*, CBeaconNode*, rtl::CHashString, rtl::CCmpString> m_BeaconMap;

public:
	CProductionBuilder (CNodeMgr* pNodeMgr);

	CGrammarNode*
	Build (
		CSymbolNode* pSymbol,
		CGrammarNode* pProduction
		);

protected:
	bool
	Scan (CGrammarNode* pNode);

	bool
	AddBeacon (CBeaconNode* pBeacon);

	void
	FindAndReplaceUnusedBeacons (CGrammarNode*& pNode);

	bool
	ProcessAllUserCode ();

	bool
	ProcessUserCode (
		lex::CSrcPos& SrcPos,
		rtl::CString* pUserCode,
		CGrammarNode* pResolver
		);

	EVariable
	FindVariable (
		int Index,
		CBeaconNode** ppBeacon
		);

	EVariable
	FindVariable (
		const tchar_t* pName,
		CBeaconNode** ppBeacon
		);
};

//.............................................................................

} // namespace pg {
} // namespace axl {