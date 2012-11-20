#include "stdafx.h"
#include "axl_jnc_Value.h"
#include "axl_jnc_Module.h"

namespace axl {
namespace jnc {

//.............................................................................
	
class CLlvmPodArray: public llvm::ConstantDataSequential 
{
public:
	static
	llvm::Constant*
	Get (
		CArrayType* pType,
		const void* p
		)
	{
		llvm::Type* pLlvmType = pType->GetLlvmType ();
		return getImpl (llvm::StringRef ((char*) p, pType->GetSize ()), pLlvmType);
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CLlvmPodStruct: public llvm::ConstantStruct
{
public:
	static
	llvm::Constant*
	Get (
		CStructType* pType,
		const void* p
		)
	{
		llvm::Type* pLlvmType = pType->GetLlvmType ();
		
		char Buffer [256];
		rtl::CArrayT <llvm::Constant*> LlvmMemberArray (ref::EBuf_Stack, Buffer, sizeof (Buffer));

		rtl::CIteratorT <CStructMember> Member = pType->GetFirstMember ();
		for (; Member; Member++)
		{
			CValue MemberConst (Member->GetType (), (char*) p + Member->GetOffset ());
			LlvmMemberArray.Append ((llvm::Constant*) MemberConst.GetLlvmValue ());
		}

		return get (
			(llvm::StructType*) pLlvmType, 
			llvm::ArrayRef <llvm::Constant*> (LlvmMemberArray, LlvmMemberArray.GetCount ())
			);
	}
};

//.............................................................................

const tchar_t*
GetValueKindString (EValue ValueKind)
{
	switch (ValueKind)
	{
	case EValue_Void:
		return _T("void");

	case EValue_Null:	
		return _T("null");

	case EValue_Type:
		return _T("type");

	case EValue_Const:
		return _T("const");

	case EValue_Variable:
		return _T("variable");

	case EValue_Function:
		return _T("function");

	case EValue_FunctionOverload:
		return _T("function-overload");

	case EValue_Property:
		return _T("property");

	case EValue_LlvmRegister:
		return _T("llvm-register");

	case EValue_BoolNot:
		return _T("bool-not");

	case EValue_BoolAnd:
		return _T("bool-and");

	case EValue_BoolOr:
		return _T("bool-or");

	default:
		return _T("<undefined>");
	}
}

//.............................................................................

CValue::CValue ()
{
	m_ValueKind = EValue_Void;
	m_pType = NULL;
	m_pLlvmValue = NULL;

	memset (m_ConstBuffer, 0, sizeof (m_ConstBuffer));
	m_Const.SetBuffer (ref::EBuf_Field, m_ConstBuffer, sizeof (m_ConstBuffer));
}

CValue::CValue (
	CType* pType,
	const void* p
	):
	m_Const (ref::EBuf_Field, m_ConstBuffer, sizeof (m_ConstBuffer))
{
	m_pLlvmValue = NULL;

	CreateConst (pType, p);
}

void
CValue::Clear ()
{
	m_ValueKind = EValue_Void;
	m_pType = NULL;	
	m_pLlvmValue = NULL;
}

llvm::Value*
CValue::GetLlvmValue () const
{
	if (m_pLlvmValue)
		return m_pLlvmValue;

	if (m_ValueKind != EValue_Const)
		return NULL;

	ASSERT (m_pType);
	m_pLlvmValue = GetLlvmConst (m_pType, GetConstData ());
	return m_pLlvmValue;
}

llvm::Constant*
CValue::GetLlvmConst (
	CType* pType,
	const void* p
	)
{
	int64_t Integer;
	double Double;
	llvm::Constant* pLlvmConst = NULL;

	EType TypeKind = pType->GetTypeKind ();
	switch (TypeKind)
	{
	case EType_Int8:
	case EType_Int8_u:
	case EType_Int16:
	case EType_Int16_u:
	case EType_Int32:
	case EType_Int32_u:
	case EType_Int64:
	case EType_Int64_u:
		Integer = *(int64_t*) p;
		pLlvmConst = llvm::ConstantInt::get (
			pType->GetLlvmType (),
			llvm::APInt (pType->GetSize () * 8, Integer, pType->IsSignedType ())
			);
		break;

	case EType_Float:
		Double = *(float*) p;
		pLlvmConst = llvm::ConstantFP::get (pType->GetLlvmType (), Double);
		break;

	case EType_Double:
		Double = *(double*) p;
		pLlvmConst = llvm::ConstantFP::get (pType->GetLlvmType (), Double);
		break;

	case EType_Array:
		pLlvmConst = CLlvmPodArray::Get ((CArrayType*) pType, p);
		break;

	case EType_Struct:
		pLlvmConst = CLlvmPodStruct::Get ((CStructType*) pType, p);
		break;

	case EType_Pointer:
	case EType_Reference:
		pLlvmConst = CLlvmPodStruct::Get (pType->GetModule ()->m_TypeMgr.GetSafePtrStructType (), p);
		break;

	case EType_Pointer_u:
		Integer = *(int64_t*) p;

		pLlvmConst = llvm::ConstantInt::get (
			pType->GetModule ()->m_TypeMgr.GetBasicType (EType_Int_pu)->GetLlvmType (),
			llvm::APInt (sizeof (void*) * 8, Integer, false)
			);

		pLlvmConst = llvm::ConstantExpr::getIntToPtr (
			pLlvmConst, 
			pType->GetLlvmType ()
			);

		break;

	default:
		ASSERT (false);
	}

	return pLlvmConst;
}

void
CValue::OverrideType (EType TypeKind)
{
	CModule* pModule = GetCurrentThreadModule ();
	ASSERT (pModule);

	CType* pType = pModule->m_TypeMgr.GetBasicType (TypeKind);
	OverrideType (pType);
}

void
CValue::SetNull ()
{
	CModule* pModule = GetCurrentThreadModule ();
	ASSERT (pModule);

	Clear ();
	m_ValueKind = EValue_Null;
	m_pType = pModule->m_TypeMgr.GetBasicType (EType_Void)->GetPointerType (EType_Pointer);
}

void
CValue::SetType (CType* pType)
{
	m_ValueKind = EValue_Type;
	m_pType = pType;
}

void
CValue::SetVariable (
	CVariable* pVariable,
	llvm::Value* pLlvmValue,
	CType* pType
	)
{
	m_ValueKind = EValue_Variable;
	m_pType = pType->GetPointerType (EType_Reference);
	m_pVariable = pVariable;
	m_pLlvmValue = pLlvmValue;
}

void
CValue::SetVariable (CVariable* pVariable)
{
	return SetVariable (pVariable, pVariable->GetLlvmValue (), pVariable->GetType ());
}

void
CValue::SetFunction (CFunction* pFunction)
{
	m_ValueKind = EValue_Function;
	m_pType = pFunction->GetType ();
	m_pFunction = pFunction;
	m_pLlvmValue = pFunction->GetLlvmFunction ();
}

void
CValue::SetFunctionOverload (CFunctionOverload* pFunctionOverload)
{
	m_ValueKind = EValue_FunctionOverload;
	m_pType = pFunctionOverload->GetOverloadCount () == 1 ? pFunctionOverload->GetFunction ()->GetType () : NULL;
	m_pFunctionOverload = pFunctionOverload;
}

void
CValue::SetProperty (CProperty* pProperty)
{
	m_ValueKind = EValue_Property;
	m_pType = pProperty->GetType ();
	m_pProperty = pProperty;
}

bool
CValue::CreateConst (
	CType* pType,
	const void* p
	)
{
	size_t Size = pType->GetSize ();

	bool Result = m_Const.GetBuffer (sizeof (TBufHdr) + Size) != NULL;
	if (!Result)
		return false;

	m_ValueKind = EValue_Const;
	m_pType = pType;
	m_Const->m_Size = Size;

	if (p)
		memcpy (GetConstData (), p, Size);
	else
		memset (GetConstData (), 0, Size);

	return true;	
}

bool
CValue::CreateConst (
	EType TypeKind,
	const void* p
	)
{
	CModule* pModule = GetCurrentThreadModule ();
	ASSERT (pModule);

	CType* pType = pModule->m_TypeMgr.GetBasicType (TypeKind);
	return CreateConst (pType, p);	
}

void
CValue::SetLiteralA (
	const char* p,
	size_t Length
	)
{
	CModule* pModule = GetCurrentThreadModule ();
	ASSERT (pModule);

	if (Length == -1)
		Length = p ? strlen (p) : 0;

	CreateConst (pModule->m_TypeMgr.GetLiteralTypeA (Length));
	char* pDst = (char*) GetConstData ();
	memcpy (pDst, p, Length * sizeof (char));
	pDst [Length] = 0;
}

void
CValue::SetLiteralW (
	const wchar_t* p,
	size_t Length
	)
{
	CModule* pModule = GetCurrentThreadModule ();
	ASSERT (pModule);

	if (Length == -1)
		Length = p ? wcslen (p) : 0;

	CreateConst (pModule->m_TypeMgr.GetLiteralTypeW (Length));
	wchar_t* pDst = (wchar_t*) GetConstData ();
	memcpy (pDst, p, Length * sizeof (wchar_t));
	pDst [Length] = 0;
}

void
CValue::SetLlvmRegister (
	llvm::Value* pValue,
	CType* pType
	)
{
	m_ValueKind = EValue_LlvmRegister;
	m_pType = pType;
	m_pLlvmValue = pValue;
}

void
CValue::SetLlvmRegister (
	llvm::Value* pValue,
	EType TypeKind
	)
{
	CModule* pModule = GetCurrentThreadModule ();
	ASSERT (pModule);

	CType* pType = pModule->m_TypeMgr.GetBasicType (TypeKind);
	SetLlvmRegister (pValue, pType);
}

//.............................................................................

} // namespace axl {
} // namespace jnc {
