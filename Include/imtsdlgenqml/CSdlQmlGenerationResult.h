// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	\brief Provides information about SDL QML code generation results.
	
	\details This class stores metadata about the code generation process including:
	- Creation timestamp
	- Generator version
	- Set of folders created during generation
 */
class CSdlQmlGenerationResult: virtual public iser::ISerializable
{
public:
	/**
		\brief Returns the creation timestamp.
		\returns Creation timestamp
	*/
	[[nodiscard]] QDateTime GetCreatedAt() const;
	
	/**
		\brief Sets the creation timestamp.
		\param createdAt Creation timestamp to set
	*/
	void SetCreatedAt(const QDateTime& createdAt);
	
	/**
		\brief Returns the generator version.
		\returns Generator version string
	*/
	[[nodiscard]] QString GetGeneratorVersion() const;
	
	/**
		\brief Sets the generator version.
		\param version Generator version to set
	*/
	void SetGeneratorVersion(const QString& version);
	
	/**
		\brief Returns the set of folders created during generation.
		\returns Set of folder paths
	*/
	[[nodiscard]] QSet<QString> GetCreatedFolders() const;
	
	/**
		\brief Sets the set of folders created during generation.
		\param folders Set of folder paths to set
	*/
	void SetCreatedFolders(const QSet<QString>& folders);
	
	// reimplemented (istd::IChangeable)
	int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	bool IsEqual(const IChangeable& object) const override;
	istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
	
	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

private:
	QDateTime m_createdAt;
	QString m_generatorVersion;
	QSet<QString> m_createdFolders;
};


} // namespace imtsdlgenqml
