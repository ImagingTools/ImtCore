// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF include
#include <imod/CModelUpdateBridge.h>
#include <imod/TModelWrap.h>
#include <istd/TComposedFactory.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtdev/IEditableCompositeDeviceInstance.h>
#include <imtdev/CDeviceInstanceBase.h>


namespace imtdev
{


/**
	@brief Base implementation class for composite device instances
	
	CCompositeDeviceInstanceBase extends CDeviceInstanceBase to support hierarchical
	device structures with sub-devices. It provides complete implementation of
	IEditableCompositeDeviceInstance, enabling dynamic creation and management of
	device trees.
	
	This class implements the **Composite Pattern**, allowing devices to contain other
	devices in a parent-child relationship. Sub-devices can themselves be composite,
	enabling multi-level hierarchies.
	
	@par Key Features:
	- **Sub-Device Management**: Add and remove sub-devices dynamically
	- **Factory Pattern**: Virtual CreateDeviceInstance() for custom sub-device creation
	- **Collection Support**: ICollectionInfo interface for sub-device list access
	- **Change Tracking**: Automatic notifications when sub-devices are added/removed
	- **Serialization**: Copy and reset operations including sub-device tree
	
	@par Design:
	The class uses:
	- QMap for sub-device storage (indexed by device ID)
	- TModelWrap for collection info (automatic change tracking)
	- Virtual factory method for extensible device instance creation
	- Nested SubDeviceCollectionInfo for ICollectionInfo implementation
	
	@par Usage Example:
	@code{.cpp}
	// Create composite device instance
	CCompositeDeviceInstanceBase compositeDevice;
	
	// Set parent device information
	compositeDevice.SetIdentifier(IDeviceInstance::IT_SERIAL, "COMPOSITE_001");
	compositeDevice.SetVersion(IDeviceInstance::VI_FIRMWARE_VERSION_MAJOR, 1, "v1");
	
	// Add sub-devices
	QByteArray cameraId = compositeDevice.AddSubDevice(
		"StandardCamera",           // Device type ID
		"Camera 1",                 // Name
		"Front-facing camera"       // Description
	);
	
	QByteArray sensorId = compositeDevice.AddSubDevice(
		"TemperatureSensor",
		"Temp Sensor",
		"Environmental sensor"
	);
	
	// Access sub-device list
	const imtbase::ICollectionInfo& subDevices = compositeDevice.GetSubDeviceList();
	int count = subDevices.GetElementsCount();
	
	// Get specific sub-device
	const IDeviceInstance* pCamera = compositeDevice.GetSubDeviceInstance(cameraId);
	if (pCamera)
	{
		// Work with sub-device
		const IDeviceSpecification& spec = pCamera->GetDeviceSpecification();
	}
	
	// Remove sub-device
	compositeDevice.RemoveSubDevice(sensorId);
	
	// Copy entire device tree
	CCompositeDeviceInstanceBase clonedDevice;
	clonedDevice.CopyFrom(compositeDevice);
	@endcode
	
	@par Extension:
	Derived classes should override CreateDeviceInstance() to provide custom
	sub-device instance creation based on device type ID. The base implementation
	returns nullptr.
	
	@par Thread Safety:
	Operations that modify the sub-device structure trigger change notifications.
	Ensure proper synchronization if accessing from multiple threads.
	
	@see CDeviceInstanceBase
	@see IEditableCompositeDeviceInstance
	@see ICompositeDeviceInstance
	@ingroup imtdev
*/
class CCompositeDeviceInstanceBase : public CDeviceInstanceBase, virtual public IEditableCompositeDeviceInstance
{
public:
	/// Base class type alias
	typedef CDeviceInstanceBase BaseClass;

	/**
		@brief Constructor
		
		Initializes the composite device instance with empty sub-device list.
		Inherits base device instance initialization from CDeviceInstanceBase.
	*/
	CCompositeDeviceInstanceBase();
	
	/**
		@brief Destructor
		
		Cleans up all sub-devices and releases resources.
	*/
	~CCompositeDeviceInstanceBase();

	// reimplemented (IEditableCompositeDeviceInstance)
	
	/**
		@brief Add new sub-device
		
		Creates and adds a new sub-device instance with the specified type and properties.
		The device type must be supported by the composite device specification.
		Calls virtual CreateDeviceInstance() to create the instance, then stores it.
		
		@param deviceTypeId Type identifier of the sub-device to create
		@param name Human-readable name for the sub-device
		@param description Optional description
		@param defaultValuePtr Optional default configuration (unused in base implementation)
		@param proposedId Optional proposed ID; if empty, auto-generated
		@return Unique identifier of the newly created sub-device, or empty on failure
	*/
	virtual QByteArray AddSubDevice(
		const QByteArray& deviceTypeId,
		const QString& name,
		const QString& description = QString(),
		const istd::IChangeable* defaultValuePtr = nullptr,
		const QByteArray& proposedId = QByteArray()) override;
	
