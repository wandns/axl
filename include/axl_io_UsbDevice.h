// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2014. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_IO_USBDEVICE_H

#include "axl_io_UsbError.h"
#include "axl_rtl_Handle.h"
#include "axl_rtl_Array.h"

namespace axl {
namespace io {

//.............................................................................

class CloseUsbContext
{
public:
	void
	operator () (libusb_context* h)
	{
		libusb_exit (h);
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class UsbContext: public rtl::Handle <libusb_context*, CloseUsbContext>
{
public:
	UsbContext ()
	{
		open ();
	}

	bool 
	open ();
};

//.............................................................................

class FreeUsbDeviceList
{
public:
	void
	operator () (libusb_device** h)
	{
		libusb_free_device_list (h, true);
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class UsbDeviceList: public rtl::Handle <libusb_device**, FreeUsbDeviceList>
{
public:
	size_t
	enumerateDevices (libusb_context* context);
};

//.............................................................................

class FreeUsbConfigDescriptor
{
public:
	void
	operator () (libusb_config_descriptor* h)
	{
		libusb_free_config_descriptor (h);
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class UsbConfigDescriptor: public rtl::Handle <libusb_config_descriptor*, FreeUsbConfigDescriptor>
{
};

//.............................................................................

class UsbDevice
{
public:
	enum DefKind
	{
		DefKind_BufferSize = 256,
	};

protected:
	libusb_device* m_device;
	libusb_device_handle* m_openHandle;

public:
	UsbDevice ()
	{
		m_device = NULL;
		m_openHandle = NULL;
	}

	~UsbDevice ()
	{
		setDevice (NULL);
	}

	libusb_device* 
	getDevice ()
	{
		return m_device;
	}

	libusb_device_handle* 
	getOpenHandle ()
	{
		return m_openHandle;
	}
	
	bool
	isOpen ()
	{
		return m_openHandle != NULL;
	}

	uint8_t 
	getBusNumber ()
	{
		ASSERT (m_device);
		return libusb_get_bus_number (m_device);
	}

	uint8_t
	getDeviceAddress ()
	{
		ASSERT (m_device);
		return libusb_get_device_address (m_device);
	}

	libusb_speed 
	getDeviceSpeed ()
	{
		ASSERT (m_device);
		return (libusb_speed) libusb_get_device_speed (m_device);
	}

	size_t
	getMaxPacketSize (uint_t endpoint);

	size_t
	getMaxIsoPacketSize (uint_t endpoint);
	
	// open-close

	void
	refDevice ()
	{
		ASSERT (m_device);
		libusb_ref_device (m_device);
	}

	void
	unrefDevice ()
	{
		ASSERT (m_device);
		libusb_unref_device (m_device);
	}

	void
	setDevice (libusb_device* device);

	void
	close ();

	bool
	open ();

	bool
	open (libusb_device* device)
	{
		setDevice (device);
		return open ();
	}

	bool
	open (
		uint_t vendorId,
		uint_t productId
		);

	int
	getConfiguration ();

	bool
	setConfiguration (int configuration);

	bool
	claimInterface (int interface);

	bool
	releaseInterface (int interface);

	bool
	setInterfaceAltSetting (
		int interface,
		int altSetting
		);

	bool
	clearHalt (uint_t endpoint);

	bool
	resetDevice ();

	bool
	isKernelDriverActive (int interface);

	bool
	attachKernelDriver (int interface);

	bool
	detachKernelDriver (int interface);

	// descriptors

	rtl::Array <char>
	getDesrciptor (
		libusb_descriptor_type type, 
		size_t index
		)
	{
		rtl::Array <char> descriptor;
		getDesrciptor (type, index, &descriptor);
		return descriptor;
	}
	
	bool
	getDesrciptor (
		libusb_descriptor_type type, 
		size_t index,
		rtl::Array <char>* descriptor
		);

	bool
	getDeviceDescriptor (libusb_device_descriptor* descriptor);

	bool
	getConfigDescriptor (
		int configuration,
		libusb_config_descriptor** descriptor
		);

	bool
	getActiveConfigDescriptor (libusb_config_descriptor** descriptor);

	rtl::String 
	getStringDesrciptor (
		size_t index,
		uint_t langId
		)
	{
		rtl::String string;
		getStringDesrciptor (index, langId, &string);
		return string;
	}

	bool
	getStringDesrciptor (
		size_t index,
		uint_t langId,
		rtl::String* string
		);

	rtl::String 
	getStringDesrciptor (size_t index)
	{
		rtl::String string;
		getStringDesrciptor (index, &string);
		return string;
	}

	bool
	getStringDesrciptor (
		size_t index,
		rtl::String* string
		);

	// synchronous transfers

	size_t 
	controlTransfer (
		uint_t requestType,
		uint_t request,
		uint_t value,
		size_t index,
		void* p,
		size_t size,
		uint_t timeout = -1
		);

	size_t 
	bulkTransfer (
		uint_t endpoint,
		void* p,
		size_t size,
		uint_t timeout = -1
		);

	size_t 
	interruptTransfer (
		uint_t endpoint,
		void* p,
		size_t size,
		uint_t timeout = -1
		);
};

//.............................................................................

} // namespace io {
} // namespace axl {
