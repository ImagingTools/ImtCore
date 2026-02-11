// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdev/ICompositeDeviceInstance.h>


namespace imtdev
{


class IDeviceSpecification;


/**
	@brief Interface for modifying composite device instance information
	
	IEditableCompositeDeviceInstance extends ICompositeDeviceInstance to provide
	mutable access to the hierarchical device structure. It enables dynamic addition
	and removal of sub-devices within a composite device instance at runtime.
	
	This interface is essential for:
	- Building composite device structures programmatically
	- Dynamic device discovery and registration
	- Hot-pluggable sub-device management
	- User-configurable device hierarchies
	
	@par Dynamic Operations:
	- **Add Sub-Device**: Create new sub-device instances within the composite
	- **Remove Sub-Device**: Remove existing sub-devices from the composite
	- **Type Validation**: Ensure sub-devices match supported types
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain or create editable composite device instance
	CCompositeDeviceInstanceBase* pComposite = // your composite instance
	
	// Add a camera sub-device
	QByteArray cameraTypeId = "StandardCamera";
	QByteArray cameraId = pComposite->AddSubDevice(
		cameraTypeId,                    // Device type ID
		"Front Camera",                  // Name
		"High resolution front camera",  // Description
		nullptr,                         // Default value (optional)
		"CAMERA_001"                     // Proposed ID (optional)
	);
	
	if (!cameraId.isEmpty())
	{
		// Sub-device successfully added
		const IDeviceInstance* pCamera =
			pComposite->GetSubDeviceInstance(cameraId);
	}
	
	// Add a sensor hub sub-device
	QByteArray sensorHubTypeId = "SensorHub";
	QByteArray sensorHubId = pComposite->AddSubDevice(
		sensorHubTypeId,
		"Environmental Sensors",
		"Temperature and humidity sensors"
	);
	
	// Remove a sub-device
	if (pComposite->RemoveSubDevice(cameraId))
	{
		// Camera sub-device successfully removed
	}
	
	// Verify sub-device list after modifications
	const imtbase::ICollectionInfo& subDevices = 
		pComposite->GetSubDeviceList();
	int subDeviceCount = subDevices.GetCount();
	@endcode
	
	@par Type Safety:
	Sub-devices can only be added if their type ID is in the set returned by
	GetSupportedSubDeviceTypeIds(). Attempting to add unsupported types will fail.
	
	@par Change Notifications:
	Adding or removing sub-devices triggers change notifications through the
	istd::IChangeable interface, allowing observers to react to structural changes.
	
	@see ICompositeDeviceInstance
	@see IDeviceSpecification
	@see CCompositeDeviceInstanceBase
	@ingroup imtdev
*/
class IEditableCompositeDeviceInstance : virtual public ICompositeDeviceInstance
{
public:
	/**
		@brief Add new sub-device to the composite
		
		Creates and adds a new sub-device instance with the specified type and
		properties. The device type must be in the list of supported sub-device
		types for this composite.
		
		@param deviceTypeId Type identifier of the sub-device to create
		@param name Human-readable name for the sub-device
		@param description Optional description of the sub-device
		@param defaultValuePtr Optional default configuration for the sub-device
		@param proposedId Optional proposed identifier for the sub-device; 
		                  if empty, an ID will be generated automatically
		@return Unique identifier of the newly created sub-device, or empty array on failure
		@see RemoveSubDevice()
		@see ICompositeDeviceInstance::GetSupportedSubDeviceTypeIds()
	*/
	virtual QByteArray AddSubDevice(
				const QByteArray& deviceTypeId,
				const QString& name,
				const QString& description = QString(),
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedId = QByteArray()) = 0;
	/**
		@brief Remove sub-device from the composite
		
		Removes an existing sub-device from the composite device structure.
		
		@param subDeviceId Unique identifier of the sub-device to remove
		@return true if sub-device was removed successfully, false otherwise
		@see AddSubDevice()
	*/
	virtual bool RemoveSubDevice(const QByteArray& subDeviceId) = 0;
};


/**
	@brief Shared pointer to IEditableCompositeDeviceInstance
	
	Smart pointer type for managing editable composite device instance lifetimes.
	Use this type when creating and configuring composite device structures.
*/
typedef std::shared_ptr<IEditableCompositeDeviceInstance> EditableCompositeDeviceInstancePtr;


} // namespace imtdev


