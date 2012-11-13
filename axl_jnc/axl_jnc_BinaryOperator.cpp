#include "stdafx.h"
#include "axl_jnc_BinaryOperator.h"
#include "axl_jnc_Module.h"

namespace axl {
namespace jnc {

//.............................................................................

const tchar_t*
GetBinOpString (EBinOp OpKind)
{
	switch (OpKind)
	{
	case EBinOp_Add:
		return _T("+");

	case EBinOp_Sub:
		return _T("-");

	case EBinOp_Mul:
		return _T("*");

	case EBinOp_Div:
		return _T("/");

	case EBinOp_Mod:
		return _T("%");

	case EBinOp_Shl:
		return _T("<<");

	case EBinOp_Shr:
		return _T(">>");

	case EBinOp_BitwiseAnd:
		return _T("&");

	case EBinOp_BitwiseOr:
		return _T("|");

	case EBinOp_BitwiseXor:
		return _T("^");

	case EBinOp_Eq:
		return _T("==");

	case EBinOp_Ne:
		return _T("!=");

	case EBinOp_Lt:
		return _T("<");

	case EBinOp_Le:
		return _T("<=");

	case EBinOp_Gt:
		return _T(">");

	case EBinOp_Ge:
		return _T(">=");

	case EBinOp_LogicalAnd:
		return _T("&&");

	case EBinOp_LogicalOr:
		return _T("||");

	case EBinOp_Idx:
		return _T("index-operator");

	default:
		return _T("undefined-binary-operator");
	};
}

//.............................................................................

IBinaryOperator::IBinaryOperator()
{
	m_pModule = GetCurrentThreadModule ();
	ASSERT (m_pModule);

	m_OpKind = EBinOp_None;
}

//.............................................................................

void
CBinaryOperatorOverload::Clear ()
{
	m_SignatureCache.Clear ();
	m_DirectMap.Clear ();
	m_ImplicitMap.Clear ();
}

IBinaryOperator*
CBinaryOperatorOverload::GetOperator (
	CType* pOpType1,
	CType* pOpType2,
	TBinaryOperatorTypeInfo* pTypeInfo
	)
{
	bool Result;

	rtl::CStringA Signature = pOpType1->GetSignature () + pOpType2->GetSignature ();
	rtl::CHashTableMapIteratorT <const tchar_t*, IBinaryOperator*> It = m_DirectMap.Find (Signature);
	if (!It)
		It = m_ImplicitMap.Find (Signature);
	
	if (It)
	{
		IBinaryOperator* pOperator = It->m_Value;
		if (!pOperator)
			return NULL;

		Result = pOperator->GetTypeInfo (pOpType1, pOpType2, pTypeInfo);
		ASSERT (Result);
		return pOperator;
	}

	// ok, we need to enumerate all overloads

	IBinaryOperator* pBestOperator = NULL;
	TBinaryOperatorTypeInfo BestTypeInfo;

	for (It = m_DirectMap.GetHead (); It; It++)
	{
		IBinaryOperator* pOperator = It->m_Value;
		
		TBinaryOperatorTypeInfo TypeInfo;
		bool Result = pOperator->GetTypeInfo (pOpType1, pOpType2, &TypeInfo);
		if (!Result)
			continue;

		if (!pBestOperator || TypeInfo.m_CastKind < BestTypeInfo.m_CastKind)
		{
			pBestOperator = pOperator;
			BestTypeInfo = TypeInfo;
		}	
	}

	m_SignatureCache.InsertTail (Signature);
	It = m_ImplicitMap.Goto (Signature); 
	It->m_Value = pBestOperator;

	if (pBestOperator)
		*pTypeInfo = BestTypeInfo;

	return pBestOperator;
}

IBinaryOperator*
CBinaryOperatorOverload::AddOperator (
	CType* pOpType1,
	CType* pOpType2,
	IBinaryOperator* pOperator
	)
{
	rtl::CStringA Signature = pOpType1->GetSignature () + pOpType2->GetSignature ();
	rtl::CHashTableMapIteratorT <const tchar_t*, IBinaryOperator*> It = m_DirectMap.Goto (Signature);

	IBinaryOperator* pPrevOperator = It->m_Value;
	if (!pPrevOperator)
		m_SignatureCache.InsertTail (Signature);

	It->m_Value = pOperator;
	return pPrevOperator;
}

//.............................................................................

bool
GetStdBinaryOperatorTypeInfo (
	CModule* pModule,
	CType* pReturnType,
	CType* pOpType,
	CType* pOpType1,
	CType* pOpType2,
	TBinaryOperatorTypeInfo* pTypeInfo
	)
{
	ECast CastKind1 = pModule->m_OperatorMgr.GetCastKind (pOpType1, pOpType);
	ECast CastKind2 = pModule->m_OperatorMgr.GetCastKind (pOpType2, pOpType);

	if (!CastKind1 || !CastKind2)
		return false;

	pTypeInfo->m_CastKind = min (CastKind1, CastKind2);
	pTypeInfo->m_pOpType1 = pOpType;
	pTypeInfo->m_pOpType2 = pOpType;
	pTypeInfo->m_pReturnType = pReturnType;
	return true;
}

bool
GetStdBinaryOperatorTypeInfo (
	CModule* pModule,
	EType ReturnTypeKind,
	EType OpTypeKind,
	CType* pOpType1,
	CType* pOpType2,
	TBinaryOperatorTypeInfo* pTypeInfo
	)
{
	CType* pReturnType = pModule->m_TypeMgr.GetBasicType (ReturnTypeKind);
	CType* pOpType = pModule->m_TypeMgr.GetBasicType (OpTypeKind);
	return GetStdBinaryOperatorTypeInfo (pModule, pReturnType, pOpType, pOpType1, pOpType2, pTypeInfo);
}

bool
GetCmpBinaryOperatorTypeInfo (
	CModule* pModule,
	CType* pOpType,
	CType* pOpType1,
	CType* pOpType2,
	TBinaryOperatorTypeInfo* pTypeInfo
	)
{
	CType* pReturnType = pModule->m_TypeMgr.GetBasicType (EType_Bool);
	return GetStdBinaryOperatorTypeInfo (pModule, pReturnType, pOpType, pOpType1, pOpType2, pTypeInfo);
}

//.............................................................................

llvm::Value*
CBinOp_Add::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAdd (pOpValue1, pOpValue2);
}

	
llvm::Value*
CBinOp_Add::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFAdd (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Sub::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateSub (pOpValue1, pOpValue2);
}

	
llvm::Value*
CBinOp_Sub::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFSub (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	
llvm::Value*
CBinOp_Mul::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateMul (pOpValue1, pOpValue2);
}
	
llvm::Value*
CBinOp_Mul::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFMul (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Div::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateSDiv (pOpValue1, pOpValue2);
}

	
llvm::Value*
CBinOp_Div::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFDiv (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	
llvm::Value*
CBinOp_Div_u::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateUDiv (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Mod::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateSRem (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Mod_u::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateURem (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Shl::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateShl (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Shr::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateLShr (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_BitwiseAnd::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_BitwiseOr::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_BitwiseXor::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateXor (pOpValue1, pOpValue2);
}

//.............................................................................

llvm::Value*
CBinOp_Eq::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpEQ (pOpValue1, pOpValue2);
}

llvm::Value*
CBinOp_Eq::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFCmpOEQ (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Ne::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpNE (pOpValue1, pOpValue2);
}

llvm::Value*
CBinOp_Ne::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFCmpONE (pOpValue1, pOpValue2);
}
	
//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Lt::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpSLT (pOpValue1, pOpValue2);
}

llvm::Value*
CBinOp_Lt::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFCmpOLT (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Lt_u::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpULT (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Le::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpSLE (pOpValue1, pOpValue2);
}

llvm::Value*
CBinOp_Le::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFCmpOLE (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Le_u::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpULE (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Gt::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpSGT (pOpValue1, pOpValue2);
}

llvm::Value*
CBinOp_Gt::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFCmpOGT (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Gt_u::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpUGT (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Ge::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpSGE (pOpValue1, pOpValue2);
}

llvm::Value*
CBinOp_Ge::LlvmOpFp (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFCmpOGE (pOpValue1, pOpValue2);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

llvm::Value*
CBinOp_Ge_u::LlvmOpInt (
	CModule* pModule,
	llvm::Value* pOpValue1,
	llvm::Value* pOpValue2
	)
{
	return pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateICmpUGE (pOpValue1, pOpValue2);
}

//.............................................................................

} // namespace axl {
} // namespace jnc {