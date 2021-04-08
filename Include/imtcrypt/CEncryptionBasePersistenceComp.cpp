#include <imtcrypt/CEncryptionBasePersistenceComp.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>

// ImtCore includes
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <imtcrypt/CRsaEncryption.h>
#include <imtcrypt/CAesEncryption.h>
#include <imtcrypt/IEncryptionKeysProvider.h>


namespace imtcrypt
{


// reimplemented (IEncryptionBasePersistence)

bool CEncryptionBasePersistenceComp::IsOperationSupported(const istd::IChangeable *dataObjectPtr, const QString* /*filePathPtr*/, int /*flags*/, bool /*beQuiet*/) const
{
	if (dataObjectPtr != nullptr)
		return true;
	return false;
}


int CEncryptionBasePersistenceComp::LoadFromFile(istd::IChangeable& /*data*/, const QString& /*filePath*/, ibase::IProgressManager* /*progressManagerPtr*/) const
{
	return OS_OK;
}


int CEncryptionBasePersistenceComp::SaveToFile(const istd::IChangeable &data, const QString &filePath, ibase::IProgressManager *progressManagerPtr) const
{
	if (m_basePersistenceCompPtr.IsValid() && !filePath.isEmpty()){
		QFileInfo fileInfo(filePath);
		QTemporaryDir tempDir;
		QString filePathTmp = tempDir.path() + "/" + fileInfo.baseName() + ".xml";

		if (m_basePersistenceCompPtr->SaveToFile(data, filePathTmp, progressManagerPtr) == OS_OK){
			if (m_encryptionCompPtr.IsValid()){
				QByteArray encryptedData;
				QFile file(filePathTmp);
				QByteArray data;
				if (file.open(QIODevice::ReadOnly)){
					data = file.readAll();
				}
				file.close();
				file.setFileName(filePath);
				if (!data.isEmpty() && m_encryptionKeysProviderCompPtr.IsValid() && file.open(QIODevice::WriteOnly)){
					m_encryptionCompPtr->EncryptData(data,imtcrypt::IEncryption::EA_AES,*m_encryptionKeysProviderCompPtr,encryptedData);
					file.write(encryptedData);
				}
				file.close();
				return OS_OK;
			}

		}
	}

	return OS_FAILED;
}


// reimplemented (ifile::IFileTypeInfo)

bool CEncryptionBasePersistenceComp::GetFileExtensions(QStringList &result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	int extensionsCount = qMin(m_fileExtensionsAttrPtr.GetCount(), m_typeDescriptionsAttrPtr.GetCount());
	for (int extIndex = 0; extIndex < extensionsCount; extIndex++){
		result.push_back(m_fileExtensionsAttrPtr[extIndex]);
	}

	return true;
}


QString CEncryptionBasePersistenceComp::GetTypeDescription(const QString *extensionPtr) const
{
	if (extensionPtr != NULL){
		int extensionsCount = qMin(m_fileExtensionsAttrPtr.GetCount(), m_typeDescriptionsAttrPtr.GetCount());
		for (int extIndex = 0; extIndex < extensionsCount; extIndex++){
			if (m_fileExtensionsAttrPtr[extIndex] == *extensionPtr){
				return m_typeDescriptionsAttrPtr[extIndex];
			}
		}
	}

	if (m_typeDescriptionsAttrPtr.GetCount() > 0){
		return m_typeDescriptionsAttrPtr[0];
	}

	return "";
}


} //namespace imtcrypt

