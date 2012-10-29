#include "stdafx.h"
#include "axl_jnc_CastOperator.h"
#include "axl_jnc_Module.h"

namespace axl {
namespace jnc {

//.............................................................................

CCastOperator::CCastOperator ()
{
	m_Price = 1;
	m_CastKind = ECast_Implicit;
	m_pOperatorLo = NULL;
	m_pDstType = NULL;
	m_pSrcType = NULL;
}

bool
CCastOperator::Cast (
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	CModule* pModule = GetCurrentThreadModule ();
	ASSERT (pModule);

	EValue ValueKind = Value.GetValueKind ();
	if (ValueKind == EValue_Const)
	{
		return 
			pResultValue->CreateConst (pType) &&
			m_pOperatorLo->ConstCast (Value, *pResultValue);
	}

	ASSERT (ValueKind == EValue_Const || ValueKind == EValue_Variable || ValueKind == EValue_LlvmRegister);
	return m_pOperatorLo->LlvmCast (pModule, Value, pType, pResultValue);
}

//.............................................................................

CSuperCast::CSuperCast ()
{
	m_pIntermediateType = NULL;
	m_pFirst = NULL;
	m_pSecond = NULL;
}

bool
CSuperCast::ConstCast (
	const CValue& SrcValue,
	const CValue& DstValue
	)
{
	ASSERT (SrcValue.GetValueKind () == EValue_Const && DstValue.GetValueKind () == EValue_Const);

	CValue TempValue;
	return 
		TempValue.CreateConst (m_pIntermediateType);
		m_pFirst->ConstCast (SrcValue, TempValue) &&
		m_pSecond->ConstCast (TempValue, DstValue);
}

bool
CSuperCast::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	CValue TempValue;
	return 
		m_pFirst->LlvmCast (pModule, Value, m_pIntermediateType, &TempValue) &&
		m_pSecond->LlvmCast (pModule, TempValue, pType, pResultValue);
}

//.............................................................................

bool
CCast_cpy::ConstCast (
	const CValue& SrcValue,
	const CValue& DstValue
	)
{
	size_t SrcSize = SrcValue.GetType ()->GetSize ();
	size_t DstSize = DstValue.GetType ()->GetSize ();

	ASSERT (SrcSize == DstSize);

	memcpy (DstValue.GetConstData (), SrcValue.GetConstData (), DstSize);
	return true;
}

bool
CCast_cpy::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	if (pType->Cmp (Value.GetType ()) == 0)
	{
		*pResultValue = Value;
		return true;
	}

	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmTrunc = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateBitCast (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmTrunc, pType);
	return true;
}

//.............................................................................

bool
CCast_int_trunc::ConstCast (
	const CValue& SrcValue,
	const CValue& DstValue
	)
{
	size_t SrcSize = SrcValue.GetType ()->GetSize ();
	size_t DstSize = DstValue.GetType ()->GetSize ();

	ASSERT (SrcSize > DstSize);

	memcpy (DstValue.GetConstData (), SrcValue.GetConstData (), DstSize);
	return true;
}

bool
CCast_int_trunc::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmTrunc = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateTrunc (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmTrunc, pType);
	return true;
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool
CCast_int_ext::ConstCast (
	const CValue& SrcValue,
	const CValue& DstValue
	)
{
	size_t SrcSize = SrcValue.GetType ()->GetSize ();
	size_t DstSize = DstValue.GetType ()->GetSize ();

	ASSERT (SrcSize < DstSize);

	char* pSrc = (char*) SrcValue.GetConstData ();
	char* pDst = (char*) DstValue.GetConstData ();

	if (pSrc [SrcSize - 1] < 0)
		memset (pDst, -1, DstSize);
	else
		memset (pDst, 0, DstSize);

	memcpy (pDst, pSrc, SrcSize);

	*(int16_t*) DstValue.GetConstData () = *(int8_t*) SrcValue.GetConstData ();
	return true;
}

bool
CCast_int_ext::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmExt = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateSExt (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmExt, pType);
	return true;
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool
CCast_int_ext_u::ConstCast (
	const CValue& SrcValue,
	const CValue& DstValue
	)
{
	size_t SrcSize = SrcValue.GetType ()->GetSize ();
	size_t DstSize = DstValue.GetType ()->GetSize ();

	ASSERT (SrcSize < DstSize);

	char* pSrc = (char*) SrcValue.GetConstData ();
	char* pDst = (char*) DstValue.GetConstData ();

	memset (pDst, 0, DstSize);
	memcpy (pDst, pSrc, SrcSize);

	*(int16_t*) DstValue.GetConstData () = *(int8_t*) SrcValue.GetConstData ();
	return true;
}

