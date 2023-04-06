#include <imtauth/CUserGroupInfo.h>


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (IUserGroupInfo)

QString CUserGroupInfo::GetDescription() const
{
	return m_description;
}


void CUserGroupInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


IUserBaseInfo::RoleIds CUserGroupInfo::GetUsers() const
{
	return m_userIds;
}


void CUserGroupInfo::SetUsers(const UserIds& users)
{
	if (m_userIds != users){
		istd::CChangeNotifier changeNotifier(this);

		m_userIds = users;
	}
}


const imtauth::IUserInfoProvider* CUserGroupInfo::GetUserProvider() const
{
	return nullptr;
}


bool CUserGroupInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	bool retVal = true;

	BaseClass::Serialize(archive);

	static iser::CArchiveTag usernameTag("Description", "Description of the group", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(usernameTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(usernameTag);

	QByteArray usersTag = "Users";
	QByteArray userTag = "User";
	QByteArrayList userPermissions = m_userIds.values();
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, userPermissions, usersTag, userTag);

	return retVal;
}


bool CUserGroupInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserGroupInfo* sourcePtr = dynamic_cast<const CUserGroupInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass::CopyFrom(object);

		m_description = sourcePtr->m_description;
		m_userIds = sourcePtr->m_userIds;

		return true;
	}

	return false;
}


istd::IChangeable *CUserGroupInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUserGroupInfo> clonePtr(new CUserGroupInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUserGroupInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	BaseClass::ResetData(mode);

	m_description.clear();
	m_userIds.clear();

	return true;
}


} // namespace imtauth