	/**
		@brief Remove sub-device
		
		Removes an existing sub-device from the composite. Triggers change notification.
		
		@param id Unique identifier of the sub-device to remove
		@return true if sub-device was removed successfully, false if not found
	*/
	virtual bool RemoveSubDevice(const QByteArray& id) override;

	// reimplemented (ICompositeDeviceInstance)
	
	/**
		@brief Get composite device specification
		
		Base implementation returns nullptr. Derived classes should override to provide
		actual composite device specification.
		
		@return Pointer to composite specification, or nullptr in base implementation
	*/
	virtual const ICompositeDeviceSpecification* GetCompositeDeviceSpecification() const override;
	
	/**
		@brief Get supported sub-device type IDs
		
		Returns the set of device type IDs that can be added as sub-devices.
		Base implementation queries the composite device specification.
		
		@return Set of supported type IDs
	*/
	virtual QSet<QByteArray> GetSupportedSubDeviceTypeIds() const override;
	
	/**
		@brief Get sub-device list
		
		Returns the collection containing all sub-devices in this composite.
		
		@return Reference to sub-device collection info
	*/
	virtual const imtbase::ICollectionInfo& GetSubDeviceList() const override;
	
	/**
		@brief Get specific sub-device instance
		
		Retrieves the device instance for a sub-device with the given ID.
		
		@param subDeviceId Unique identifier of the sub-device
		@return Pointer to sub-device instance, or nullptr if not found
	*/
	virtual const IDeviceInstance* GetSubDeviceInstance(const QByteArray& subDeviceId) const override;

	// reimplemented (istd::IChangeable)
	
	/**
		@brief Get supported operations
		
		Returns flags indicating supported operations (copy, reset, etc.).
		Includes base class operations plus composite-specific operations.
		
		@return Operation flags
	*/
	virtual int GetSupportedOperations() const override;
	
	/**
		@brief Copy from another composite device
		
		Copies all data including sub-devices from another composite device instance.
		Performs deep copy of the entire device tree.
		
		@param object Source object to copy from
		@param mode Compatibility mode for copying
		@return true if copy succeeded
	*/
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	
	/**
		@brief Reset composite device data
		
		Clears all data including removing all sub-devices.
		
		@param mode Compatibility mode for reset
		@return true if reset succeeded
	*/
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	/**
		@brief Create device instance (factory method)
		
		Virtual factory method for creating sub-device instances based on device type ID.
		Derived classes must override this to provide actual device instance creation.
		
		@param deviceTypeId Type identifier of the device to create
		@return Pointer to newly created device instance, or nullptr if type not supported
		@note Base implementation returns nullptr
	*/
	virtual IDeviceInstance* CreateDeviceInstance(const QByteArray& deviceTypeId) const;

private:
	/**
		@brief Internal collection info implementation for sub-devices
		
		Nested class that implements imtbase::ICollectionInfo interface for
		accessing the sub-device list. Provides read-only and limited write
		operations on the sub-device collection.
	*/
	class SubDeviceCollectionInfo : virtual public imtbase::ICollectionInfo
	{
	public:
		/**
			@brief Set parent composite device
			
			@param parentPtr Pointer to parent composite device instance
		*/
		void SetParent(CCompositeDeviceInstanceBase* parentPtr);

		// reimpolemented (imtbase::ICollectionInfo)
		virtual int GetElementsCount(
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual Ids GetElementIds(
			int offset = 0,
			int count = -1,
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual bool GetSubsetInfo(
			ICollectionInfo& subsetInfo,
			int offset = 0,
			int count = -1,
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual QVariant GetElementInfo(
					const Id& elementId,
					int infoType,
					ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual idoc::MetaInfoPtr GetElementMetaInfo(
					const Id& elementId,
					ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual bool SetElementName(
					const Id& elementId,
					const QString& name,
					ilog::IMessageConsumer* logPtr = nullptr) override;
		virtual bool SetElementDescription(
					const Id& elementId,
					const QString& description,
					ilog::IMessageConsumer* logPtr = nullptr) override;
		virtual bool SetElementEnabled(
					const Id& elementId,
					bool isEnabled = true,
					ilog::IMessageConsumer* logPtr = nullptr) override;

	private:
		/// Pointer to parent composite device
		CCompositeDeviceInstanceBase* m_parentPtr;
	};

private:
	/**
		@brief Remove all sub-devices
		
		Internal helper to clear all sub-devices from the composite.
	*/
	void RemoveAllSubDevices();

private:
	/**
		@brief Sub-device item structure
		
		Stores information about a single sub-device including its name,
		description, and instance pointer.
	*/
	struct SubDeviceItem
	{
		/// Human-readable name
		QString name;
		/// Description text
		QString description;
		/// Device instance pointer
		DeviceInstancePtr instancePtr;
	};

	/// Map of sub-devices by ID
	QMap<QByteArray, SubDeviceItem> m_subDeviceMap;
	/// Collection with change tracking
	imod::TModelWrap<SubDeviceCollectionInfo> m_subDeviceCollection;
};


} // namespace imtdev


