#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QStringList>

// ACF includes
#include <iser/ISerializable.h>


namespace imtsdlgenqml
{


/**
	The CSdlQmlGenerationResult class provides information about SDL QML code generation results.
	
	This class stores metadata about the code generation process including:
	- Creation timestamp
	- Generator version
	- List of folders created during generation
 */
class CSdlQmlGenerationResult: virtual public iser::ISerializable
{
public:
	/**
		Get the creation timestamp.
		\return Creation timestamp
	*/
	QDateTime GetCreatedAt() const;
	
	/**
		Set the creation timestamp.
		\param createdAt Creation timestamp to set
	*/
	void SetCreatedAt(const QDateTime& createdAt);
	
	/**
		Get the generator version.
		\return Generator version string
	*/
	QString GetGeneratorVersion() const;
	
	/**
		Set the generator version.
		\param version Generator version to set
	*/
	void SetGeneratorVersion(const QString& version);
	
	/**
		Get the list of folders created during generation.
		\return List of folder paths
	*/
	QStringList GetCreatedFolders() const;
	
	/**
		Set the list of folders created during generation.
		\param folders List of folder paths to set
	*/
	void SetCreatedFolders(const QStringList& folders);
	
	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
	
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	QDateTime m_createdAt;
	QString m_generatorVersion;
	QStringList m_createdFolders;
};


} // namespace imtsdlgenqml
