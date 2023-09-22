#include <imtauth/CUserSettings.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CClassInfo.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// static public methods

QByteArray CUserSettings::GetTypeName()
{
	return istd::CClassInfo::GetName<CUserSettings>();
}


// public methods

CUserSettings::CUserSettings()
{
}


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


iprm::IParamsSet* CUserSettings::GetSettings() const
{
	return m_settingsPtr.GetPtr();
}


void CUserSettings::SetSettings(const iprm::IParamsSet& settings)
{
	if (m_settingsPtr.IsValid()){
		istd::CChangeNotifier changeNotifier(this);

		m_settingsPtr->CopyFrom(settings);
	}
}


// reimplemented (iser::IObject)

QByteArray CUserSettings::GetFactoryId() const
{
	return GetTypeName();
}


// reimplemented (iser::ISerializable)

bool imtauth::CUserSettings::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag userTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userTag);

	iser::CArchiveTag settingsTag("Settings", "Settings params set", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(settingsTag);

	if (m_settingsPtr.IsValid()){
		retVal = retVal && m_settingsPtr->Serialize(archive);
	}

	retVal = retVal && archive.EndTag(settingsTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

int imtauth::CUserSettings::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET | SO_CLONE;
}


bool imtauth::CUserSettings::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CUserSettings* sourcePtr = dynamic_cast<const CUserSettings*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = sourcePtr->m_userId;

		if (!m_settingsPtr.IsValid() && sourcePtr->m_settingsPtr.IsValid()){
			m_settingsPtr.SetCastedOrRemove(sourcePtr->m_settingsPtr->CloneMe());

			if (!m_settingsPtr.IsValid()){
				return false;
			}

			return true;
		}
		else if (m_settingsPtr.IsValid() && !sourcePtr->m_settingsPtr.IsValid()){
			m_settingsPtr.Reset();

			return true;
		}
		else if (m_settingsPtr.IsValid() && sourcePtr->m_settingsPtr.IsValid()){
			return m_settingsPtr->CopyFrom(*sourcePtr->m_settingsPtr);
		}
	}

	return false;
}


istd::IChangeable* imtauth::CUserSettings::CloneMe(CompatibilityMode mode) const
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

	if (m_settingsPtr.IsValid()){
		m_settingsPtr->ResetData(mode);
	}

	return true;
}


} // namespace imtauth
