// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
			DeviceControllerProxy,
			"Proxy device controller for hierarchical access and enumeration",
			"Proxy Device Controller");

I_EXPORT_COMPONENT(
			DeviceDataFilePersistence,
			"File persistence for the device data",
			"File Device Persistence Storage EEPROM");

I_EXPORT_COMPONENT(
			DeviceIdBasedConfigurationManager,
			"Device ID based configuration manager",
			"Device Configuration Manager");

I_EXPORT_COMPONENT(
			DeviceIdBasedConfiguration,
			"Device ID based configuration",
			"Device Configuration");

I_EXPORT_COMPONENT(
			DeviceIdBasedAttributes,
			"Device ID based attributes",
			"Device Attributes");

I_EXPORT_COMPONENT(
			DeviceInstanceListAdapter,
			"Device instance list adapter",
			"Device Instance List Adapter");

I_EXPORT_COMPONENT(
			DeviceStateProviderAdapter,
			"Device state provider adapter",
			"Device State Provider Adapter");


} // namespace ImtDevPck


