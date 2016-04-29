// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2015. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_ZIP_ZIP_H

#include "axl_sl_String.h"
#include "axl_sl_Array.h"

struct mz_zip_archive_tag;

namespace axl {
namespace zip {

//.............................................................................

struct ZipFileInfo
{
	uint32_t m_centralDirOffset;
	uint16_t m_versionMadeBy;
	uint16_t m_versionNeeded;
	uint16_t m_bitFlags;
	uint16_t m_method;
	uint64_t m_timestamp;
	uint32_t m_crc32;
	uint64_t m_compressedSize;
	uint64_t m_uncompressedSize;
	uint16_t m_internalAttr;
	uint32_t m_externalAttr;
	uint64_t m_localHeaderOffset;
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class ZipReader
{
protected:
	mz_zip_archive_tag* m_zip;
	
public:
	ZipReader ()
	{
		m_zip = NULL;
	}
	
	~ZipReader ()
	{
		close ();
	}

	bool
	isOpen ()
	{
		return m_zip != NULL;
	}

	void
	close ();

	bool
	openFile (const char* fileName);

	bool
	openMem (
		const void* p,
		size_t size
		);

	size_t
	getFileCount ();

	sl::String
	getFileName (size_t index);

	bool
	getFileInfo (
		size_t index,
		ZipFileInfo* fileInfo,
		sl::String* comment = NULL
		);

	bool 
	isDirectoryFile (size_t index);

	bool 
	isFileEncrypted (size_t index);

	bool
	extractFileToMem (
		size_t index,
		void* p,
		size_t size
		);

	bool
	extractFileToMem (
		size_t index,
		sl::Array <char>* buffer
		);

	sl::Array <char>
	extractFileToMem (size_t index)
	{
		sl::Array <char> buffer;
		extractFileToMem (index, &buffer);
		return buffer;
	}

	bool
	extractFileToFile (
		size_t index,
		const char* fileName
		);
};

//.............................................................................

} // namespace zip
} // namespace axl