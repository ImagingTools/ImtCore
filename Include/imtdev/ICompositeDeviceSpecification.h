// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdev/IDeviceSpecification.h>
#include <imtbase/ICollectionInfo.h>


namespace imtdev
{


/**
	@brief Interface for composite device static information
	
	ICompositeDeviceSpecification extends IDeviceSpecification to describe static
	properties and capabilities of composite device types. It provides information
	about the sub-devices that can exist within the composite device structure.
	
	While IDeviceSpecification describes a single device type, ICompositeDeviceSpecification
	adds metadata about the hierarchical structure, including specifications for each
	supported sub-device type.
	
	@par Key Features:
	- **Sub-Device Specifications**: Access to static info for each sub-device type
	- **Hierarchical Metadata**: Information about parent-child relationships
	- **Type Validation**: Verify which sub-device types are supported
	- **Default Configurations**: Default sub-device configurations
	
	@par Usage Example:
	@code{.cpp}
	const ICompositeDeviceSpecification* pCompositeSpec = 
		pController->GetDeviceStaticInfo(compositeDeviceTypeId);
	
	if (pCompositeSpec)
	{
		// Access base device specification properties
		QByteArray deviceClass = pCompositeSpec->GetClass();
		QString typeName = pCompositeSpec->GetTypeName();
		
		// Get list of sub-device specifications
		const imtbase::ICollectionInfo& subDeviceList = 
			pCompositeSpec->GetSubDeviceList();
		
		// Iterate through sub-device specifications
		for (int i = 0; i < subDeviceList.GetCount(); ++i)
		{
			QByteArray subDeviceId = GetSubDeviceId(subDeviceList, i);
			
			// Get specification for this sub-device type
			const IDeviceSpecification* pSubSpec = 
				pCompositeSpec->GetSubDeviceSpecification(subDeviceId);
			
			if (pSubSpec)
			{
				QString subDeviceName = pSubSpec->GetTypeName();
				QByteArrayList subCommands = pSubSpec->GetSupportedCommands();
				
				// Sub-device specification might also be composite
				auto pSubCompositeSpec = 
					dynamic_cast<const ICompositeDeviceSpecification*>(pSubSpec);
				if (pSubCompositeSpec)
				{
					// Handle nested composite specification
				}
			}
		}
	}
	@endcode
	
	@par Specification Hierarchy Example:
	@code
	CameraArraySpec (ICompositeDeviceSpecification)
	├── Supports: "StandardCamera" type
	├── Supports: "SensorHub" type
	│
	├── StandardCameraSpec (IDeviceSpecification)
	│   ├── Class: "Camera"
	│   ├── Commands: "CAPTURE", "SET_EXPOSURE", ...
	│   └── Capabilities: DC_READ | DC_WRITE
	│
	└── SensorHubSpec (ICompositeDeviceSpecification)
	    ├── Class: "SensorArray"
	    ├── TemperatureSensorSpec (IDeviceSpecification)
	    └── HumiditySensorSpec (IDeviceSpecification)
	@endcode
	
	@see IDeviceSpecification
	@see ICompositeDeviceInstance
	@see CCompositeDeviceInstanceBase
	@ingroup imtdev
*/
class ICompositeDeviceSpecification: virtual public IDeviceSpecification
{
public:
	/**
		@brief Get the list of sub-device specifications
		
		Returns a collection containing specifications for all sub-device types
		that are part of this composite device. Each specification describes the
		static properties of one sub-device type.
		
		@return Reference to collection info containing sub-device specifications
		@see GetSubDeviceSpecification()
		@see imtbase::ICollectionInfo
	*/
	virtual const imtbase::ICollectionInfo& GetSubDeviceList() const = 0;

	/**
		@brief Get specification for a specific sub-device
		
		Retrieves the device specification that describes the static properties
		of a particular sub-device type within this composite device.
		
		@param id Unique identifier of the sub-device type
		@return Pointer to sub-device specification, or nullptr if not found
		@see GetSubDeviceList()
		@see IDeviceSpecification
	*/
	virtual const IDeviceSpecification* GetSubDeviceSpecification(const QByteArray& id) const = 0;
};


} // namespace imtdev


