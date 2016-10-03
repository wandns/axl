// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2015. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_SL_ARRAY_H

#include "axl_sl_ArrayDetails.h"
#include "axl_err_Error.h"

namespace axl {
namespace sl {

//.............................................................................

template <typename T>
class ArrayRef
{
public:
	typedef ArrayDetails <T> Details;
	typedef typename Details::Hdr Hdr;

protected:
	T* m_p;
	Hdr* m_hdr;
	size_t m_count;

public:
	ArrayRef ()
	{
		initialize ();
	}

	ArrayRef (const ArrayRef& src)
	{
		initialize ();
		attach (src);
	}

	ArrayRef (
		Hdr* hdr,
		const T* p,
		size_t count
		)
	{
		initialize ();
		attach (hdr, p, count);
	}

	ArrayRef (
		Hdr* hdr,
		const T* p,
		const void* end
		)
	{
		initialize ();
		attach (hdr, p, (T*) end - p);
	}

	ArrayRef (
		const T* p,
		size_t count
		)
	{
		initialize ();
		attach (NULL, p, count);
	}

	ArrayRef (
		const T* p,
		const void* end
		)
	{
		initialize ();
		attach (NULL, p, (T*) end - p);
	}

	~ArrayRef ()
	{
		release ();
	}

	operator const T* () const
	{
		return m_p;
	}
	
	ArrayRef& 
	operator = (const ArrayRef& src)
	{
		attach (src);
		return *this;
	}	

	const T*
	ca () const
	{
		return m_p;
	}

	size_t
	getCount () const
	{
		return m_count;
	}

	Hdr*
	getHdr () const
	{
		return m_hdr;
	}

	bool
	isEmpty () const
	{
		return m_count == 0;
	}

	const T&
	getFront () const
	{
		ASSERT (m_count);
		return m_p [0];
	}

	T&
	getFront ()
	{
		ASSERT (m_count);
		return m_p [0];
	}

	const T&
	getBack () const
	{
		ASSERT (m_count);
		return m_p [m_count - 1];
	}

	T&
	getBack ()
	{
		ASSERT (m_count);
		return m_p [m_count - 1];
	}

	size_t
	find (T e)
	{
		for (size_t i = 0; i < m_count; i++)
			if (m_p [i] == e)
				return i;

		return -1;
	}

	size_t
	findReverse (T e)
	{
		for (intptr_t i = m_count - 1; i >= 0; i--)
			if (m_p [i] == e)
				return i;

		return -1;
	}

	void
	release ()
	{
		if (m_hdr)
			m_hdr->release ();

		initialize ();
	}

protected:
	void
	initialize ()
	{
		m_p = NULL;
		m_hdr = NULL;
		m_count = 0;
	}

	void
	attach (const ArrayRef& src)
	{
		if (&src != this)
			attach (src.m_hdr, src.m_p, src.m_count);
	}

	void
	attach (
		Hdr* hdr,
		T* p,
		size_t count
		)
	{
		if (hdr != m_hdr) // try to avoid unnecessary interlocked ops
		{
			if (hdr)
				hdr->addRef ();

			if (m_hdr)
				m_hdr->release ();
		
			m_hdr = hdr;
		}

		m_p = p;
		m_count = count;
	}
};

//.............................................................................

template <typename T>
class Array: public ArrayRef <T>
{
public:
	typedef sl::ArrayRef <T> ArrayRef;

	typedef typename ArrayRef::Details Details;
	typedef typename ArrayRef::Hdr Hdr;
	
public:
	Array ()
	{
	}

	Array (const Array& src)
	{
		copy (src);
	}

	Array (const ArrayRef& src)
	{
		copy (src);
	}

	explicit Array (size_t count)
	{
		setCount (count);
	}

	Array (
		const T* p,
		size_t count
		)
	{
		copy (p, count);
	}

	Array (
		const T* p,
		const void* end
		)
	{
		copy (p, (T*) end - p);
	}

	Array (
		ref::BufKind bufKind,
		void* p,
		size_t size
		)
	{
		setBuffer (bufKind, p, size);
	}

	operator const T* () const
	{
		return this->ca ();
	}

	operator T* ()
	{
		return a ();
	}

	Array&
	operator = (const Array& src)
	{
		copy (src);
		return *this;
	}

	Array&
	operator = (const ArrayRef& src)
	{
		copy (src);
		return *this;
	}

	T*
	a ()
	{
		return ensureExclusive () ? this->m_p : NULL;
	}

	void
	clear ()
	{
		setCount (0);
	}

