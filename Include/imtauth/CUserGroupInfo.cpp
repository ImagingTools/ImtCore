// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserGroupInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtauth/IUserGroupInfoProvider.h>


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
			imtauth::IUserGroupInfoSharedPtr parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr.IsValid()){
				for (const QByteArray& userId : parentGroupPtr->GetUsers()){
					if (!retVal.contains(userId)){
						retVal << userId;
					}
				}
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
	qsizetype removedCount = m_userIds.removeAll(userId);
	if (removedCount != 0){
		istd::CChangeNotifier changeNotifier(this);
	}

	return removedCount != 0;
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
	qsizetype removedCount = m_parentGroupIds.removeAll(parentGroupId);
	if (removedCount != 0){
		istd::CChangeNotifier changeNotifier(this);
	}

	return removedCount != 0;
}


const imtauth::IUserInfoProvider* CUserGroupInfo::GetUserProvider() const
{
	return m_userInfoProviderPtr;
}


// reimplemented (IUserBaseInfo)

imtauth::IUserBaseInfo::RoleIds CUserGroupInfo::GetRoles(const QByteArray& productId) const
{
	IUserBaseInfo::RoleIds retVal = BaseClass::GetRoles(productId);
	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_parentGroupIds){
			IUserGroupInfoSharedPtr parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr.IsValid()){
				for (const QByteArray& roleId : parentGroupPtr->GetRoles(productId)){
					if (!retVal.contains(roleId)){
						retVal << roleId;
					}
				}
			}
		}
	}

	return retVal;
}


IUserBaseInfo::FeatureIds CUserGroupInfo::GetPermissions(const QByteArray& productId) const
{
	IUserBaseInfo::FeatureIds allPermissions = BaseClass::GetPermissions(productId);

	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_parentGroupIds){
			IUserGroupInfoSharedPtr parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr.IsValid()){
				allPermissions += parentGroupPtr->GetPermissions(productId);
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


istd::IChangeableUniquePtr CUserGroupInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUserGroupInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
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
		IUserGroupInfoSharedPtr groupPtr(m_userGroupInfoProviderPtr->GetUserGroup(groupId));
		if (groupPtr.IsValid()){
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
			IUserGroupInfoSharedPtr groupPtr(m_userGroupInfoProviderPtr->GetUserGroup(groupId));
			if (groupPtr.IsValid()){
				GetParentGroupList(*groupPtr, groupList);
			}
		}
	}
}


} // namespace imtauth


