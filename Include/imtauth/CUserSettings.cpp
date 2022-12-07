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


iprm::IParamsSet *CUserSettings::GetSettings() const
{
	return m_settingsPtr;
}


void CUserSettings::SetSettings(const iprm::IParamsSet *paramsSet)
{
	if (m_settingsPtr != paramsSet){
		istd::CChangeNotifier changeNotifier(this);

		m_settingsPtr = const_cast<iprm::IParamsSet*>(paramsSet);
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
	retVal = retVal && m_settingsPtr->Serialize(archive);
//	retVal = retVal && archive.Process(m_settingsJson);
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
		m_settingsPtr = sourcePtr->m_settingsPtr;

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
	m_settingsPtr = nullptr;

	return true;
}


} // namespace imtauth
