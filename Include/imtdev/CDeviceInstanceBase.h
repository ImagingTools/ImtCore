// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF include
#include <imod/CModelUpdateBridge.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtdev/IEditableDeviceInstance.h>
#include <imtdev/IDeviceSpecification.h>


namespace imtdev
{


/**
	@brief Base implementation class for device instances
	
	CDeviceInstanceBase provides a complete, reusable implementation of IEditableDeviceInstance
	that manages device identifiers, version information, and attributes. This class serves
	as the foundation for device instance objects throughout the imtdev library.
	
	@par Key Features:
	- **Identifier Management**: Storage for multiple identifier types (serial numbers, etc.)
	- **Version Tracking**: Comprehensive version information (firmware, hardware versions)
	- **Attributes Support**: Lazy-created attributes provider for dynamic properties
	- **Change Notifications**: Model update bridge for observer pattern support
	- **Serialization**: Support for copying and resetting device instance data
	
	@par Design:
	The class uses:
	- QMap for identifier storage (flexible, type-indexed)
	- TModelWrap for version info (automatic change tracking)
	- CModelUpdateBridge for propagating changes to observers
	- Lazy initialization for attributes (created only when needed)
	
	@par Usage Example:
	@code{.cpp}
	// Typically obtained from device controller
	DeviceInstancePtr pInstance = pController->GetDeviceInstance(deviceId);
	
	// For direct creation in derived classes or special scenarios:
	CDeviceInstanceBase deviceInstance;
	
	// Set device identifier
	deviceInstance.SetIdentifier(
		IDeviceInstance::IT_SERIAL, 
		QByteArray("SN12345"));
	
	// Set version information
	deviceInstance.SetVersion(
		IDeviceInstance::VI_FIRMWARE_VERSION_MAJOR, 
		2, 
		"Major version");
	deviceInstance.SetVersion(
		IDeviceInstance::VI_FIRMWARE_VERSION_MINOR, 
		1, 
		"Minor version");
	
	// Access attributes manager
	iattr::IAttributesManager* attrMgr = deviceInstance.GetAttributesManager();
	attrMgr->SetAttribute("Temperature", 25.5);
	
	// Read device information
	QByteArray serial = deviceInstance.GetIdentifier(IDeviceInstance::IT_SERIAL);
	const iser::IVersionInfo& version = deviceInstance.GetVersion();
	
	// Copy to another instance
	CDeviceInstanceBase anotherInstance;
	anotherInstance.CopyFrom(deviceInstance);
	@endcode
	
	@par Extension:
	Derived classes can override EnsureAttributesCreated() to customize attribute
	provider initialization.
	
	@see IEditableDeviceInstance
	@see IDeviceInstance
	@see CCompositeDeviceInstanceBase
	@ingroup imtdev
*/
class CDeviceInstanceBase : virtual public IEditableDeviceInstance
{
public:
	/**
		@brief Constructor
		
		Initializes the device instance with empty identifiers, version info,
		and no attributes (created lazily on first access).
	*/
	CDeviceInstanceBase();
	
	/**
		@brief Destructor
		
		Cleans up resources including attributes provider if it was created.
	*/
	~CDeviceInstanceBase();

	// reimplemented (imtdev::IEditableDeviceInstance)
	
	/**
		@brief Set device identifier
		
		Stores the identifier value for the specified identifier type.
		Triggers change notification if the value differs from current.
		
		@param idType Type of identifier (e.g., IT_SERIAL)
		@param id Identifier value
		@return true (always succeeds)
	*/
	virtual bool SetIdentifier(IdentifierTypes idType, const QByteArray& id) override;
	
	/**
		@brief Set version information component
		
		Sets a specific version component (e.g., firmware major version) with
		its value and description.
		
		@param versionId Version component identifier
		@param version Version number
		@param description Human-readable description
		@return true if set successfully
	*/
	virtual bool SetVersion(
		int versionId,
		quint32 version,
		const QString& description) override;
	
	/**
		@brief Get attributes manager for modification
		
		Returns the attributes manager, creating it if necessary (lazy initialization).
		
		@return Pointer to attributes manager
	*/
	virtual iattr::IAttributesManager* GetAttributesManager() override;

	// reimplemented (imtdev::IDeviceInstance)
	
	/**
		@brief Get device identifier
		
		Retrieves the identifier value for the specified type.
		
		@param idType Type of identifier to retrieve
		@return Identifier value, or empty array if not set
	*/
	virtual QByteArray GetIdentifier(int idType) const override;
	
	/**
		@brief Get version information
		
		Returns the complete version information object containing all
		version components.
		
		@return Reference to version info
	*/
	virtual const iser::IVersionInfo& GetVersion() const override;
	
	/**
		@brief Get attributes provider (read-only)
		
		Returns the attributes provider for reading attributes.
		
		@return Pointer to attributes provider, or nullptr if not created yet
	*/
	virtual const iattr::IAttributesProvider* GetAttributes() const override;

	// reimplemented (istd::IChangeable)
	
	/**
		@brief Get supported operations
		
		Returns flags indicating which operations are supported (copy, reset, etc.).
		
		@return Operation flags
	*/
	virtual int GetSupportedOperations() const override;
	
	/**
		@brief Copy data from another instance
		
		Copies identifiers, version info, and attributes from another device instance.
		
		@param object Source object to copy from
		@param mode Compatibility mode for copying
		@return true if copy succeeded
	*/
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	
	/**
		@brief Reset device instance data
		
		Clears all identifiers, version information, and attributes.
		
		@param mode Compatibility mode for reset
		@return true if reset succeeded
	*/
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	/**
		@brief Ensure attributes provider is created
		
		Creates the attributes provider if it doesn't exist yet (lazy initialization).
		Can be overridden by derived classes to customize attribute provider creation.
		
		@return true if attributes exist or were created successfully
	*/
	virtual bool EnsureAttributesCreated() const;

protected:
	/**
		@brief Internal version information implementation
		
		Nested class that implements iser::IVersionInfo for managing version
		components. Stores version numbers and descriptions in a map indexed
		by version ID.
	*/
	class VersionInfo : virtual public iser::IVersionInfo
	{
	public:
		/**
			@brief Set a version component
			
			@param versionId Version component identifier
			@param version Version number
			@param description Description text
			@return true if set successfully
		*/
		bool SetVersion(
			int versionId,
			quint32 version,
			const QString& description);

		// reimplemenbted (iser::IVersionInfo)
		virtual VersionIds GetVersionIds() const override;
		virtual bool GetVersionNumber(int versionId, quint32& result) const override;
		virtual QString GetVersionIdDescription(int versionId) const override;
		virtual QString GetEncodedVersionName(int versionId, quint32 versionNumber) const override;

		// reimplemented (istd::IChangeable)
		virtual int GetSupportedOperations() const override;
		virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
		virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
		virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	private:
		struct VersionItem
		{
			quint32 versionNumber;
			QString versionDescription;
		};

		QMap<int, VersionItem> m_versions;
	};

	/// Bridge for change notifications (mutable for change tracking in const methods)
	mutable imod::CModelUpdateBridge m_updateBridge;
	/// Lazy-created attributes (mutable to allow creation in const GetAttributes())
	mutable std::unique_ptr<iattr::IAttributesProvider> m_attributesPtr;

private:
	/// Storage for device identifiers by type
	QMap<int, QByteArray> m_identifiers;
	/// Version information with change tracking
	imod::TModelWrap<VersionInfo> m_versionInfo;
};


} // namespace imtdev
