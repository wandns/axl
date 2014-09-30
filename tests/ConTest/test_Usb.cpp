#include "pch.h"

using namespace axl;

namespace test_Usb { 

//.............................................................................

void 
run ()
{
	bool result;

	io::UsbContext context;

	io::UsbDeviceList deviceList;
	size_t count = deviceList.enumerateDevices (context);
	if (count == -1)
	{
		printf ("Cannot enumerate USB devices (%s)\n", err::getError ()->getDescription ().cc ());
		return;
	}

	rtl::String bufferString;
	bufferString.getBuffer (4096);

	libusb_device** pp = deviceList;
	for (; *pp; pp++)
	{
		io::UsbDevice device;
		device.setDevice (*pp);

		libusb_device_descriptor descriptor;
		device.getDeviceDescriptor (&descriptor);

		printf (
			"VID_%04x&PID_%04x (Bus: %d Address: %d)\n", 
			descriptor.idVendor,
			descriptor.idProduct,
			device.getBusNumber (), 
			device.getDeviceAddress ()
			);

		result = device.open ();
		if (!result)
			continue;

		result = device.getStringDesrciptor (descriptor.iManufacturer, &bufferString);
		if (result)
			printf ("\tManufacturer:  %s\n", bufferString.cc ());

		result = device.getStringDesrciptor (descriptor.iProduct, &bufferString);
		if (result)
			printf ("\tProduct name:  %s\n", bufferString.cc ());

		result = device.getStringDesrciptor (descriptor.iSerialNumber, &bufferString);
		if (result)
			printf ("\tSerial number: %s\n", bufferString.cc ());
	}

}

//.............................................................................

} // namespace test_Vso
