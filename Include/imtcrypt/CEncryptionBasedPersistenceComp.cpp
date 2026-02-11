// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcrypt/CEncryptionBasedPersistenceComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>

// imt includes
#include <imtcrypt/CAesKey.h>
#include <imtcrypt/CAesEncryption.h>
#include <imtcrypt/CRsaEncryption.h>
#include <imtcrypt/CRsaKey.h>


namespace imtcrypt
{


// public methods

// reimplemented (imtcrypt::IEncryptedFilePersistence)

ifile::IFilePersistence::OperationState CEncryptionBasedPersistenceComp::LoadFromEncryptedFile(
			const QByteArray& key,
			const QString& filePath,
			istd::IChangeable& data) const
{
	Q_UNUSED(data);

	if (!filePath.isEmpty() &&
		m_encryptionKeysProviderCompPtr.IsValid() &&
		m_encryptionCompPtr.IsValid()){
		QFileInfo fileInfo(filePath);
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly)){
			QByteArray decryptedData;
			QByteArray encryptedData = file.readAll();

			LocalKeyProvider localKeyProvider;

			imtcrypt::IEncryption::EncryptionAlgorithm encryptionAlgorithm = imtcrypt::IEncryption::EA_AES;
			if (m_encryptionAlgorithm.IsValid()){
				switch (*m_encryptionAlgorithm){
				case imtcrypt::IEncryption::EA_RSA:
					encryptionAlgorithm = imtcrypt::IEncryption::EA_RSA;
					localKeyProvider = LocalKeyProvider(encryptionAlgorithm,
								m_encryptionKeysProviderCompPtr->GetEncryptionKey(IEncryptionKeysProvider::KT_PUBLIC),
								m_encryptionKeysProviderCompPtr->GetEncryptionKey(IEncryptionKeysProvider::KT_PRIVATE));
					break;
				default:
					localKeyProvider = LocalKeyProvider(encryptionAlgorithm,
						key,
						m_encryptionKeysProviderCompPtr->GetEncryptionKey(IEncryptionKeysProvider::KT_INIT_VECTOR));
					break;
				}
			}

			if (!m_encryptionCompPtr->DecryptData(encryptedData,encryptionAlgorithm, localKeyProvider, decryptedData)){
				file.close();
				return OS_FAILED;
			}

			file.close();
			// data.CopyFrom(decryptedData);
			return OS_OK;
		}
	}

	return OS_FAILED;
}


// reimplemented (ifile::IFilePersistence)

bool CEncryptionBasedPersistenceComp::IsOperationSupported(
			const istd::IChangeable *dataObjectPtr,
			const QString* /*filePathPtr*/,
			int /*flags*/,
			bool /*beQuiet*/) const
{
	return dataObjectPtr != nullptr;
}


ifile::IFilePersistence::OperationState CEncryptionBasedPersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* progressManagerPtr) const
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
				switch (*m_encryptionAlgorithm){
				case 0:
					encryptionAlgorithm = imtcrypt::IEncryption::EA_RSA;
					break;
				default:
					encryptionAlgorithm = imtcrypt::IEncryption::EA_AES;
					break;
				}
			}

			if (!m_encryptionCompPtr->DecryptData(encryptedData,encryptionAlgorithm, *m_encryptionKeysProviderCompPtr, decryptedData)){
				SendErrorMessage(0, QString("Encryption Persistence: Data decryption failed. File '%1' could not be loaded").arg(filePath));

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


ifile::IFilePersistence::OperationState CEncryptionBasedPersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString & filePath,
			ibase::IProgressManager* progressManagerPtr) const
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
						switch (*m_encryptionAlgorithm){
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


// methods of embedded class LocalKeyProvider

CEncryptionBasedPersistenceComp::LocalKeyProvider::LocalKeyProvider(
	const imtcrypt::IEncryption::EncryptionAlgorithm& algorithm,
	const QByteArray& keyFirst,
	const QByteArray& keySecond)
	:m_keyFirst(keyFirst)
	,m_keySecond(keySecond)
	, m_algorithm(algorithm)
{
}


// reimplemented (imtcrypt::IEncryptionKeysProvider)

QByteArray CEncryptionBasedPersistenceComp::LocalKeyProvider::GetEncryptionKey(KeyType type) const
{
	bool ok = IsRequestOk(type);
	if (ok){
		if (type == imtcrypt::IEncryptionKeysProvider::KT_PASSWORD || type == imtcrypt::IEncryptionKeysProvider::KT_PUBLIC)
			return m_keyFirst;
		if (type == imtcrypt::IEncryptionKeysProvider::KT_INIT_VECTOR || type == imtcrypt::IEncryptionKeysProvider::KT_PRIVATE)
			return m_keySecond;
	}

	return QByteArray();
}


bool CEncryptionBasedPersistenceComp::LocalKeyProvider::IsRequestOk(KeyType type) const
{
	if ((m_algorithm == imtcrypt::IEncryption::EA_AES) &&
		(type == imtcrypt::IEncryptionKeysProvider::KT_PASSWORD || type == IEncryptionKeysProvider::KT_INIT_VECTOR)){
		return true;
	}
	if ((m_algorithm == imtcrypt::IEncryption::EA_RSA) &&
		(type == imtcrypt::IEncryptionKeysProvider::KT_PRIVATE || type == IEncryptionKeysProvider::KT_PUBLIC)){
		return true;
	}

	return false;
}


} //namespace imtcrypt


