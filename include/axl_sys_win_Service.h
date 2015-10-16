// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2014. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_SYS_WIN_SERVICE_H

#include "axl_sys_win_Handle.h"
#include "axl_err_Error.h"

namespace axl {
namespace sys {
namespace win {

//.............................................................................

class Service: public ServiceHandle
{
public:
	bool
	start (
		const wchar_t* argv [],
		size_t argc
		)
	{
		bool_t result = ::StartServiceW (m_h, argc, argv);
		return err::complete (result);
	}

	bool
	start ()
	{
		return start (NULL, 0);
	}

	bool
	stop ()
	{
		SERVICE_STATUS serviceStatus;
		return control (SERVICE_CONTROL_STOP, &serviceStatus);
	}

	bool 
	control (
		dword_t code,
		SERVICE_STATUS* serviceStatus
		)
	{
		bool_t result = ::ControlService (m_h, code, serviceStatus);
		return err::complete (result);
	}

	bool 
	remove ()
	{
		bool_t result = ::DeleteService (m_h);
		return err::complete (result);
	}

	bool
	setServiceType (dword_t serviceType)
	{
		return changeServiceConfig (serviceType, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE);
	}

	bool
	setStartType (dword_t startType)
	{
		return changeServiceConfig (SERVICE_NO_CHANGE, startType, SERVICE_NO_CHANGE);
	}

	bool
	setErrorControl (dword_t errorControl)
	{
		return changeServiceConfig (SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, errorControl);
	}

	bool
	setDescription (const wchar_t* description)
	{
		SERVICE_DESCRIPTIONW info = { (WCHAR*) description };
		return changeServiceConfig2 (SERVICE_CONFIG_DESCRIPTION, &info);
	}

	bool 
	queryServiceStatus (SERVICE_STATUS* serviceStatus)
	{
		bool_t result = ::QueryServiceStatus (m_h, serviceStatus);
		return err::complete (result);
	}

	bool 
	changeServiceConfig (
		dword_t serviceType,
		dword_t startType,
		dword_t errorControl,
		const wchar_t* binaryFilePath = NULL,
		const wchar_t* loadOrderGroup = NULL,
		dword_t* tagId = NULL,
		const wchar_t* dependencies = NULL,
		const wchar_t* serviceStartName = NULL,
		const wchar_t* password = NULL,
		const wchar_t* displayName = NULL
		)
	{
		bool_t result = ::ChangeServiceConfigW (
			m_h,
			serviceType,
			startType,
			errorControl,
			binaryFilePath,
			loadOrderGroup,
			tagId,
			dependencies,
			serviceStartName,
			password,
			displayName
			);

		return err::complete (result);
	}

	bool 
	changeServiceConfig2 (
		dword_t infoLevel,
		const void* info
		)
	{
		bool_t result = ::ChangeServiceConfig2W (m_h, infoLevel, (void*) info);
		return err::complete (result);
	}
};

//.............................................................................

} // namespace win
} // namespace sys 
} // namespace axl
