#include <imtcrypt/CEncryptionBasedPersistenceComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>


namespace imtcrypt
{


// protected methods

// reimplemented (IEncryptionBasePersistence)

bool CEncryptionBasedPersistenceComp::IsOperationSupported(const istd::IChangeable *dataObjectPtr, const QString* /*filePathPtr*/, int /*flags*/, bool /*beQuiet*/) const
{
	if (dataObjectPtr != nullptr){
		return true;
	}

	return false;
}


int CEncryptionBasedPersistenceComp::LoadFromFile(istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* progressManagerPtr) const
{
	if (		m_basePersistenceCompPtr.IsValid() &&
				!filePath.isEmpty() &&
				m_encryptionKeysProviderCompPtr.IsValid() &&
				m_encryptionCompPtr.IsValid()){
		QFileInfo fileInfo(filePath);
		QTemporaryDir tempDir;
		QString filePathTmp = tempDir.path() + "/" + fileInfo.baseName() + ".xml";
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly)){
			QByteArray decryptedData;
			QByteArray encryptedData = file.readAll();
			imtcrypt::IEncryption::EncryptionAlgorithm encryptionAlgorithm = imtcrypt::IEncryption::EA_AES;
			if (m_encryptionAlgorithm.IsValid()){
				switch (*m_encryptionAlgorithm) {
				case 0:
					encryptionAlgorithm = imtcrypt::IEncryption::EA_RSA;
					break;
				default:
					encryptionAlgorithm = imtcrypt::IEncryption::EA_AES;
					break;
				}
			}

			if (!m_encryptionCompPtr->DecryptData(encryptedData,encryptionAlgorithm,*m_encryptionKeysProviderCompPtr,decryptedData)){
				SendErrorMessage(0, QString("Data decryption was failed. File '%1' could not be loaded").arg(filePath), "Encryption Persistence");

				file.close();

				return OS_FAILED;
			}

			file.close();
			file.setFileName(filePathTmp);
			if (file.open(QIODevice::WriteOnly)){
				file.write(decryptedData);
				file.close();

				return m_basePersistenceCompPtr->LoadFromFile(data, filePathTmp, progressManagerPtr);
			}
		}
	}

	return OS_FAILED;
}


int CEncryptionBasedPersistenceComp::SaveToFile(const istd::IChangeable& data, const QString & filePath, ibase::IProgressManager* progressManagerPtr) const
{
	if (m_basePersistenceCompPtr.IsValid() && !filePath.isEmpty()){
		QFileInfo fileInfo(filePath);
		QTemporaryDir tempDir;
		QString filePathTmp = tempDir.path() + "/" + fileInfo.baseName() + ".xml";

		if (m_basePersistenceCompPtr->SaveToFile(data, filePathTmp, progressManagerPtr) == OS_OK){
			if (m_encryptionCompPtr.IsValid()){
				QByteArray encryptedData;
				QFile file(filePathTmp);
				QByteArray xmlData;
				if (file.open(QIODevice::ReadOnly)){
					xmlData = file.readAll();
				}

				file.close();
				file.setFileName(filePath);

				if (!xmlData.isEmpty() && m_encryptionKeysProviderCompPtr.IsValid() && file.open(QIODevice::WriteOnly)){
					imtcrypt::IEncryption::EncryptionAlgorithm encryptionAlgorithm = imtcrypt::IEncryption::EA_AES;
					if (m_encryptionAlgorithm.IsValid()){
						switch (*m_encryptionAlgorithm) {
						case 0:
							encryptionAlgorithm = imtcrypt::IEncryption::EA_RSA;
							break;
						default:
							encryptionAlgorithm = imtcrypt::IEncryption::EA_AES;
							break;
						}
					}
					m_encryptionCompPtr->EncryptData(xmlData, encryptionAlgorithm, *m_encryptionKeysProviderCompPtr, encryptedData);

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

bool CEncryptionBasedPersistenceComp::GetFileExtensions(QStringList &result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
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


QString CEncryptionBasedPersistenceComp::GetTypeDescription(const QString *extensionPtr) const
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

	return QString();
}


} //namespace imtcrypt


