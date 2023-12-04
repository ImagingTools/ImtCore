#include <imtupdate/CUpdateManifest.h>

//Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QDebug>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtupdate
{

// public methods


QString CUpdateManifest::GetVersion() const
{
	return m_ver;
}


QString CUpdateManifest::GetModuleType() const
{
	return m_moduleType;
}


QString CUpdateManifest::GetDownloadLink() const
{
	return m_downloadLink;
}


QString CUpdateManifest::GetDownloadPath() const
{
	return m_downloadPath;
}


QString CUpdateManifest::GetHash() const
{
	return m_hash;
}


QString CUpdateManifest::GetHashType() const
{
	return m_hashType;
}


// reimplemented (istd::IChangeble)

int CUpdateManifest::GetSupportedOperations() const
{
	return istd::IChangeable::SO_COPY |
			istd::IChangeable::SO_CLONE |
			istd::IChangeable::SO_COMPARE |
			istd::IChangeable::SO_RESET;
}


bool CUpdateManifest::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CUpdateManifest* objectPtr = dynamic_cast<const CUpdateManifest*>(&object);

	if (objectPtr != NULL){
		istd::CChangeNotifier notifier(this);

		m_ver = objectPtr->m_ver;
		m_moduleType = objectPtr->m_moduleType;
		m_downloadLink = objectPtr->m_downloadLink;
		m_downloadPath = objectPtr->m_downloadPath;
		m_hash = objectPtr->m_hash;
		m_hashType = objectPtr->m_hashType;

		return true;
	}

	return false;
}


bool CUpdateManifest::IsEqual(const IChangeable& object) const
{
	const CUpdateManifest* otherObjectPtr = dynamic_cast<const CUpdateManifest*>(&object);
	if (otherObjectPtr != nullptr){
		return m_ver == otherObjectPtr->m_ver &&
			   m_moduleType == otherObjectPtr->m_moduleType &&
				m_downloadLink == otherObjectPtr->m_downloadLink &&
				m_downloadPath == otherObjectPtr->m_downloadPath &&
				m_hash == otherObjectPtr->m_hash &&
				m_hashType == otherObjectPtr->m_hashType;
	}

	return false;
}


istd::IChangeable* CUpdateManifest::CloneMe(CompatibilityMode /*mode*/) const
{
	CUpdateManifest* clonedPtr = new CUpdateManifest();

	clonedPtr->m_downloadLink = m_downloadLink;
	clonedPtr->m_downloadLink = m_downloadPath;
	clonedPtr->m_moduleType = m_moduleType;
	clonedPtr->m_ver = m_ver;
	clonedPtr->m_hash = m_hash;
	clonedPtr->m_hashType = m_hashType;

	return clonedPtr;
}


bool CUpdateManifest::ResetData(istd::IChangeable::CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_ver = QString();
	m_moduleType = QString();
	m_downloadLink = QString();
	m_downloadPath = QString();
	m_hash = QString();
	m_hashType = QString();

	return true;
}


// reimplemented (iser::ISerialisable)

bool CUpdateManifest::Serialize(iser::IArchive& archive)
{

	istd::CChangeNotifier notifier(archive.IsStoring() ? NULL : this);
	Q_UNUSED(notifier);

	bool retVal = true;

	/// \bug 1st must be a group if serializing withuot an ACF tag
	static iser::CArchiveTag verTag("VER", "App version", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(verTag);
	retVal = retVal && archive.Process(m_ver);
	retVal = retVal && archive.EndTag(verTag);

	static iser::CArchiveTag moduleTag("MODULE", "Module type app or lib", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(moduleTag);
	retVal = retVal && archive.Process(m_moduleType);
	retVal = retVal && archive.EndTag(moduleTag);

	static iser::CArchiveTag linkTag("LINK", "Link to download version", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(linkTag);
	retVal = retVal && archive.Process(m_downloadLink);
	retVal = retVal && archive.EndTag(linkTag);

	static iser::CArchiveTag pathTag("Path", "Path to download version", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(linkTag);
	retVal = retVal && archive.Process(m_downloadPath);
	retVal = retVal && archive.EndTag(linkTag);

	static iser::CArchiveTag hashTag("HASH", "Hash of the file", iser::CArchiveTag::TT_MULTIPLE);
	retVal = retVal && archive.BeginTag(hashTag);

	static iser::CArchiveTag hashValueTag("Value", "Hash value", iser::CArchiveTag::TT_LEAF, &hashTag);
	retVal = retVal && archive.BeginTag(hashValueTag);
	retVal = retVal && archive.Process(m_hash);
	retVal = retVal && archive.EndTag(hashValueTag);

	static iser::CArchiveTag hashTypeTag("Type", "Hash type", iser::CArchiveTag::TT_LEAF, &hashTag);
	retVal = retVal && archive.BeginTag(hashTypeTag);
	retVal = retVal && archive.Process(m_hashType);
	retVal = retVal && archive.EndTag(hashTypeTag);

	retVal = retVal && archive.EndTag(hashTag);

	return retVal;
}


} // namespace imtupdate

