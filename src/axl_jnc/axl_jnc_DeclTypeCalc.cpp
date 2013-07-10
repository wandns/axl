#include "pch.h"
#include "axl_jnc_DeclTypeCalc.h"
#include "axl_jnc_Module.h"

namespace axl {
namespace jnc {

//.............................................................................

CDeclTypeCalc::CDeclTypeCalc ()
{
	m_pModule = NULL;
	m_TypeModifiers = 0;
}

CType*
CDeclTypeCalc::CalcType (
	CDeclarator* pDeclarator,
	uint_t* pFlags
	)
{
	CType* pType = pDeclarator->GetBaseType (); 
	rtl::CArrayT <uint_t> PointerPrefixArray = pDeclarator->GetPointerPrefixArray ();
	m_Suffix = pDeclarator->GetSuffixList ().GetTail ();
	m_pModule = pType->GetModule ();

	size_t PointerPrefixCount = PointerPrefixArray.GetCount ();
	for (size_t i = 0; i < PointerPrefixCount; i++)
	{
		EType TypeKind = pType->GetTypeKind ();

		m_TypeModifiers = PointerPrefixArray [i];
		if (m_TypeModifiers & ETypeModifier_Function)
		{
			CFunctionType* pFunctionType = GetFunctionType (pType);
			if (!pFunctionType)
				return NULL;

			pType = GetFunctionPtrType (pFunctionType);
		}
		else if (m_TypeModifiers & ETypeModifier_Property)
		{
			CPropertyType* pPropertyType = GetPropertyType (pType);
			if (!pPropertyType)
				return NULL;

			pType = GetPropertyPtrType (pPropertyType);
		}
		else if (m_TypeModifiers & ETypeModifier_AutoEv)
		{
			CClassType* pAutoEvType = GetAutoEvType (pType);
			if (!pAutoEvType)
				return NULL;

			pType = GetClassPtrType (pAutoEvType);
		}
		else switch (TypeKind)
		{
		case EType_Class:
			pType = GetClassPtrType ((CClassType*) pType);
			break;

		case EType_Function:
			pType = GetFunctionPtrType ((CFunctionType*) pType);
			break;

		case EType_Property:
			pType = GetPropertyPtrType ((CPropertyType*) pType);
			break;

		case EType_NamedImport:
			pType = GetImportPtrType ((CNamedImportType*) pType);
			break;

		default:
			pType = GetDataPtrType (pType);
		}

		if (!pType || !CheckUnusedModifiers ())
			return NULL;
	}

	m_TypeModifiers |= pDeclarator->GetTypeModifiers ();

	if (m_TypeModifiers & ETypeModifier_Property)
	{
		pType = GetPropertyType (pType);
		if (!pType)
			return NULL;

		if (pFlags)
			*pFlags = GetPropertyFlags ();
	}
	else if (m_TypeModifiers & ETypeModifier_Bindable)
	{
		pType = GetBindableDataType (pType);
		if (!pType)
			return NULL;

		if (pFlags)
			*pFlags = EPropertyFlag_AutoGet | EPropertyFlag_AutoSet;
	}
	else if (m_TypeModifiers & ETypeModifier_Multicast)
	{
		pType = GetMulticastType (pType);
		if (!pType)
			return NULL;
	}
	else if (m_TypeModifiers & ETypeModifier_AutoEv)
	{
		pType = GetAutoEvType (pType);
		if (!pType)
			return NULL;
	}

	while (m_Suffix)
	{
		CDeclSuffix* pSuffix = (CDeclSuffix*) *m_Suffix;
		EDeclSuffix SuffixKind = pSuffix->GetSuffixKind ();

		switch (SuffixKind)
		{
		case EDeclSuffix_Array:
			pType = GetArrayType (pType);
			break;

		case EDeclSuffix_Function:
			if (m_TypeModifiers & ETypeModifier_AutoEv)
				pType = GetAutoEvType (pType);
			else
				pType = GetFunctionType (pType);

			if (!CheckUnusedModifiers ())
				return NULL;

			break;
		}

		if (!pType)
			return NULL;
	}

	if (!(pType->GetTypeKindFlags () & ETypeKindFlag_Code))
	{
		if (pFlags)
			*pFlags = GetPtrTypeFlags ();
	}

	if (!CheckUnusedModifiers ())
		return NULL;

	return pType;
}

CType*
CDeclTypeCalc::CalcPtrType (
	CType* pType,
	uint_t TypeModifiers
	)
{
	m_TypeModifiers = TypeModifiers;

	EType TypeKind = pType->GetTypeKind ();
	switch (TypeKind)
	{
	case EType_Class:
		pType = GetClassPtrType ((CClassType*) pType);
		break;

	case EType_Function:
		pType = GetFunctionPtrType ((CFunctionType*) pType);
		break;

	case EType_Property:
		pType = GetPropertyPtrType ((CPropertyType*) pType);
		break;

	default:
		pType = GetDataPtrType (pType);
	}

	if (!CheckUnusedModifiers ())
		return NULL;

	return pType;
}

bool
CDeclTypeCalc::CheckUnusedModifiers ()
{
	if (m_TypeModifiers)
	{
		err::SetFormatStringError ("unused modifier '%s'", GetTypeModifierString (m_TypeModifiers).cc ());
		return false;
	}

	return true;
}

uint_t 
CDeclTypeCalc::GetPtrTypeFlags ()
{
	uint_t Flags = 0;

	if (m_TypeModifiers & ETypeModifier_Const)
		Flags |= EPtrTypeFlag_Const;

	if (m_TypeModifiers & ETypeModifier_Volatile)
		Flags |= EPtrTypeFlag_Volatile;

	m_TypeModifiers &= ~ETypeModifierMask_DataPtr;
	return Flags;
}

uint_t 
CDeclTypeCalc::GetPropertyFlags ()
{
	uint_t Flags = 0;

	if (m_TypeModifiers & ETypeModifier_AutoGet)
		Flags |= EPropertyFlag_AutoGet;

	m_TypeModifiers &= ~ETypeModifier_AutoGet;
	return Flags;
}

CType*
CDeclTypeCalc::GetIntegerType (CType* pType)
{
	ASSERT (m_TypeModifiers & ETypeModifierMask_Integer);

	if (!(pType->GetTypeKindFlags () & ETypeKindFlag_Integer))
	{
		err::SetFormatStringError ("'%s' modifier cannot be applied to '%s'", 
			GetTypeModifierString (m_TypeModifiers & ETypeModifierMask_Integer).cc (), 
			pType->GetTypeString ().cc () 
			);
		return NULL;
	}

	if (m_TypeModifiers & ETypeModifier_Unsigned)
	{
		EType ModTypeKind = GetUnsignedIntegerTypeKind (pType->GetTypeKind ());
		pType = m_pModule->m_TypeMgr.GetPrimitiveType (ModTypeKind);
	}

	if (m_TypeModifiers & ETypeModifier_BigEndian)
	{
		EType ModTypeKind = GetBigEndianIntegerTypeKind (pType->GetTypeKind ());
		pType = m_pModule->m_TypeMgr.GetPrimitiveType (ModTypeKind);
	}

	m_TypeModifiers &= ~ETypeModifierMask_Integer;
	return pType;
}

CArrayType*
CDeclTypeCalc::GetArrayType (CType* pElementType)
{
	ASSERT (m_Suffix && m_Suffix->GetSuffixKind () == EDeclSuffix_Array);

	CDeclArraySuffix* pSuffix = (CDeclArraySuffix*) *m_Suffix--;

	EType TypeKind = pElementType->GetTypeKind ();
	switch (TypeKind)
	{
	case EType_Class:
	case EType_Void:
	case EType_Function:
	case EType_Property:
		err::SetFormatStringError (
			"cannot create array of '%s'", 
			pElementType->GetTypeString ().cc () 
			);
		return NULL;

		pElementType = GetClassPtrType ((CClassType*) pElementType);
		break;

	default:
		if (m_TypeModifiers & ETypeModifierMask_Integer)
		{
			pElementType = GetIntegerType (pElementType);
			if (!pElementType)
				return NULL;
		}
	}

	return m_pModule->m_TypeMgr.GetArrayType (pElementType, pSuffix->GetElementCount ());
}

CType*
CDeclTypeCalc::PrepareReturnType (CType* pType)
{
	while (m_Suffix && m_Suffix->GetSuffixKind () == EDeclSuffix_Array)
	{
		pType = GetArrayType (pType);
		if (!pType)
			return NULL;
	}

	EType TypeKind = pType->GetTypeKind ();
	switch (TypeKind)
	{
	case EType_Class:
	case EType_Function:
	case EType_Property:
		err::SetFormatStringError (
			"function cannot return '%s'", 
			pType->GetTypeString ().cc () 
			);
		return NULL;

	default:
		if (m_TypeModifiers & ETypeModifierMask_Integer)
			pType = GetIntegerType (pType);
	}

	return pType;
}

CFunctionType*
CDeclTypeCalc::GetFunctionType (CType* pReturnType)
{
	pReturnType = PrepareReturnType (pReturnType);
	if (!pReturnType)
		return NULL;

	if (!m_Suffix || m_Suffix->GetSuffixKind () != EDeclSuffix_Function)
	{
		err::SetFormatStringError ("missing function suffix");
		return NULL;
	}

	CDeclFunctionSuffix* pSuffix = (CDeclFunctionSuffix*) *m_Suffix--;

	ECallConv CallConv = GetCallConvFromModifiers (m_TypeModifiers);

	m_TypeModifiers &= ~ETypeModifierMask_Function;

	return m_pModule->m_TypeMgr.CreateUserFunctionType (
		CallConv,
		pReturnType, 
		pSuffix->GetArgArray (),
		pSuffix->GetFunctionTypeFlags ()
		);
}

CPropertyType*
CDeclTypeCalc::GetPropertyType (CType* pReturnType)
{
	pReturnType = PrepareReturnType (pReturnType);
	if (!pReturnType)
		return NULL;

	if (pReturnType->GetTypeKind () == EType_Void)
	{
		err::SetFormatStringError ("property cannot return 'void'");
		return NULL;
	}

	ECallConv CallConv = GetCallConvFromModifiers (m_TypeModifiers);

	uint_t TypeFlags = 0;
	if (m_TypeModifiers & ETypeModifier_Const)
		TypeFlags |= EPropertyTypeFlag_Const;

	if (m_TypeModifiers & ETypeModifier_Bindable)
		TypeFlags |= EPropertyTypeFlag_Bindable;

	m_TypeModifiers &= ~ETypeModifierMask_Property;

	if (!(m_TypeModifiers & ETypeModifier_Indexed))
	{
		return m_pModule->m_TypeMgr.GetSimplePropertyType (
			CallConv, 
			pReturnType, 
			TypeFlags
			);
	}
	else
	{
		if (!m_Suffix || m_Suffix->GetSuffixKind () != EDeclSuffix_Function)
		{
			err::SetFormatStringError ("missing indexed property suffix");
			return NULL;
		}

		CDeclFunctionSuffix* pSuffix = (CDeclFunctionSuffix*) *m_Suffix--;
		return m_pModule->m_TypeMgr.CreateIndexedPropertyType (
			CallConv, 
			pReturnType, 
			pSuffix->GetArgArray (),
			TypeFlags
			);
	}
}

CPropertyType*
CDeclTypeCalc::GetBindableDataType (CType* pDataType)
{
	pDataType = PrepareReturnType (pDataType);
	if (!pDataType)
		return NULL;

	if (pDataType->GetTypeKind () == EType_Void)
	{
		err::SetFormatStringError ("bindable data cannot be 'void'");
		return NULL;
	}

	if (m_TypeModifiers & ETypeModifier_Indexed)
	{
		err::SetFormatStringError ("bindable data cannot be 'indexed'");
		return NULL;
	}

	ECallConv CallConv = GetCallConvFromModifiers (m_TypeModifiers);
	
	m_TypeModifiers &= ~ETypeModifierMask_Property;
	return m_pModule->m_TypeMgr.GetSimplePropertyType (CallConv, pDataType, EPropertyTypeFlag_Bindable);
}

CClassType*
CDeclTypeCalc::GetAutoEvType (CType* pReturnType)
{
	CFunctionType* pStartMethodType = GetFunctionType (pReturnType);
	if (!pStartMethodType)
		return NULL;

	m_TypeModifiers &= ~ETypeModifier_AutoEv;
	return m_pModule->m_TypeMgr.GetAutoEvInterfaceType (pStartMethodType);
}

CClassType*
CDeclTypeCalc::GetMulticastType (CType* pLeftType)
{
	CFunctionPtrType* pPtrType;

	EType TypeKind = pLeftType->GetTypeKind ();
	if (TypeKind == EType_FunctionPtr)
	{
		pPtrType = (CFunctionPtrType*) pLeftType;
	}
	else if (TypeKind == EType_Function)
	{
		pPtrType = GetFunctionPtrType ((CFunctionType*) pLeftType);
		if (!pPtrType)
			return NULL;
	}
	else 
	{
		CFunctionType* pFunctionType = GetFunctionType (pLeftType);
		if (!pFunctionType)
			return NULL;
		
		pPtrType = GetFunctionPtrType (pFunctionType);
		if (!pPtrType)
			return NULL;
	}

	m_TypeModifiers &= ~ETypeModifier_Multicast;
	return m_pModule->m_TypeMgr.GetMulticastType (pPtrType);
}

CDataPtrType* 
CDeclTypeCalc::GetDataPtrType (CType* pDataType)
{
	if (m_TypeModifiers & ETypeModifierMask_Integer)
	{
		pDataType = GetIntegerType (pDataType);
		if (!pDataType)
			return NULL;
	}

	EDataPtrType PtrTypeKind = (m_TypeModifiers & ETypeModifier_Thin) ? EDataPtrType_Thin : EDataPtrType_Normal;
	uint_t TypeFlags = GetPtrTypeFlagsFromModifiers (m_TypeModifiers);

	m_TypeModifiers &= ~ETypeModifierMask_DataPtr;
	return pDataType->GetDataPtrType (PtrTypeKind, TypeFlags);
}

CClassPtrType*
CDeclTypeCalc::GetClassPtrType (CClassType* pClassType)
{
	EClassPtrType PtrTypeKind = (m_TypeModifiers & ETypeModifier_Weak) ? EClassPtrType_Weak : EClassPtrType_Normal;
	uint_t TypeFlags = GetPtrTypeFlagsFromModifiers (m_TypeModifiers);

	m_TypeModifiers &= ~ETypeModifierMask_ClassPtr;
	return pClassType->GetClassPtrType (PtrTypeKind, TypeFlags);
}

CFunctionPtrType*
CDeclTypeCalc::GetFunctionPtrType (CFunctionType* pFunctionType)
{
	EFunctionPtrType PtrTypeKind = 
		(m_TypeModifiers & ETypeModifier_Weak) ? EFunctionPtrType_Weak : 
		(m_TypeModifiers & ETypeModifier_Thin) ? EFunctionPtrType_Thin : EFunctionPtrType_Normal;

	uint_t TypeFlags = GetPtrTypeFlagsFromModifiers (m_TypeModifiers);

	m_TypeModifiers &= ~ETypeModifierMask_FunctionPtr;
	return pFunctionType->GetFunctionPtrType (PtrTypeKind);
}

CPropertyPtrType*
CDeclTypeCalc::GetPropertyPtrType (CPropertyType* pPropertyType)
{
	EPropertyPtrType PtrTypeKind = 
		(m_TypeModifiers & ETypeModifier_Weak) ? EPropertyPtrType_Weak : 
		(m_TypeModifiers & ETypeModifier_Thin) ? EPropertyPtrType_Thin : EPropertyPtrType_Normal;

	uint_t TypeFlags = GetPtrTypeFlagsFromModifiers (m_TypeModifiers);

	m_TypeModifiers &= ~ETypeModifierMask_PropertyPtr;
	return pPropertyType->GetPropertyPtrType (PtrTypeKind);
}

CImportPtrType*
CDeclTypeCalc::GetImportPtrType (CNamedImportType* pImportType)
{	
	uint_t TypeModifiers = m_TypeModifiers & ETypeModifierMask_ImportPtr;
	m_TypeModifiers &= ~ETypeModifierMask_ImportPtr;	
	return m_pModule->m_TypeMgr.GetImportPtrType (pImportType, TypeModifiers);
}

//.............................................................................

} // namespace jnc {
} // namespace axl {
