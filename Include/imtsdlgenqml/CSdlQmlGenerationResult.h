#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QSet>

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
		\brief Returns the creation timestamp.
		\returns Creation timestamp
	*/
	QDateTime GetCreatedAt() const;
	
	/**
		\brief Sets the creation timestamp.
		\param createdAt Creation timestamp to set
	*/
	void SetCreatedAt(const QDateTime& createdAt);
	
	/**
		\brief Returns the generator version.
		\returns Generator version string
	*/
	QString GetGeneratorVersion() const;
	
	/**
		\brief Sets the generator version.
		\param version Generator version to set
	*/
	void SetGeneratorVersion(const QString& version);
	
	/**
		\brief Returns the set of folders created during generation.
		\returns Set of folder paths
	*/
	QSet<QString> GetCreatedFolders() const;
	
	/**
		\brief Sets the set of folders created during generation.
		\param folders Set of folder paths to set
	*/
	void SetCreatedFolders(const QSet<QString>& folders);
	
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
	QSet<QString> m_createdFolders;
};


} // namespace imtsdlgenqml
