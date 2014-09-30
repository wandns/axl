// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2014. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_err_ErrorMgr.h"
#include "axl_lex_Token.h"
#include "axl_rtl_Pack.h"

namespace axl {
namespace err {

//.............................................................................

// {56FC601E-5D2C-4bbe-B455-3D8B81AAD8FF}
AXL_RTL_DEFINE_GUID (GUID_ParseError, 0x56fc601e, 0x5d2c, 0x4bbe, 0xb4, 0x55, 0x3d, 0x8b, 0x81, 0xaa, 0xd8, 0xff);

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

enum ParseErrorKind
{
	ParseErrorKind_SrcPos,
	ParseErrorKind_InvalidSyntax,
	ParseErrorKind_InvalidSyntaxIn,
	ParseErrorKind_ExpectedToken,
	ParseErrorKind_UnexpectedToken,
	ParseErrorKind_UnexpectedTokenIn,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class ParseErrorProvider: public ErrorProvider
{
public:
	virtual 
	rtl::String 
	getErrorDescription (const ErrorData* error);
};

//.............................................................................

// utility

inline 
void 
registerParseErrorProvider ()
{
	getErrorMgr ()->registerProvider (
		GUID_ParseError, 
		rtl::getSimpleSingleton <ParseErrorProvider> ()
		);
}

inline
Error
pushSrcPosError (
	const char* filePath,
	int line,
	int col = 0
	)
{
	return pushPackError <rtl::PackSeq_3 <const char*, int, int> > (
		GUID_ParseError,
		ParseErrorKind_SrcPos, 
		filePath, 
		line, 
		col
		);
}

inline
Error
pushSrcPosError (
	const char* filePath,
	const lex::LineCol& lineCol
	)
{
	return pushSrcPosError (filePath, lineCol.m_line, lineCol.m_col);
}

inline
Error
pushSrcPosError (const lex::SrcPos& srcPos)
{
	return pushSrcPosError (srcPos.m_filePath, srcPos.m_line, srcPos.m_col);
}

inline
Error
ensureSrcPosError (
	const char* filePath,
	int line,
	int col = 0
	)
{
	Error error = getError ();
	return error->isKind (GUID_ParseError, ParseErrorKind_SrcPos) ? error : pushSrcPosError (filePath, line, col);
}

inline
Error
ensureSrcPosError (
	const char* filePath,
	const lex::LineCol& lineCol
	)
{
	return ensureSrcPosError (filePath, lineCol.m_line, lineCol.m_col);
}

inline
Error
ensureSrcPosError (const lex::SrcPos& srcPos)
{
	return ensureSrcPosError (srcPos.m_filePath, srcPos.m_line, srcPos.m_col);
}

inline
Error
setSyntaxError ()
{
	return setError (GUID_ParseError, ParseErrorKind_InvalidSyntax);
}

inline
Error
setSyntaxError (const char* location)
{
	return setPackError <rtl::Pack <const char*> > (
		GUID_ParseError,
		ParseErrorKind_InvalidSyntaxIn, 
		location
		);
}

inline
Error
setExpectedTokenError (
	const char* expectedToken,
	const char* actualToken
	)
{
	return setPackError <rtl::PackSeq_2 <const char*, const char*> > (
		GUID_ParseError,
		ParseErrorKind_ExpectedToken, 
		expectedToken,
		actualToken
		);
}

inline
Error
setUnexpectedTokenError (const char* token)
{
	return setPackError <rtl::Pack <const char*> > (
		GUID_ParseError,
		ParseErrorKind_UnexpectedToken, 
		token
		);
}

inline
Error
setUnexpectedTokenError (
	const char* token,
	const char* location
	)
{
	return setPackError <rtl::PackSeq_2 <const char*, const char*> > (
		GUID_ParseError,
		ParseErrorKind_UnexpectedTokenIn, 
		token,
		location
		);
}

//.............................................................................

} // namespace err
} // namespace axl