	size_t
	copy (const ArrayRef& src)
	{
		if (&src == this)
			return this->m_count;
		
		Hdr* hdr = src.getHdr ();
		if (!hdr || (hdr->getFlags () & ref::BufHdrFlag_Exclusive))
			return copy (src, src.getCount ());

		this->attach (src);
		return this->m_count;
	}

	size_t
	copyReverse (const ArrayRef& src)
	{
		return copyReverse (src, src.m_count);
	}

	size_t
	copy (
		const T* p,
		size_t count
		)
	{
		if (count == 0)
		{
			clear ();
			return true;
		}

		bool result = setCount (count);
		if (!result)
			return -1;

		Details::copy (this->m_p, p, count);
		return true;
	}

	size_t
	copyReverse (
		const T* p,
		size_t count
		)
	{
		if (count == 0)
		{
			clear ();
			return true;
		}

		bool result = setCount (count);
		if (!result)
			return -1;

		Details::copyReverse (this->m_p, p, count);
		return true;
	}

	size_t
	copy (T e)
	{
		return copy (&e, 1);
	}

	T*
	append (
		const T* p,
		size_t count
		)
	{
		return insert (-1, p, count);
	}

	T*
	appendReverse (
		const T* p,
		size_t count
		)
	{
		return insertReverse (-1, p, count);
	}

	T*
	append (T e)
	{
		return insert (-1, e);
	}

	T*
	appendMultiply (
		T e,
		size_t count
		)
	{
		return insertMultiply (-1, e, count);
	}

	T*
	append (const ArrayRef& src)
	{
		return insert (-1, src, src.getCount ());
	}

	T*
	appendReverse (const ArrayRef& src)
	{
		return insertReverse (-1, src, src.getCount ());
	}

	T*
	insertSpace (
		size_t index,
		size_t count
		)
	{
		size_t oldCount = this->m_count;
		bool result = setCount (this->m_count + count);
		if (!result)
			return NULL;

		if (index > oldCount)
			index = oldCount;

		T* dst = this->m_p + index;

		if (count && index < oldCount)
			Details::copy (dst + count, dst, oldCount - index);

		return dst;
	}

	T*
	insertEmptySpace (
		size_t index,
		size_t count
		)
	{
		T* dst = insertSpace (index, count);
		if (!dst)
			return NULL;

		Details::clear (dst, count);
		return dst;
	}

	T*
	insert (
		size_t index,
		const T* p,
		size_t count
		)
	{
		T* dst = insertSpace (index, count);
		if (!dst)
			return NULL;

		if (p)
			Details::copy (dst, p, count);
		else
			Details::clear (dst, count);

		return dst;
	}

	T*
	insertReverse (
		size_t index,
		const T* p,
		size_t count
		)
	{
		T* dst = insertSpace (index, count);
		if (!dst)
			return NULL;

		if (p)
			Details::copyReverse (dst, p, count);
		else
			Details::clear (dst, count);

		return dst;
	}

	T*
	insert (
		size_t index,
		T e
		)
	{
		T* dst = insertSpace (index, 1);
		if (!dst)
			return NULL;

		*dst = e;
		return dst;
	}

	T*
	insertMultiply (
		size_t index,
		T e,
		size_t count
		)
	{
		T* dst = insertSpace (index, count);
		if (!dst)
			return NULL;

		T* end = dst + count;

		for (; dst < end; dst++)
			*dst = e;

		return dst;
	}

	T*
	insert (
		size_t index,
		const ArrayRef& src
		)
	{
		return insert (index, src, src.getCount ());
	}

	T*
	insertReverse (
		size_t index,
		const ArrayRef& src
		)
	{
		return insertReverse (index, src, src.getCount ());
	}

	bool
	remove (
		size_t index,
		size_t count = 1
		)
	{
		if (count == 0)
			return true;

		size_t oldCount = this->m_count;
		if (index >= oldCount)
			return true;

		if (index + count >= oldCount)
			return setCount (index);

		bool result = ensureExclusive ();
		if (!result)
			return false;

		size_t newCount = oldCount - count;

		T* dst = this->m_p + index;
		Details::copy (dst, dst + count, newCount - index);

		return setCount (newCount);
	}

	size_t
	pop (size_t count = 1)
	{
		if (count >= this->m_count)
			count = this->m_count;

		setCount (this->m_count - count);
		return count;
	}

	T
	getBackAndPop ()
	{
		T e = this->getBack ();
		pop ();
		return e;
	}

