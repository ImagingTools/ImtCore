#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtcrypt/IEncryption.h>
#include <imtcrypt/IEncryptionKeysProvider.h>


namespace imtcrypt
{


/**
	Component for saving an encrypted object using imtcrypt::IEncryption and imtcrypt::IEncryptionKeysProvider interfaces
	and underlaying data object persistence (see attribute \c BasePersistence).
	\ingroup Cryptography
*/
class CEncryptionBasedPersistenceComp: public ilog::CLoggerComponentBase, virtual public ifile::IFilePersistence
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEncryptionBasedPersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_ASSIGN(m_encryptionCompPtr, "Encryption", "Encrypt/Decrypt instances", true, "Encryption");
		I_ASSIGN(m_encryptionKeysProviderCompPtr, "EncryptionKeysProvider", "Keys provider instances", true, "EncryptionKeysProvider");
		I_ASSIGN(m_basePersistenceCompPtr, "BasePersistence", "Base file export instances", true, "BasePersistence");
		I_ASSIGN(m_encryptionAlgorithm, "EncryptionAlgorithm", "Algoritm for encrypt/decrypt (0 - RSA, 1 - AES)", true, 1);
		I_ASSIGN_MULTI_1(m_fileExtensionsAttrPtr, "FileExtensions", "List of possible file extensions", true, "txt");
		I_ASSIGN_MULTI_1(m_typeDescriptionsAttrPtr, "TypeDescriptions", "List of descriptions for each extension", true, "Text file");
	I_END_COMPONENT;

protected:
	// reimplemented (ifile::IFilePersistence)
	virtual bool IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr = NULL,
			int flags = -1,
			bool beQuiet = true) const override;
	virtual int LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath = QString(),
			ibase::IProgressManager* progressManagerPtr = NULL) const override;
	virtual int SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath = QString(),
			ibase::IProgressManager* progressManagerPtr = NULL) const override;

	// reimplemented (ifile::IFileTypeInfo)
	virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = NULL, int flags = -1, bool doAppend = false) const;
	virtual QString GetTypeDescription(const QString* extensionPtr = NULL) const;

private:
	I_REF(imtcrypt::IEncryption, m_encryptionCompPtr);
	I_REF(imtcrypt::IEncryptionKeysProvider, m_encryptionKeysProviderCompPtr);
	I_REF(ifile::IFilePersistence, m_basePersistenceCompPtr);
	I_ATTR(int, m_encryptionAlgorithm);
	I_MULTIATTR(QString, m_fileExtensionsAttrPtr);
	I_MULTITEXTATTR(m_typeDescriptionsAttrPtr);
};


} //namespace imtcrypt


