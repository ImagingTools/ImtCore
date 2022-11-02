#include <imtauth/CUserSettings.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (iser::IUserSettings)

QByteArray imtauth::CUserSettings::GetUserId() const
{
	return m_userId;
}


void imtauth::CUserSettings::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QByteArray imtauth::CUserSettings::GetSettings() const
{
	return m_settingsJson;
}


void imtauth::CUserSettings::SetSettings(const QByteArray& jsonData)
{
	if (m_settingsJson != jsonData){
		istd::CChangeNotifier changeNotifier(this);

		m_settingsJson = jsonData;
	}
}


// reimplemented (iser::ISerializable)

bool imtauth::CUserSettings::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	bool retVal = true;

	static iser::CArchiveTag userTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userTag);

	static iser::CArchiveTag settingsTag("Settings", "Settings params set", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(settingsTag);
	retVal = retVal && archive.Process(m_settingsJson);
	retVal = retVal && archive.EndTag(settingsTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

int imtauth::CUserSettings::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}


bool imtauth::CUserSettings::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserSettings* sourcePtr = dynamic_cast<const CUserSettings*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = sourcePtr->m_userId;
		m_settingsJson = sourcePtr->m_settingsJson;

		return true;
	}

	return false;
}


istd::IChangeable *imtauth::CUserSettings::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUserSettings> clonePtr(new CUserSettings);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool imtauth::CUserSettings::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_userId.clear();
	m_settingsJson.clear();

	return true;
}


} // namespace imtauth


