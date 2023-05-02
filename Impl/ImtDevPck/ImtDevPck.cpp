#include "ImtDevPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDevPck
{


I_EXPORT_PACKAGE(
			"ImtDevPck",
			"Device components", 
			IM_PROJECT("ImtCore") IM_COMPANY("ImagingTools") "Device Hardware");

I_EXPORT_COMPONENT(
			CompositeDeviceController,
			"Composite device controller for hierarchical access and enumeration",
			"Composite Device Controller");

I_EXPORT_COMPONENT(
			DeviceDataFilePersistence,
			"File persistence for the device data",
			"File Device Persistence Storage EEPROM");


} // namespace ImtDevPck


