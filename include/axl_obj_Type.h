// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2013. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_OBJ_TYPE_H

#include "axl_rtl_SimpleSingleton.h"
#include "axl_rtl_Guid.h"
#include "axl_rtl_Func.h"

namespace axl {
namespace obj {
	
//.............................................................................

// compile-time type information

template <typename T>
class CTypeT
{
public:
	static
	size_t 
	GetSize ()
	{ 
		return sizeof (T);
	}

	static
	const char*
	GetName ()
	{ 
		return typeid (T).name ();
	}

	static
	void
	Construct (void* p)
	{
		new (p) T;
	}

	static
	void
	Destruct (void* p)
	{
		((T*) p)->~T ();
	}

	static
	void
	Copy (
		void* p,
		const void* pSrc
		)
	{
		*(T*) p = *(const T*) pSrc;
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// specialization for void

template <>
class CTypeT <void>
{
public:
	static
	size_t 
	GetSize ()
	{ 
		return 0;
	}

	static
	const char*
	GetName ()
	{ 
		return typeid (void).name ();
	}

	static
	void
	Construct (void* p)
	{
	}

	static
	void
	Destruct (void* p)
	{
	}

	static
	void
	Copy (
		void* p,
		const void* pSrc
		)
	{
	}
};

//.............................................................................

// run-time type information

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// the only interface not derived from root (can't get type of a type of a type...)

struct IType
{
protected:
	size_t m_Size;
	const char* m_pName;
	const rtl::TGuid* m_pGuid;
	
public:
	size_t 
	GetSize ()
	{ 
		return m_Size;
	}

	const char*
	GetName ()
	{ 
		return m_pName;
	}

	const rtl::TGuid*
	GetGuid ()
	{ 
		return m_pGuid;
	}

	virtual
	void
	Construct (void* p) = 0;

	virtual
	void
	Destruct (void* p) = 0;

	virtual
	void
	Copy (
		void* p,
		const void* pSrc
		) = 0;

	virtual
	size_t
	GetInterfaceOffset (const rtl::TGuid& Guid) = 0;

	bool
	HasInterface (const rtl::TGuid& Guid)
	{
		return GetInterfaceOffset (Guid) != -1;
	}
};

//.............................................................................

template <typename T>
class ITypeSimpleImplT: public IType
{
public:
	typedef CTypeT <T> CType;

	ITypeSimpleImplT ()
	{
		m_Size = CType::GetSize ();
		m_pName = CType::GetName ();
	}

	virtual
	void
	Construct (void* p)
	{
		CType::Construct (p);
	}

	virtual
	void
	Destruct (void* p)
	{
		CType::Destruct (p);
	}

	virtual
	void
	Copy (
		void* p,
		const void* pSrc
		)
	{
		CType::Copy (p, pSrc);
	}

	virtual
	size_t
	GetInterfaceOffset (const rtl::TGuid& Guid)
	{
		return -1;
	}

	static
	ITypeSimpleImplT*
	GetSingleton ()
	{
		return rtl::GetSimpleSingleton <ITypeSimpleImplT> ();
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <typename T>
class ITypeClassImplT: public ITypeSimpleImplT <T>
{
public:
	ITypeClassImplT ()
	{
		this->m_pGuid = T::__GetGuid (); // thanks a lot gcc
	}

	virtual
	size_t
	GetInterfaceOffset (const rtl::TGuid& Guid)
	{
		return T::__GetInterfaceOffset (Guid);
	}

	static
	ITypeClassImplT*
	GetSingleton ()
	{
		return rtl::GetSimpleSingleton <ITypeClassImplT> ();
	}
};

//.............................................................................

// convenient macros for accessing IType* singletons

#define AXL_OBJ_TYPEOF(Class) \
	axl::obj::ITypeSimpleImplT <Class>::GetSingleton ()

#define AXL_OBJ_CLASSOF(Class) \
	axl::obj::ITypeClassImplT <Class>::GetSingleton ()

//.............................................................................

// tagging with GUIDs

#define AXL_OBJ_GUID_TAG(l, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
	static \
	const axl::rtl::TGuid* \
	__GetGuid () \
	{ \
		static axl::rtl::TGuid GuidTag = AXL_RTL_GUID_INITIALIZER(l, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8); \
		return &GuidTag; \
	}

// use existing GUID

#define AXL_OBJ_GUID_TAG_E(Guid) \
	static \
	const axl::rtl::TGuid* \
	__GetGuid () \
	{ \
		return &Guid; \
	}

#define AXL_OBJ_GUIDOF(Class) \
	(*(Class::__GetGuid ()))

//.............................................................................

// convenient macros for organizing interface maps

#define AXL_OBJ_BEGIN_INTERFACE_MAP(Class) \
	typedef Class __CThisClass; \
	static \
	size_t \
	__GetInterfaceOffset (const rtl::TGuid& Guid) \
	{

#define AXL_OBJ_INTERFACE_ENTRY(I) \
		if (Guid == AXL_OBJ_GUIDOF (I)) \
			return rtl::COffsetOfClassT <__CThisClass, I> () (); \

#define AXL_OBJ_INTERFACE_CHAIN(B) \
		{ \
		size_t Offset = B::__GetInterfaceOffset (Guid); \
		if (Offset != -1) \
			return rtl::COffsetOfClassT <__CThisClass, B> () () + Offset; \
		}

#define AXL_OBJ_END_INTERFACE_MAP() \
		return  -1; \
	}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

#define AXL_OBJ_SIMPLE_INTERFACE_MAP(Class, I) \
	AXL_OBJ_BEGIN_INTERFACE_MAP (Class) \
		AXL_OBJ_INTERFACE_ENTRY (I) \
	AXL_OBJ_END_INTERFACE_MAP ()

//.............................................................................

// tag with GUID and create a singe-entry interface map, so when deriving from 
// a single interface it is not required to explicitly specify interface map

#define AXL_OBJ_INTERFACE(Iface, l, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AXL_OBJ_GUID_TAG (l, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AXL_OBJ_SIMPLE_INTERFACE_MAP (Iface, Iface)

//.............................................................................

#define AXL_OBJ_IMPLEMENT_GET_OBJECT(Class) \
	virtual \
	void* \
	GetObject (axl::obj::IType** ppType) \
	{ \
		if (ppType) \
			*ppType = AXL_OBJ_CLASSOF (Class); \
		return this; \
	}	

// will be redefined in axl_ref_RefCount.h

#define AXL_OBJ_REF_COUNT_MISCREATION_GUARD(Class) 

//.............................................................................

// convenient macros for declaring creatable classes

#define AXL_OBJ_BEGIN_CLASS(Class, l, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AXL_OBJ_GUID_TAG (l, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AXL_OBJ_IMPLEMENT_GET_OBJECT (Class) \
	AXL_OBJ_REF_COUNT_MISCREATION_GUARD (Class) \
	AXL_OBJ_BEGIN_INTERFACE_MAP (Class)

#define AXL_OBJ_BEGIN_CLASS_E(Class, Guid) \
	AXL_OBJ_GUID_TAG_E (Guid) \
	AXL_OBJ_IMPLEMENT_GET_OBJECT (Class) \
	AXL_OBJ_REF_COUNT_MISCREATION_GUARD (Class) \
	AXL_OBJ_BEGIN_INTERFACE_MAP (Class)

#define AXL_OBJ_BEGIN_CLASS_0(Class) \
	AXL_OBJ_GUID_TAG_E (axl::rtl::GUID_Null) \
	AXL_OBJ_IMPLEMENT_GET_OBJECT (Class) \
	AXL_OBJ_REF_COUNT_MISCREATION_GUARD (Class) \
	AXL_OBJ_BEGIN_INTERFACE_MAP (Class)

#define AXL_OBJ_END_CLASS() \
	AXL_OBJ_END_INTERFACE_MAP ()

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

#define AXL_OBJ_CLASS(Class, Iface, l, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AXL_OBJ_BEGIN_CLASS (Class, l, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AXL_OBJ_INTERFACE_ENTRY (Iface) \
	AXL_OBJ_END_CLASS()

#define AXL_OBJ_CLASS_E(Class, Iface, Guid) \
	AXL_OBJ_BEGIN_CLASS_E (Class, Guid) \
	AXL_OBJ_INTERFACE_ENTRY (Iface) \
	AXL_OBJ_END_CLASS()

#define AXL_OBJ_CLASS_0(Class, Iface) \
	AXL_OBJ_BEGIN_CLASS_0 (Class) \
	AXL_OBJ_INTERFACE_ENTRY (Iface) \
	AXL_OBJ_END_CLASS()

//.............................................................................

} // namespace obj
} // namespace axl
