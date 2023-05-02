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
	Interface for the data persistence on a storage in the device.
*/
class IDeviceDataPersistence: virtual public istd::IPolymorphic
{
public:
	enum StorageType
	{
		ST_ROM,
		ST_EEPROM
	};

	enum MetaInfo
	{
		MIT_SERIAL_NUMBER = idoc::IDocumentMetaInfo::MIT_USER,
		MIT_VERSION_NUMBER,
		MIT_FIRMWARE_VERSION
	};

	/**
		Read meta-info for the device.
	*/
	virtual bool ReadDeviceMetaInfo(idoc::IDocumentMetaInfo& metaInfo) const = 0;

	/**
		Read all data at once from the device storage of the given type.
	*/
	virtual bool ReadDataFromStorage(QByteArray& data, StorageType storageType, ibase::IProgressManager* progressPtr = nullptr) const = 0;

	/**
		Write all data at once to the device storage of the given type.
	*/
	virtual bool WriteDataToStorage(const QByteArray& data, StorageType storageType, ibase::IProgressManager* progressPtr = nullptr) = 0;

	/**
		Delete all data from device storage.
		This operation is supported only for storages with write-access.
	*/
	virtual bool RemoveAllData(StorageType storageType) = 0;

	/**
		Reset the storage to the initial state.
		This operation is supported only for storages with write-access.
	*/
	virtual bool ResetStorage(StorageType storageType) = 0;

	/**
		Returns true if device is ready.
	*/
	virtual bool IsDeviceReady() const = 0;
};


} // namespace imtdev


