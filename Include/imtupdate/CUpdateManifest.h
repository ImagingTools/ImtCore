// ACF includes
#include<iser/ISerializable.h>


namespace imtupdate
{


/**
	The CManifest class, provides an information about the product updates
 */
class CUpdateManifest: virtual public iser::ISerializable
{

public:
	QString GetVersion() const;
	QString GetModuleType() const;
	/**
		Returns the link to download the file.
		Unlike the \c GetDownloadPath, it specifies the full link with the server address, port and schema
		\note It may be empty but in this case \c GetDownloadLink must be specified
	*/
	QString GetDownloadLink() const;
	/**
		Returns the path to download the file.
		Unlike the \c GetDownloadLink, it does NOT specifies the full link with the server address, port and schema
		\note It may be empty but in this case \c GetDownloadLink must be specified
	*/
	QString GetDownloadPath() const;
	QString GetHash() const;
	QString GetHashType() const;

	// reimplemented (istd::IChangeble)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	QString m_ver;
	QString m_moduleType;
	QString m_downloadLink;
	QString m_downloadPath;
	QString m_hash;
	QString m_hashType;
};


}// namespace imtupdate

