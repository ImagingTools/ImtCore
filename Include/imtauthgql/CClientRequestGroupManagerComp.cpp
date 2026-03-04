// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestGroupManagerComp.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IUserGroupManager)

QByteArrayList CClientRequestGroupManagerComp::GetGroupIds() const
{
	return GetElementIds(QByteArrayLiteral("Groups"));
}


QByteArray CClientRequestGroupManagerComp::CreateGroup(const QString& groupName, const QString& description)
{
	namespace groupssdl = sdl::imtauth::Groups;

	groupssdl::GroupAddRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	arguments.input.Version_1_0->typeId = QByteArrayLiteral("Group");
	arguments.input.Version_1_0->name = groupName;
	arguments.input.Version_1_0->description = description;

	QByteArray productId;
	if (m_applicationInfoCompPtr.IsValid()){
		productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}
	arguments.input.Version_1_0->productId = productId;

	groupssdl::CGroupData::V1_0 groupData;
	groupData.description = description;
	groupData.name = groupName;
	groupData.productId = productId;

	arguments.input.Version_1_0->item = groupData;

	sdl::imtbase::ImtCollection::CAddedNotificationPayload payload;
	bool ok = SendModelRequestInternal<groupssdl::GroupAddRequestArguments, sdl::imtbase::ImtCollection::CAddedNotificationPayload, groupssdl::CGroupAddGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArray();
	}

	if (!payload.Version_1_0->id.HasValue()){
		return QByteArray();
	}

	return *payload.Version_1_0->id;
}


bool CClientRequestGroupManagerComp::RemoveGroup(const QByteArray& groupId)
{
	return RemoveElements(QByteArrayLiteral("Groups"), {groupId});
}


imtauth::IUserGroupInfoUniquePtr CClientRequestGroupManagerComp::GetGroup(const QByteArray& groupId) const
{
	if (!m_userGroupFactoryCompPtr.IsValid()){
		return nullptr;
	}

	sdl::imtauth::Groups::CGroupData::V1_0 groupData;
	bool ok = GetGroupDataSdl(groupId, groupData);
	if (!ok){
		return nullptr;
	}

	imtauth::IUserGroupInfoUniquePtr groupInfoPtr = m_userGroupFactoryCompPtr.CreateInstance();
	if (!groupInfoPtr.IsValid()){
		return nullptr;
	}

	QByteArray productId;
	if (m_applicationInfoCompPtr.IsValid()){
		productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	if (productId.isEmpty()){
		return nullptr;
	}

	if (groupData.name){
		groupInfoPtr->SetName(*groupData.name);
	}

	if (groupData.description){
		groupInfoPtr->SetDescription(*groupData.description);
	}

	if (groupData.roles){
		groupInfoPtr->SetRoles(productId, groupData.roles->ToList());
	}

	if (groupData.users){
		groupInfoPtr->SetUsers(groupData.users->ToList());
	}

	return groupInfoPtr;
}


bool CClientRequestGroupManagerComp::AddUsersToGroup(const QByteArray& groupId, const QByteArrayList& userIds)
{
	if (userIds.isEmpty()){
		return false;
	}

	sdl::imtauth::Groups::CGroupData::V1_0 groupData;
	bool ok = GetGroupDataSdl(groupId, groupData);
	if (!ok){
		return false;
	}

	if (!groupData.users.HasValue()){
		return false;
	}

	QByteArrayList currentUserIds = groupData.users->ToList();

	QByteArrayList result = currentUserIds;
	for (const QByteArray& userId : userIds){
		if (!result.contains(userId)){
			result << userId;
		}
	}

	groupData.users->FromList(result);

	return SetGroupDataSdl(groupId, groupData);
}


bool CClientRequestGroupManagerComp::RemoveUsersFromGroup(const QByteArray& groupId, const QByteArrayList& userIds)
{
	if (userIds.isEmpty()){
		return false;
	}

	sdl::imtauth::Groups::CGroupData::V1_0 groupData;
	bool ok = GetGroupDataSdl(groupId, groupData);
	if (!ok){
		return false;
	}

	if (!groupData.users.HasValue()){
		return false;
	}

	QByteArrayList currentUserIds = groupData.users->ToList();

	QByteArrayList result = currentUserIds;
	for (const QByteArray& userId : userIds){
		if (result.contains(userId)){
			result.removeAll(userId);
		}
	}

	groupData.users->FromList(result);

	return SetGroupDataSdl(groupId, groupData);
}


bool CClientRequestGroupManagerComp::AddRolesToGroup(const QByteArray& groupId, const QByteArrayList& roleIds)
{
	if (roleIds.isEmpty()){
		return false;
	}

	sdl::imtauth::Groups::CGroupData::V1_0 groupData;
	bool ok = GetGroupDataSdl(groupId, groupData);
	if (!ok){
		return false;
	}

	if (!groupData.users.HasValue()){
		return false;
	}

	QByteArrayList currentRoleIds = groupData.roles->ToList();

	QByteArrayList result = currentRoleIds;
	for (const QByteArray& roleId : roleIds){
		if (!result.contains(roleId)){
			result << roleId;
		}
	}

	groupData.roles->FromList(result);

	return SetGroupDataSdl(groupId, groupData);
}


bool CClientRequestGroupManagerComp::RemoveRolesFromGroup(const QByteArray& groupId, const QByteArrayList& roleIds)
{
	if (roleIds.isEmpty()){
		return false;
	}

	sdl::imtauth::Groups::CGroupData::V1_0 groupData;
	bool ok = GetGroupDataSdl(groupId, groupData);
	if (!ok){
		return false;
	}

	if (!groupData.users.HasValue()){
		return false;
	}

	QByteArrayList currentRoleIds = groupData.roles->ToList();

	QByteArrayList result = currentRoleIds;
	for (const QByteArray& roleId : roleIds){
		if (result.contains(roleId)){
			result.removeAll(roleId);
		}
	}

	groupData.roles->FromList(result);

	return SetGroupDataSdl(groupId, groupData);
}


// private methods

bool CClientRequestGroupManagerComp::GetGroupDataSdl(const QByteArray& groupId, sdl::imtauth::Groups::CGroupData::V1_0& groupData) const
{
	namespace groupssdl = sdl::imtauth::Groups;

	groupssdl::GroupItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = groupId;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	groupssdl::CGroupData payload;
	bool ok = SendModelRequestInternal<groupssdl::GroupItemRequestArguments, groupssdl::CGroupData, groupssdl::CGroupItemGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	groupData = *payload.Version_1_0;

	return true;
}


bool CClientRequestGroupManagerComp::SetGroupDataSdl(const QByteArray& groupId, const sdl::imtauth::Groups::CGroupData::V1_0& groupData) const
{
	namespace groupssdl = sdl::imtauth::Groups;

	groupssdl::GroupUpdateRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = groupId;
	arguments.input.Version_1_0->typeId = QByteArray("Group");
	arguments.input.Version_1_0->item = groupData;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload payload;
	bool ok = SendModelRequestInternal<groupssdl::GroupUpdateRequestArguments, sdl::imtbase::ImtCollection::CUpdatedNotificationPayload, groupssdl::CGroupUpdateGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->id){
		return false;
	}

	return !payload.Version_1_0->id->isEmpty();
}


} // namespace imtauthgql


