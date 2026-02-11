// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace ibase
{
	class IProgressManager;
}


namespace imtdev
{


/**
	@brief Interface for device data persistence
	
	IDeviceDataPersistence provides low-level access to device storage (ROM, EEPROM) for
	reading and writing data, as well as managing device metadata. This interface enables
	firmware updates, configuration storage, and metadata retrieval from device hardware.
	
	The interface supports:
	- Reading/writing device storage (ROM for read-only, EEPROM for read-write)
	- Metadata access (serial numbers, version numbers, firmware versions)
	- Bulk data operations with progress tracking
	- Storage reset and cleanup operations
	
	@par Storage Types:
	- **ST_ROM**: Read-only memory (typically firmware, factory data)
	- **ST_EEPROM**: Electrically erasable programmable ROM (user configuration, calibration)
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain persistence interface reference (via component system)
	IDeviceDataPersistence* pPersistence = // get from component system
	
	// Check if device is ready
	if (!pPersistence->IsDeviceReady())
	{
		// Device not ready for operations
		return;
	}
	
	// Read device metadata
	idoc::IDocumentMetaInfo* pMetaInfo = // create meta info object
	if (pPersistence->ReadDeviceMetaInfo(*pMetaInfo))
	{
		QString serialNum = pMetaInfo->GetMetaInfo(
			IDeviceDataPersistence::MIT_SERIAL_NUMBER);
		QString fwVersion = pMetaInfo->GetMetaInfo(
			IDeviceDataPersistence::MIT_FIRMWARE_VERSION);
	}
	
	// Read all data from EEPROM
	QByteArray eepromData;
	ibase::IProgressManager* pProgress = // your progress manager
	if (pPersistence->ReadDataFromStorage(eepromData, 
	                                       IDeviceDataPersistence::ST_EEPROM, 
	                                       pProgress))
	{
		// Process EEPROM data
	}
	
	// Write data to EEPROM
	QByteArray newData = // prepare configuration data
	pPersistence->WriteDataToStorage(newData, 
	                                 IDeviceDataPersistence::ST_EEPROM,
	                                 pProgress);
	
	// Reset EEPROM to factory defaults
	pPersistence->ResetStorage(IDeviceDataPersistence::ST_EEPROM);
	@endcode
	
	@par Thread Safety:
	Operations should be synchronized as they typically involve hardware communication.
	Progress tracking enables long-running operations to report status without blocking.
	
	@see CDeviceDataFilePersistenceComp
	@see IDeviceDataProvider
	@ingroup imtdev
*/
class IDeviceDataPersistence: virtual public istd::IPolymorphic
{
public:
	/**
		@brief Device storage types
		
		Enumeration of storage types available on devices.
	*/
	enum StorageType
	{
		/// Read-only memory (firmware, factory data)
		ST_ROM,
		/// Electrically erasable programmable ROM (user configuration)
		ST_EEPROM
	};

	/**
		@brief Device metadata information types
		
		Enumeration of metadata fields that can be read from devices.
		Extends idoc::IDocumentMetaInfo::MIT_USER for device-specific metadata.
	*/
	enum MetaInfo
	{
		/// Device serial number
		MIT_SERIAL_NUMBER = idoc::IDocumentMetaInfo::MIT_USER,
		/// Device version number
		MIT_VERSION_NUMBER,
		/// Firmware version string
		MIT_FIRMWARE_VERSION
	};

	/**
		@brief Read metadata from the device
		
		Retrieves device metadata including serial number, version, and firmware version.
		
		@param metaInfo Document metadata object to populate with device information
		@return true if metadata was read successfully, false otherwise
		@see MetaInfo
		@see idoc::IDocumentMetaInfo
	*/
	virtual bool ReadDeviceMetaInfo(idoc::IDocumentMetaInfo& metaInfo) const = 0;

	/**
		@brief Read all data from device storage
		
		Reads the complete contents of the specified storage type (ROM or EEPROM).
		Supports progress tracking for long-running operations.
		
		@param data Output buffer to receive storage data
		@param storageType Type of storage to read from (ST_ROM or ST_EEPROM)
		@param progressPtr Optional progress manager for tracking read progress (can be nullptr)
		@return true if data was read successfully, false otherwise
		@see StorageType
		@see WriteDataToStorage()
	*/
	virtual bool ReadDataFromStorage(QByteArray& data, StorageType storageType, ibase::IProgressManager* progressPtr = nullptr) const = 0;

	/**
		@brief Write all data to device storage
		
		Writes the complete data buffer to the specified storage type. Only writable
		storage types (ST_EEPROM) support this operation.
		
		@param data Data buffer to write to storage
		@param storageType Type of storage to write to (typically ST_EEPROM)
		@param progressPtr Optional progress manager for tracking write progress (can be nullptr)
		@return true if data was written successfully, false otherwise
		@see StorageType
		@see ReadDataFromStorage()
		@note ST_ROM is read-only and cannot be written via this interface
	*/
	virtual bool WriteDataToStorage(const QByteArray& data, StorageType storageType, ibase::IProgressManager* progressPtr = nullptr) = 0;

	/**
		@brief Delete all data from device storage
		
		Erases all data from the specified storage. This operation is only supported
		for writable storage types (ST_EEPROM).
		
		@param storageType Type of storage to clear
		@return true if storage was cleared successfully, false otherwise
		@see ResetStorage()
		@note Only supported for writable storage types
	*/
	virtual bool RemoveAllData(StorageType storageType) = 0;

	/**
		@brief Reset storage to initial/factory state
		
		Resets the storage to its initial state, typically restoring factory defaults.
		This operation is only supported for writable storage types (ST_EEPROM).
		
		@param storageType Type of storage to reset
		@return true if storage was reset successfully, false otherwise
		@see RemoveAllData()
		@note Only supported for writable storage types
	*/
	virtual bool ResetStorage(StorageType storageType) = 0;

	/**
		@brief Check if device is ready for operations
		
		Verifies that the device is in a state where storage operations can be performed.
		
		@return true if device is ready, false otherwise
		@note Always check this before performing storage operations
	*/
	virtual bool IsDeviceReady() const = 0;
};


} // namespace imtdev


