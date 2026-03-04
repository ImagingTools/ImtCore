// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdev/CDeviceDataFilePersistenceComp.h>


// Qt includes
#include <QtCore/QCoreApplication>

// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtdev/IDeviceDataProvider.h>


namespace imtdev
{


// public methods

// reimplemented (ifile::IFilePersistence)

bool CDeviceDataFilePersistenceComp::IsOperationSupported(
			const istd::IChangeable* /*dataObjectPtr*/,
			const QString* /*filePathPtr*/,
			int /*flags*/,
			bool /*beQuiet*/) const
{
	return m_deviceDataPersistenceCompPtr.IsValid();
}


ifile::IFilePersistence::OperationState CDeviceDataFilePersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& /*filePath*/,
			ibase::IProgressManager* progressManagerPtr) const
{
	if (!m_deviceDataPersistenceCompPtr.IsValid()){
		SendCriticalMessage(0, "Device data storage persistence was not set. Check the component diagram!");

		return OS_FAILED;
	}

	imtdev::IDeviceDataProvider* dataProviderPtr = dynamic_cast<imtdev::IDeviceDataProvider*>(&data);
	if (dataProviderPtr == nullptr){
		SendCriticalMessage(0, QT_TR_NOOP("Wrong data object type"));

		return OS_FAILED;
	}

	iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(&data);
	if (serializablePtr == nullptr){
		SendCriticalMessage(0, QT_TR_NOOP("Wrong data object type. Serialization is not supported"));

		return OS_FAILED;
	}

	QByteArray deviceDataBuffer;
	bool retVal = m_deviceDataPersistenceCompPtr->ReadDataFromStorage(deviceDataBuffer, imtdev::IDeviceDataPersistence::ST_EEPROM, progressManagerPtr);
	if (!retVal){
		SendErrorMessage(0, QT_TR_NOOP("Reading of the device data failed"));

		return OS_FAILED;
	}

	if (!deviceDataBuffer.isEmpty()){
		iser::CMemoryReadArchive dataArchive(deviceDataBuffer.constData(),  imtbase::narrow_cast<int>(deviceDataBuffer.size()));

		retVal = serializablePtr->Serialize(dataArchive);
		if (!retVal){
			SendErrorMessage(0, QT_TR_NOOP("Reading of the device data during deserialization failed"));

			return OS_FAILED;
		}
	}

	idoc::IDocumentMetaInfo* metaInfoPtr = const_cast<idoc::IDocumentMetaInfo*>(dataProviderPtr->GetDeviceMetaInfo());
	if (metaInfoPtr != nullptr){
		retVal = retVal && m_deviceDataPersistenceCompPtr->ReadDeviceMetaInfo(*metaInfoPtr);
		if (!retVal){
			SendErrorMessage(0, QT_TR_NOOP("Reading of the device meta-data failed"));

			return OS_FAILED;
		}
	}

	return retVal ? OS_OK : OS_FAILED;
}


ifile::IFilePersistence::OperationState CDeviceDataFilePersistenceComp::SaveToFile(const istd::IChangeable& data, const QString& /*filePath*/, ibase::IProgressManager* progressManagerPtr) const
{
	if (!m_deviceDataPersistenceCompPtr.IsValid()){
		return OS_FAILED;
	}

	iser::ISerializable* serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&data));
	if (serializablePtr == nullptr){
		return OS_FAILED;
	}

	iser::CMemoryWriteArchive archive;
	bool retVal = serializablePtr->Serialize(archive);
	if (retVal){
		retVal = retVal && m_deviceDataPersistenceCompPtr->ResetStorage(imtdev::IDeviceDataPersistence::ST_EEPROM);
		retVal = retVal && m_deviceDataPersistenceCompPtr->WriteDataToStorage(
					QByteArray((const char*)archive.GetBuffer(),
					archive.GetBufferSize()),
					imtdev::IDeviceDataPersistence::ST_EEPROM,
					progressManagerPtr);
	}

	return retVal ? OS_OK : OS_FAILED;
}


// reimplemented (ifile::IFileTypeInfo)

bool CDeviceDataFilePersistenceComp::GetFileExtensions(QStringList& /*result*/, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool /*doAppend*/) const
{
	return false;
}


QString CDeviceDataFilePersistenceComp::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return QString();
}


} // namespace imtdev


