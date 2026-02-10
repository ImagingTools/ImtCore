// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iattr/IAttributesManager.h>

// ImtCore includes
#include <imtdev/IDeviceInstance.h>


namespace imtdev
{


/**
	@brief Interface for modifying device instance information
	
	IEditableDeviceInstance extends IDeviceInstance to provide mutable access to
	device instance properties. While IDeviceInstance provides read-only access,
	this interface allows modification of identifiers, version information, and
	attributes.
	
	This interface is essential for:
	- Initial device instance creation and configuration
	- Updating device metadata after discovery
	- Synchronizing instance data with physical device
	- Managing dynamic attributes during runtime
	
	@par Mutable Properties:
	- **Identifiers**: Set device identifiers like serial numbers
	- **Version Information**: Update firmware and hardware versions
	- **Attributes**: Modify device-specific attributes via attributes manager
	
	@par Usage Example:
	@code{.cpp}
	// Typically obtained from device controller or factory
	// For implementation reference:
	CDeviceInstanceBase* pEditableInstance = // your instance
	
	// Or obtained from controller with editable interface:
	// EditableDeviceInstancePtr pEditableInstance = ...;
	
	// Set device identifier (e.g., serial number)
	QByteArray serialNumber = "SN123456789";
	pEditableInstance->SetIdentifier(
		IDeviceInstance::IT_SERIAL, 
		serialNumber);
	
	// Set firmware version
	pEditableInstance->SetVersion(
		IDeviceInstance::VI_FIRMWARE_VERSION_MAJOR,
		2,
		"Major firmware version");
	pEditableInstance->SetVersion(
		IDeviceInstance::VI_FIRMWARE_VERSION_MINOR,
		1,
		"Minor firmware version");
	pEditableInstance->SetVersion(
		IDeviceInstance::VI_FIRMWARE_VERSION_PATCH,
		0,
		"Patch firmware version");
	
	// Set hardware version
	pEditableInstance->SetVersion(
		IDeviceInstance::VI_HARDWARE_VERSION_MAJOR,
		1,
		"Hardware revision");
	
	// Access attributes manager to set device-specific attributes
	iattr::IAttributesManager* pAttrMgr = 
		pEditableInstance->GetAttributesManager();
	if (pAttrMgr)
	{
		pAttrMgr->SetAttribute("DeviceTemperature", QVariant(25.5));
		pAttrMgr->SetAttribute("CalibrationDate", QVariant("2024-01-15"));
	}
	
	// Use as regular device instance (read-only access)
	DeviceInstancePtr pInstance = pEditableInstance;
	QByteArray id = pInstance->GetIdentifier(IDeviceInstance::IT_SERIAL);
	@endcode
	
	@par Implementation Notes:
	Implementations typically trigger change notifications when properties are
	modified, allowing observers to react to device instance updates.
	
	@see IDeviceInstance
	@see CDeviceInstanceBase
	@see IEditableCompositeDeviceInstance
	@ingroup imtdev
*/
class IEditableDeviceInstance : virtual public IDeviceInstance
{
public:
	/**
		@brief Set device identifier
		
		Sets the device identifier of the specified type (e.g., serial number).
		
		@param idType Type of identifier to set (e.g., IT_SERIAL)
		@param id Identifier value as a byte array
		@return true if identifier was set successfully, false otherwise
		@see IDeviceInstance::GetIdentifier()
		@see IDeviceInstance::IdentifierTypes
	*/
	virtual bool SetIdentifier(IdentifierTypes idType, const QByteArray& id) = 0;

	/**
		@brief Set device version information
		
		Sets a version component (firmware or hardware version number) with
		an optional description.
		
		@param versionId Version component identifier (e.g., VI_FIRMWARE_VERSION_MAJOR)
		@param version Version number value
		@param description Human-readable version description
		@return true if version was set successfully, false otherwise
		@see IDeviceInstance::GetVersion()
		@see IDeviceInstance::VersionIds
	*/
	virtual bool SetVersion(
		int versionId,
		quint32 version,
		const QString& description) = 0;

	/**
		@brief Get device instance attributes manager
		
		Returns the attributes manager that provides mutable access to device
		instance attributes. Unlike GetAttributes() which returns read-only
		provider, this returns the manager interface for modifications.
		
		@return Pointer to attributes manager
		@see IDeviceInstance::GetAttributes()
		@see iattr::IAttributesManager
	*/
	virtual iattr::IAttributesManager* GetAttributesManager() = 0;
};


/**
	@brief Shared pointer to IEditableDeviceInstance
	
	Smart pointer type for managing editable device instance lifetimes. Use this type
	when creating and configuring device instances.
*/
typedef std::shared_ptr<IEditableDeviceInstance> EditableDeviceInstancePtr;


} // namespace imtdev


