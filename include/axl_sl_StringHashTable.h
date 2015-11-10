// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2015. All rights reserved
// Author: Vladimir Gladkov

#pragma once 

#define _AXL_SL_STRINGHASHTABLE_H

#include "axl_sl_HashTable.h"

namespace axl {
namespace sl {

//.............................................................................

template <typename C>
class StringHashTableBase: public HashTable <
	const C*,
	sl::HashStringBase <C>, 
	sl::CmpStringBase <C>
	>
{
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

typedef StringHashTableBase <char>      StringHashTable;
typedef StringHashTableBase <wchar_t>   StringHashTable_w;
typedef StringHashTableBase <utf8_t>    StringHashTable_utf8;
typedef StringHashTableBase <utf16_t>   StringHashTable_utf16;
typedef StringHashTableBase <utf32_t>   StringHashTable_utf32;

typedef StringHashTable::Iterator       StringHashTableIterator;
typedef StringHashTable_w::Iterator     StringHashTableIterator_w;
typedef StringHashTable_utf8::Iterator  StringHashTableIterator_utf8;
typedef StringHashTable_utf16::Iterator StringHashTableIterator_utf16;
typedef StringHashTable_utf32::Iterator StringHashTableIterator_utf32;

//.............................................................................

template <
	typename Value,
	typename C
	>
class StringHashTableMapBase: public HashTableMap <
	const C*,
	Value,
	sl::HashStringBase <C>, 
	sl::CmpStringBase <C>
	>
{
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <typename Value>
class StringHashTableMap: public StringHashTableMapBase <Value, char>
{
};

template <typename Value>
class StringHashTableMap_w: public StringHashTableMapBase <Value, wchar_t>
{
};

template <typename Value>
class StringHashTableMap_utf8: public StringHashTableMapBase <Value, utf8_t>
{
};

template <typename Value>
class StringHashTableMap_utf16: public StringHashTableMapBase <Value, utf16_t>
{
};

template <typename Value>
class StringHashTableMap_utf32: public StringHashTableMapBase <Value, utf32_t>
{
};

//.............................................................................

template <
	typename Value,
	typename C = char
	>
class StringHashTableMapIterator: public HashTableMapIterator <const C*, Value>
{
public:
	StringHashTableMapIterator ()
	{ 
	}

	StringHashTableMapIterator (const Iterator <typename StringHashTableMapIterator::Entry>& src)
	{ 
		this->m_p = src.getEntry ();
	}

	StringHashTableMapIterator (typename StringHashTableMapIterator::Entry* p)
	{ 
		operator = (p); 
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <typename Value>
class StringHashTableMapIterator_w: public StringHashTableMapIterator <Value, wchar_t>
{
public:
	StringHashTableMapIterator_w ()
	{ 
	}

	StringHashTableMapIterator_w (const Iterator <typename StringHashTableMapIterator_w::Entry>& src)
	{ 
		this->m_p = src.getLink (); 
	}

	StringHashTableMapIterator_w (typename StringHashTableMapIterator_w::Entry* p)
	{ 
		operator = (p); 
	}
};

//.............................................................................

#define AXL_SL_BEGIN_STRING_HASH_TABLE_MAP(class, Value) \
	AXL_SL_BEGIN_STRING_HASH_TABLE_MAP_T (class, Value, char)

#define AXL_SL_BEGIN_STRING_HASH_TABLE_MAP_T(class, Value, C) \
	AXL_SL_BEGIN_HASH_TABLE_MAP ( \
		class, \
		const C*, \
		Value, \
		sl::HashStringBase <C>, \
		sl::CmpStringBase <C> \
		)

//.............................................................................

} // namespace sl
} // namespace axl
