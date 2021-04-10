#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <imtcrypt/IEncryption.h>
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <icomp/CComponentBase.h>

namespace imtcrypt
{


class IEncryptionKeysProvider;


/**
	Component describing saving an encrypted object using external components imtcrypt::IEncryption and imtcrypt::IEncryptionKeysProvider
	\ingroup Cryptography
*/
class CEncryptionBasedPersistenceComp: public icomp::CComponentBase, virtual public ifile::IFilePersistence
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEncryptionBasedPersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_ASSIGN(m_encryptionCompPtr, "Encryption", "Encrypt/Decrypt instances", true, "Encryption");
		I_ASSIGN(m_encryptionKeysProviderCompPtr, "EncryptionKeysProvider", "Keys provider instances", true, "EncryptionKeysProvider");
		I_ASSIGN(m_basePersistenceCompPtr, "BasePersistence", "Base file export instances", true, "BasePersistence");
		I_ASSIGN_MULTI_1(m_fileExtensionsAttrPtr, "FileExtensions", "List of possible file extensions", false, "txt");
		I_ASSIGN_MULTI_1(m_typeDescriptionsAttrPtr, "TypeDescriptions", "List of descriptions for each extension", false, "Text file");
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
	I_MULTIATTR(QString, m_fileExtensionsAttrPtr);
	I_MULTITEXTATTR(m_typeDescriptionsAttrPtr);


};


} //namespace imtcrypt