bool
CCast_int_ext_u::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmExt = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateZExt (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmExt, pType);
	return true;
}

//.............................................................................

bool
CCast_i16_swp::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	// y = (x & 0x00ff) << 8 | (x & 0xff00) >> 8

	llvm::Type* pLlvmType = pModule->m_TypeMgr.GetBasicType (EType_Int16)->GetLlvmType ();

	const uint64_t Mask1 = 0x00ff;
	const uint64_t Mask2 = 0xff00;
	const uint64_t BitCount = 8;

	llvm::Value* pLlvmMaskValue1 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (16, Mask1, false));
	llvm::Value* pLlvmMaskValue2 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (16, Mask2, false));
	llvm::Value* pLlvmBitCountValue = llvm::ConstantInt::get (pLlvmType, llvm::APInt (16, BitCount, false));

	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);

	llvm::Value* pLlvmByte1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue1);
	pLlvmByte1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateShl (pLlvmByte1, pLlvmBitCountValue);

	llvm::Value* pLlvmByte2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue2);
	pLlvmByte2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateLShr (pLlvmByte2, pLlvmBitCountValue);

	llvm::Value* pLlvmResult = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmByte1, pLlvmByte2);
	pResultValue->SetLlvmRegister (pLlvmResult, pType);
	return true;
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool
CCast_i32_swp::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	// y = (x & 0x000000ff) << 24 | (x & 0x0000ff00) << 8 | (x & 0x00ff0000) >> 8 | (x & 0xff000000) >> 24

	llvm::Type* pLlvmType = pModule->m_TypeMgr.GetBasicType (EType_Int32)->GetLlvmType ();

	const uint64_t Mask1 = 0x000000ff;
	const uint64_t Mask2 = 0x0000ff00;
	const uint64_t Mask3 = 0x00ff0000;
	const uint64_t Mask4 = 0xff000000;
	const uint64_t BitCount1 = 8;
	const uint64_t BitCount2 = 24;

	llvm::Value* pLlvmMaskValue1 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (32, Mask1, false));
	llvm::Value* pLlvmMaskValue2 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (32, Mask2, false));
	llvm::Value* pLlvmMaskValue3 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (32, Mask3, false));
	llvm::Value* pLlvmMaskValue4 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (32, Mask4, false));
	llvm::Value* pLlvmBitCountValue1 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (32, BitCount1, false));
	llvm::Value* pLlvmBitCountValue2 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (32, BitCount2, false));

	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);

	llvm::Value* pLlvmByte1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue1);
	pLlvmByte1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateShl (pLlvmByte1, pLlvmBitCountValue2);

	llvm::Value* pLlvmByte2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue2);
	pLlvmByte2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateShl (pLlvmByte2, pLlvmBitCountValue1);

	llvm::Value* pLlvmWord1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmByte1, pLlvmByte2);

	llvm::Value* pLlvmByte3 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue3);
	pLlvmByte3 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateLShr (pLlvmByte3, pLlvmBitCountValue1);

	llvm::Value* pLlvmByte4 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue4);
	pLlvmByte4 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateLShr (pLlvmByte4, pLlvmBitCountValue2);

	llvm::Value* pLlvmWord2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmByte3, pLlvmByte4);

	llvm::Value* pLlvmResult = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmWord1, pLlvmWord2);
	pResultValue->SetLlvmRegister (pLlvmResult, pType);
	return true;
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool
CCast_i64_swp::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	// same as above, just longer ;)

	llvm::Type* pLlvmType = pModule->m_TypeMgr.GetBasicType (EType_Int64)->GetLlvmType ();

	const uint64_t Mask1 = 0x00000000000000ff;
	const uint64_t Mask2 = 0x000000000000ff00;
	const uint64_t Mask3 = 0x0000000000ff0000;
	const uint64_t Mask4 = 0x00000000ff000000;
	const uint64_t Mask5 = 0x000000ff00000000;
	const uint64_t Mask6 = 0x0000ff0000000000;
	const uint64_t Mask7 = 0x00ff000000000000;
	const uint64_t Mask8 = 0xff00000000000000;
	const uint64_t BitCount1 = 8;
	const uint64_t BitCount2 = 24;
	const uint64_t BitCount3 = 40;
	const uint64_t BitCount4 = 56;

	llvm::Value* pLlvmMaskValue1 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, Mask1, false));
	llvm::Value* pLlvmMaskValue2 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, Mask2, false));
	llvm::Value* pLlvmMaskValue3 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, Mask3, false));
	llvm::Value* pLlvmMaskValue4 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, Mask4, false));
	llvm::Value* pLlvmMaskValue5 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, Mask5, false));
	llvm::Value* pLlvmMaskValue6 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, Mask6, false));
	llvm::Value* pLlvmMaskValue7 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, Mask7, false));
	llvm::Value* pLlvmMaskValue8 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, Mask8, false));
	llvm::Value* pLlvmBitCountValue1 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, BitCount1, false));
	llvm::Value* pLlvmBitCountValue2 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, BitCount2, false));
	llvm::Value* pLlvmBitCountValue3 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, BitCount3, false));
	llvm::Value* pLlvmBitCountValue4 = llvm::ConstantInt::get (pLlvmType, llvm::APInt (64, BitCount4, false));

	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);

	llvm::Value* pLlvmByte1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue1);
	pLlvmByte1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateShl (pLlvmByte1, pLlvmBitCountValue4);

	llvm::Value* pLlvmByte2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue2);
	pLlvmByte2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateShl (pLlvmByte2, pLlvmBitCountValue3);

	llvm::Value* pLlvmWord1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmByte1, pLlvmByte2);

	llvm::Value* pLlvmByte3 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue3);
	pLlvmByte3 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateShl (pLlvmByte3, pLlvmBitCountValue2);

	llvm::Value* pLlvmByte4 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue4);
	pLlvmByte4 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateShl (pLlvmByte4, pLlvmBitCountValue1);

	llvm::Value* pLlvmWord2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmByte3, pLlvmByte4);
	llvm::Value* pLlvmDWord1 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmWord1, pLlvmWord2);

	llvm::Value* pLlvmByte5 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue5);
	pLlvmByte5 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateLShr (pLlvmByte5, pLlvmBitCountValue1);

	llvm::Value* pLlvmByte6 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue6);
	pLlvmByte6 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateLShr (pLlvmByte6, pLlvmBitCountValue2);

	llvm::Value* pLlvmWord3 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmByte5, pLlvmByte6);

	llvm::Value* pLlvmByte7 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue7);
	pLlvmByte7 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateLShr (pLlvmByte7, pLlvmBitCountValue3);

	llvm::Value* pLlvmByte8 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateAnd (pLlvmValue, pLlvmMaskValue8);
	pLlvmByte8 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateLShr (pLlvmByte8, pLlvmBitCountValue4);

	llvm::Value* pLlvmWord4 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmByte7, pLlvmByte8);
	llvm::Value* pLlvmDWord2 = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmWord3, pLlvmWord4);

	llvm::Value* pLlvmResult = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateOr (pLlvmDWord1, pLlvmDWord2);
	pResultValue->SetLlvmRegister (pLlvmResult, pType);
	return true;
}

//.............................................................................

bool
CCast_f64_f32::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmExt = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFPTrunc (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmExt, pType);
	return true;
}

//.............................................................................

bool
CCast_f32_f64::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmExt = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFPExt (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmExt, pType);
	return true;
}

//.............................................................................

bool
CCast_int_fp::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmExt = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateSIToFP (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmExt, pType);
	return true;
}

//.............................................................................

bool
CCast_uint_fp::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmExt = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateUIToFP (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmExt, pType);
	return true;
}

//.............................................................................

bool
CCast_fp_int::LlvmCast (
	CModule* pModule,
	const CValue& Value,
	CType* pType,
	CValue* pResultValue
	)
{
	llvm::Value* pLlvmValue = pModule->m_OperatorMgr.LoadValue (Value);
	llvm::Value* pLlvmExt = pModule->m_ControlFlowMgr.GetLlvmBuilder ()->CreateFPToSI (pLlvmValue, pType->GetLlvmType ());
	pResultValue->SetLlvmRegister (pLlvmExt, pType);
	return true;
}

//.............................................................................

} // namespace axl {
} // namespace jnc {
