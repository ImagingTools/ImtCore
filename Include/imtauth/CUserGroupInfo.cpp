#include <imtauth/CUserGroupInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods


CUserGroupInfo::CUserGroupInfo():
	m_userInfoProviderPtr(nullptr)
{

}


// reimplemented (IUserGroupInfo)

IUserGroupInfo::UserIds CUserGroupInfo::GetUsers() const
{
	QByteArrayList retVal = m_userIds;

	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_parentGroupIds){
			const imtauth::IUserGroupInfo* parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr != nullptr){
				retVal += parentGroupPtr->GetUsers();
			}
		}
	}

	return retVal;
}


void CUserGroupInfo::SetUsers(const UserIds& users)
{
	if (m_userIds != users){
		istd::CChangeNotifier changeNotifier(this);

		m_userIds = users;
	}
}


void CUserGroupInfo::AddUser(const QByteArray& userId)
{
	if (!m_userIds.contains(userId)){
		istd::CChangeNotifier changeNotifier(this);

		m_userIds << userId;
	}
}


bool CUserGroupInfo::RemoveUser(const QByteArray& userId)
{
	bool result = m_userIds.removeAll(userId);
	if (result){
		istd::CChangeNotifier changeNotifier(this);
	}

	return result;
}


imtauth::IUserGroupInfo::GroupIds CUserGroupInfo::GetParentGroups() const
{
	return m_parentGroupIds;
}


bool CUserGroupInfo::AddParentGroup(const QByteArray& parentGroupId)
{
	if (m_id == parentGroupId){
		return false;
	}

	QByteArrayList parentGroups = GetParentGroups(parentGroupId);

	if (parentGroups.contains(m_id)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_parentGroupIds.append(parentGroupId);

	return true;
}


bool CUserGroupInfo::RemoveParentGroup(const QByteArray& parentGroupId)
{
	bool result = m_parentGroupIds.removeAll(parentGroupId);
	if (result){
		istd::CChangeNotifier changeNotifier(this);
	}

	return result;
}


const imtauth::IUserInfoProvider* CUserGroupInfo::GetUserProvider() const
{
	return m_userInfoProviderPtr;
}


// reimplemented (IUserBaseInfo)

imtauth::IUserBaseInfo::RoleIds CUserGroupInfo::GetRoles() const
{
	QByteArrayList retVal = m_roles;

	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_parentGroupIds){
			const imtauth::IUserGroupInfo* parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr != nullptr){
				retVal += parentGroupPtr->GetRoles();
			}
		}
	}

	return retVal;
}


IUserBaseInfo::FeatureIds CUserGroupInfo::GetPermissions() const
{
	IUserBaseInfo::FeatureIds allPermissions = BaseClass::GetPermissions();

	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_parentGroupIds){
			const imtauth::IUserGroupInfo* parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr != nullptr){
				allPermissions += parentGroupPtr->GetPermissions();
			}
		}
	}

	return allPermissions;
}


bool CUserGroupInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_userIds, "Users", "User");
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_parentGroupIds, "ParentGroups", "ParentGroup");

	return retVal;
}


bool CUserGroupInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserGroupInfo* sourcePtr = dynamic_cast<const CUserGroupInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass::CopyFrom(object);

		m_userIds = sourcePtr->m_userIds;
		m_parentGroupIds = sourcePtr->m_parentGroupIds;

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

	m_userIds.clear();
	m_parentGroupIds.clear();
	m_userGroupInfoProviderPtr = nullptr;
	m_userInfoProviderPtr = nullptr;

	return true;
}


// protected methods

QByteArrayList CUserGroupInfo::GetParentGroups(const QByteArray& groupId) const
{
	QByteArrayList retVal;
	if (m_userGroupInfoProviderPtr != nullptr){
		const istd::TDelPtr<IUserGroupInfo> groupPtr(const_cast<IUserGroupInfo*>(m_userGroupInfoProviderPtr->GetUserGroup(groupId)));
		if (groupPtr != nullptr){
			GetParentGroupList(*groupPtr, retVal);
		}
	}

	return retVal;
}


void CUserGroupInfo::GetParentGroupList(const IUserGroupInfo& userGroupInfo, QByteArrayList& groupList) const
{
	QByteArrayList parentGroups = userGroupInfo.GetParentGroups();

	groupList += parentGroups;
	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& groupId : parentGroups){
			const istd::TDelPtr<IUserGroupInfo> groupPtr(const_cast<IUserGroupInfo*>(m_userGroupInfoProviderPtr->GetUserGroup(groupId)));
			if (groupPtr != nullptr){
				GetParentGroupList(*groupPtr, groupList);
			}
		}
	}
}


} // namespace imtauth


