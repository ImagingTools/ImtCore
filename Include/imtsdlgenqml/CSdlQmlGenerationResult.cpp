// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgenqml/CSdlQmlGenerationResult.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtsdlgenqml
{


// static helpers

/// \workaround ACF doens't support iterable containers serialization yet \todo Remove when supported
bool SerializeQSetAsQStringList(iser::IArchive& archive, QSet<QString>& container, const QByteArray& tagName, const QByteArray& tagDescription)
{
	if (archive.IsStoring()){
		QStringList list = container.values();

		return iser::CPrimitiveTypesSerializer::SerializeContainer<QStringList>(archive, list, tagName, tagDescription);
	}

	QStringList list;
	bool retVal = iser::CPrimitiveTypesSerializer::SerializeContainer<QStringList>(archive, list, tagName, tagDescription);
	container = QSet<QString>(list.begin(), list.end());
	
	return retVal;
}



// public methods


QDateTime CSdlQmlGenerationResult::GetCreatedAt() const
{
	return m_createdAt;
}


void CSdlQmlGenerationResult::SetCreatedAt(const QDateTime& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier changeNotifier(this);
		
		m_createdAt = createdAt;
	}
}


QString CSdlQmlGenerationResult::GetGeneratorVersion() const
{
	return m_generatorVersion;
}


void CSdlQmlGenerationResult::SetGeneratorVersion(const QString& version)
{
	if (m_generatorVersion != version){
		istd::CChangeNotifier changeNotifier(this);
		
		m_generatorVersion = version;
	}
}


QSet<QString> CSdlQmlGenerationResult::GetCreatedFolders() const
{
	return m_createdFolders;
}


void CSdlQmlGenerationResult::SetCreatedFolders(const QSet<QString>& folders)
{
	if (m_createdFolders != folders){
		istd::CChangeNotifier changeNotifier(this);
		
		m_createdFolders = folders;
	}
}


// reimplemented (istd::IChangeable)


int CSdlQmlGenerationResult::GetSupportedOperations() const
{
	return istd::IChangeable::SO_COPY |
			istd::IChangeable::SO_CLONE |
			istd::IChangeable::SO_COMPARE |
			istd::IChangeable::SO_RESET;
}


bool CSdlQmlGenerationResult::CopyFrom(const IChangeable& object, CompatibilityMode /* mode */)
{
	const auto* objectPtr = dynamic_cast<const CSdlQmlGenerationResult*>(&object);
	
	if (objectPtr != nullptr){
		istd::CChangeNotifier notifier(this);
		
		m_createdAt = objectPtr->m_createdAt;
		m_generatorVersion = objectPtr->m_generatorVersion;
		m_createdFolders = objectPtr->m_createdFolders;
		
		return true;
	}
	
	return false;
}


bool CSdlQmlGenerationResult::IsEqual(const IChangeable& object) const
{
	const auto* otherObjectPtr = dynamic_cast<const CSdlQmlGenerationResult*>(&object);
	if (otherObjectPtr != nullptr){
		return m_createdAt == otherObjectPtr->m_createdAt &&
				m_generatorVersion == otherObjectPtr->m_generatorVersion &&
				m_createdFolders == otherObjectPtr->m_createdFolders;
	}
	
	return false;
}


istd::IChangeableUniquePtr CSdlQmlGenerationResult::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CSdlQmlGenerationResult);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}
	
	return nullptr;
}


bool CSdlQmlGenerationResult::ResetData(CompatibilityMode /* mode */)
{
	istd::CChangeNotifier changeNotifier(this);
	
	m_createdAt = QDateTime();
	m_generatorVersion = QString();
	m_createdFolders.clear();
	
	return true;
}


// reimplemented (iser::ISerializable)


bool CSdlQmlGenerationResult::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	
	bool retVal = true;
	
	static iser::CArchiveTag createdAtTag("CreatedAt", "Creation timestamp", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);
	
	static iser::CArchiveTag generatorVersionTag("GeneratorVersion", "Generator version", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(generatorVersionTag);
	retVal = retVal && archive.Process(m_generatorVersion);
	retVal = retVal && archive.EndTag(generatorVersionTag);
	
	// Serialize QSet<QString> as QStringList for compatibility
	retVal = retVal && SerializeQSetAsQStringList(archive, m_createdFolders, "dirs", "Set of created folders");
	
	return retVal;
}


} // namespace imtsdlgenqml
