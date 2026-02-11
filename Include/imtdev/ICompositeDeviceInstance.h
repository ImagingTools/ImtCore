// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdev/IDeviceInstance.h>
#include <imtdev/ICompositeDeviceSpecification.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtdev
{


/**
	@brief Interface for composite (hierarchical) device instances
	
	ICompositeDeviceInstance extends IDeviceInstance to support hierarchical device
	structures where a parent device contains one or more sub-devices. This enables
	modeling of complex hardware systems where a single physical unit contains multiple
	logical devices.
	
	This interface implements the **Composite Pattern**, allowing devices to be organized
	in tree structures with parent-child relationships. Sub-devices can themselves be
	composite devices, enabling multi-level hierarchies.
	
	@par Use Cases:
	- Multi-sensor systems (e.g., camera array with multiple sensors)
	- Modular hardware platforms (e.g., controller with plug-in modules)
	- Integrated systems (e.g., multi-function printer with scanner/printer/fax)
	- Device clusters (e.g., distributed sensor network with local controller)
	
	@par Key Features:
	- **Sub-Device Management**: Access to child devices within the composite
	- **Type Filtering**: Query supported sub-device types
	- **Hierarchical Navigation**: Traverse device tree structure
	- **Unified Interface**: Sub-devices use same IDeviceInstance interface
	
	@par Usage Example:
	@code{.cpp}
	// Get composite device instance
	CompositeDeviceInstancePtr pComposite = 
		std::dynamic_pointer_cast<ICompositeDeviceInstance>(pDeviceInstance);
	
	if (pComposite)
	{
		// Get composite device specification
		const ICompositeDeviceSpecification* pSpec = 
			pComposite->GetCompositeDeviceSpecification();
		
		// Check supported sub-device types
		QSet<QByteArray> supportedTypes = pComposite->GetSupportedSubDeviceTypeIds();
		for (const QByteArray& typeId : supportedTypes)
		{
			qDebug() << "Supports sub-device type:" << typeId;
		}
		
		// Get list of available sub-devices
		const imtbase::ICollectionInfo& subDevices = pComposite->GetSubDeviceList();
		
		// Access specific sub-device
		for (int i = 0; i < subDevices.GetCount(); ++i)
		{
			QByteArray subDeviceId = GetSubDeviceId(subDevices, i);
			const IDeviceInstance* pSubDevice = 
				pComposite->GetSubDeviceInstance(subDeviceId);
			
			if (pSubDevice)
			{
				// Work with sub-device as a regular device instance
				const IDeviceSpecification& subSpec = 
					pSubDevice->GetDeviceSpecification();
				
				// Sub-device might also be composite - check recursively
				auto pSubComposite = 
					dynamic_cast<const ICompositeDeviceInstance*>(pSubDevice);
				if (pSubComposite)
				{
					// Handle nested composite device
				}
			}
		}
	}
	@endcode
	
	@par Device Hierarchy Example:
	@code
	CameraArray (Composite Device)
	├── Camera1 (Simple Device)
	├── Camera2 (Simple Device)
	└── SensorHub (Composite Device)
	    ├── TemperatureSensor (Simple Device)
	    └── HumiditySensor (Simple Device)
	@endcode
	
	@see IDeviceInstance
	@see ICompositeDeviceSpecification
	@see IEditableCompositeDeviceInstance
	@see CCompositeDeviceInstanceBase
	@ingroup imtdev
*/
class ICompositeDeviceInstance: virtual public IDeviceInstance
{
public:
	/**
		@brief Get composite device specification
		
		Returns the composite device specification that describes static properties
		of this composite device type, including sub-device specifications.
		
		@return Pointer to composite device specification, or nullptr if not available
		@see ICompositeDeviceSpecification
	*/
	virtual const ICompositeDeviceSpecification* GetCompositeDeviceSpecification() const = 0;

	/**
		@brief Get supported sub-device type IDs
		
		Returns a set of device type identifiers that are supported as sub-devices
		within this composite device. Only devices of these types can be added as
		sub-devices.
		
		@return Set of supported sub-device type identifiers
		@see IDeviceSpecification::GetTypeId()
	*/
	virtual QSet<QByteArray> GetSupportedSubDeviceTypeIds() const = 0;

	/**
		@brief Get the list of available sub-devices
		
		Returns a collection containing information about all sub-devices currently
		available within this composite device. The collection supports iteration
		and provides change notifications when sub-devices are added or removed.
		
		@return Reference to collection info containing sub-device instances
		@see GetSubDeviceInstance()
		@see imtbase::ICollectionInfo
	*/
	virtual const imtbase::ICollectionInfo& GetSubDeviceList() const = 0;

	/**
		@brief Get device instance info of specific sub-device
		
		Retrieves the device instance information for a sub-device with the given
		identifier.
		
		@param subDeviceId Unique identifier of the sub-device within this composite
		@return Pointer to sub-device instance, or nullptr if not found
		@see GetSubDeviceList()
		@see IDeviceInstance
	*/
	virtual const IDeviceInstance* GetSubDeviceInstance(const QByteArray& subDeviceId) const = 0;
};


/**
	@brief Shared pointer to ICompositeDeviceInstance
	
	Smart pointer type for managing composite device instance lifetimes. Use this type
	when storing or passing composite device instances.
*/
typedef std::shared_ptr<ICompositeDeviceInstance> CompositeDeviceInstancePtr;


} // namespace imtdev


