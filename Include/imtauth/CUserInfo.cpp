#include <imtauth/CUserInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

QByteArray CUserInfo::GetPasswordHash() const
{
	return m_passwordHash;
}


void CUserInfo::SetPasswordHash(const QByteArray& passwordHash)
{
	if (m_passwordHash != passwordHash){
		istd::CChangeNotifier changeNotifier(this);

		m_passwordHash = passwordHash;
	}
}


QString CUserInfo::GetMail() const
{
	return m_mail;
}


void CUserInfo::SetMail(const QString& mail)
{
	if (m_mail != mail){
		istd::CChangeNotifier changeNotifier(this);

		m_mail = mail;
	}
}


bool CUserInfo::IsAdmin() const
{
	return false;
}


IUserGroupInfo::GroupIds CUserInfo::GetGroups() const
{
	return m_groupIds;
}


void CUserInfo::AddToGroup(const QByteArray& groupId)
{
	if (!m_groupIds.contains(groupId)){
		istd::CChangeNotifier changeNotifier(this);

		m_groupIds << groupId;
	}
}


bool CUserInfo::RemoveFromGroup(const QByteArray& groupId)
{
	bool result = m_groupIds.removeAll(groupId);
	if (result){
		istd::CChangeNotifier changeNotifier(this);
	}

	return result;
}


IUserBaseInfo::RoleIds CUserInfo::GetRoles() const
{
	QByteArrayList retVal = m_roles;

	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_groupIds){
			const imtauth::IUserGroupInfo* parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr != nullptr){
				QByteArrayList groupRoleIds = parentGroupPtr->GetRoles();
				for (const QByteArray& roleId : groupRoleIds){
					if (!retVal.contains(roleId)){
						retVal << roleId;
					}
				}
			}
		}
	}

	return retVal;
}


IUserBaseInfo::FeatureIds CUserInfo::GetPermissions() const
{
	IUserBaseInfo::FeatureIds allPermissions = BaseClass::GetPermissions();

	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_groupIds){
			const imtauth::IUserGroupInfo* parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr != nullptr){
				allPermissions += parentGroupPtr->GetPermissions();
			}
		}
	}

	return allPermissions;
}


bool CUserInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	static iser::CArchiveTag mailTag("Mail", "Mail of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mailTag);
	retVal = retVal && archive.Process(m_mail);
	retVal = retVal && archive.EndTag(mailTag);

	static iser::CArchiveTag passwordTag("PasswordHash", "Password hash", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(passwordTag);
	retVal = retVal && archive.Process(m_passwordHash);
	retVal = retVal && archive.EndTag(passwordTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_groupIds, "Groups", "Group");

	return retVal;
}


bool CUserInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserInfo* sourcePtr = dynamic_cast<const CUserInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass::CopyFrom(object);

		m_passwordHash = sourcePtr->m_passwordHash;
		m_mail = sourcePtr->m_mail;
		m_groupIds = sourcePtr->m_groupIds;

		return true;
	}

	return false;
}


istd::IChangeable *CUserInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUserInfo> clonePtr(new CUserInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUserInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	BaseClass::ResetData(mode);

	m_mail.clear();
	m_passwordHash.clear();
	m_groupIds.clear();

	return true;
}


} // namespace imtauth