	bool
	move (
		size_t indexDst,
		size_t indexSrc,
		size_t count = 1
		)
	{
		if (count == 0 || indexDst == indexSrc)
			return true;

		size_t oldCount = this->m_count;

		if (indexDst + count > oldCount || indexSrc + count > oldCount)
		{
			err::setError (err::SystemErrorCode_InvalidParameter);
			return false;
		}

		T* temp = (T*) AXL_MEM_ALLOCATE (count * sizeof (T));
		if (!temp)
			return false;

		T* dst = this->m_p + indexDst;
		T* src = this->m_p + indexSrc;

		Details::constructCopy (temp, src, count);

		if (indexSrc < indexDst)
			Details::copy (src, src + count, indexDst - indexSrc);
		else
			Details::copy (dst + count, dst, indexSrc - indexDst);

		Details::copy (dst, temp, count);

		Details::destruct (temp, count);
		AXL_MEM_FREE (temp);

		return true;
	}

	void
	reverse (
		size_t index,
		size_t count
		)
	{
		size_t thisCount = this->m_count;

		if (index >= thisCount)
			return;

		if (index + count > thisCount)
			count = thisCount - index;

		Details::reverse (this->m_p + index, count);
	}

	void
	reverse ()
	{
		reverse (0, -1);
	}

	void
	reverseFrom (size_t index)
	{
		reverse (index, -1);
	}

	void
	reverseUntil (size_t index)
	{
		reverse (0, index);
	}

	bool
	ensureExclusive ()
	{
		return this->m_count ? setCount (this->m_count) : true;
	}

	bool
	reserve (size_t count)
	{
		size_t size = count * sizeof (T);

		if (this->m_hdr && 
			this->m_hdr->getRefCount () == 1 &&
			this->m_hdr->m_bufferSize >= size)
			return true;

		size_t bufferSize = sl::getMinPower2Ge (size);

		ref::Ptr <Hdr> hdr = AXL_REF_NEW_EXTRA (Hdr, bufferSize);
		hdr->m_bufferSize = bufferSize;
		Details::setHdrCount (hdr, this->m_count);

		T* p = (T*) (hdr + 1);

		if (this->m_count)
			Details::constructCopy (p, this->m_p, this->m_count);

		if (this->m_hdr)
			this->m_hdr->release ();

		this->m_p = p;
		this->m_hdr = hdr.detach ();
		return true;
	}

	bool
	setCount (size_t count)
	{
		size_t size = count * sizeof (T);

		if (this->m_hdr && 
			this->m_hdr->getRefCount () == 1)
		{
			if (this->m_count == count)
				return true;

			if (this->m_hdr->m_bufferSize >= size)
			{
				if (count > this->m_count)
					Details::construct (this->m_p + this->m_count, count - this->m_count);
				else
					Details::destruct (this->m_p + count, this->m_count - count);

				Details::setHdrCount (this->m_hdr, count);
				this->m_count = count;
				return true;
			}
		}

		if (count == 0)
		{
			this->release ();
			return true;
		}

		if (!this->m_count)
		{
			bool result = reserve (count);
			if (!result)
				return false;

			Details::construct (this->m_p, count);
			Details::setHdrCount (this->m_hdr, count);
			this->m_count = count;
			return true;
		}

		ASSERT (this->m_hdr);

		size_t bufferSize = sl::getMinPower2Ge (size);

		ref::Ptr <Hdr> hdr = AXL_REF_NEW_EXTRA (Hdr, bufferSize);
		if (!hdr)
			return false;

		hdr->m_bufferSize = bufferSize;
		Details::setHdrCount (hdr, count);

		T* p = (T*) (hdr + 1);

		if (count <= this->m_count)
		{
			Details::constructCopy (p, this->m_p, count);
		}
		else
		{
			Details::constructCopy (p, this->m_p, this->m_count);
			Details::construct (p + this->m_count, count - this->m_count);
		}

		this->m_hdr->release ();

		this->m_p = p;
		this->m_hdr = hdr.detach ();
		this->m_count = count;
		return true;
	}
	
	size_t
	ensureCount (size_t count)
	{
		if (this->m_count < count)
			setCount (count);

		return this->m_count;
	}

	size_t
	setBuffer (
		ref::BufKind kind,
		void* p,
		size_t size
		)
	{
		ASSERT (size >= sizeof (Hdr) + sizeof (T));
		
		uint_t flags = kind != ref::BufKind_Static ? ref::BufHdrFlag_Exclusive : 0;
		size_t bufferSize = size - sizeof (Hdr);

		ref::Ptr <Hdr> hdr = AXL_REF_NEW_INPLACE (Hdr, p, flags);
		hdr->m_bufferSize = bufferSize;
		Details::setHdrCount (hdr, 0);

		if (this->m_hdr)
			this->m_hdr->release ();

		this->m_p = (T*) (hdr + 1);
		this->m_hdr = hdr.detach ();
		this->m_count = 0;

		return bufferSize / sizeof (T);
	}
};

//.............................................................................

} // namespace sl
} // namespace axl
