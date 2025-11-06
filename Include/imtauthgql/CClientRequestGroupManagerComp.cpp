#include <imtauthgql/CClientRequestGroupManagerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IUserGroupManager)

QByteArrayList CClientRequestGroupManagerComp::GetGroupIds() const
{
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::GetElementIdsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->collectionId = QByteArrayLiteral("Groups");

	imtgql::CGqlRequest gqlRequest;
	if (!imtcollection::CGetElementIdsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArrayList();
	}

	QString errorMessage;
	imtcollection::CGetElementIdsPayload payload = SendModelRequest<imtcollection::CGetElementIdsPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArrayList();
	}

	if (!payload.Version_1_0.HasValue()){
		return QByteArrayList();
	}

	if (!payload.Version_1_0->elementIds.HasValue()){
		return QByteArrayList();
	}

	return payload.Version_1_0->elementIds->ToList();
}


QByteArray CClientRequestGroupManagerComp::CreateGroup(const QString& groupName, const QString& description)
{
	namespace groupssdl = sdl::imtauth::Groups;

	groupssdl::GroupAddRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	groupssdl::CGroupData::V1_0 groupData;
	groupData.description = description;
	groupData.name = groupName;

	arguments.input.Version_1_0->item = groupData;

	imtgql::CGqlRequest gqlRequest;
	if (!groupssdl::CGroupAddGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	QString errorMessage;
	sdl::imtbase::ImtCollection::CAddedNotificationPayload payload = SendModelRequest<sdl::imtbase::ImtCollection::CAddedNotificationPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArray();
	}

	if (!payload.Version_1_0.HasValue()){
		return QByteArray();
	}

	if (!payload.Version_1_0->id.HasValue()){
		return QByteArray();
	}

	return *payload.Version_1_0->id;
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
	if (groupData.productId){
		productId = *groupData.productId;
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

	return groupInfoPtr.PopInterfacePtr();
}


bool CClientRequestGroupManagerComp::AddUsersToGroup(const QByteArray& groupId, const QByteArrayList& userIds)
{
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

	imtgql::CGqlRequest gqlRequest;
	if (!groupssdl::CGroupItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	groupssdl::CGroupData response = SendModelRequest<groupssdl::CGroupData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!response.Version_1_0.HasValue()){
		return false;
	}

	groupData = *response.Version_1_0;

	return true;
}


bool CClientRequestGroupManagerComp::SetGroupDataSdl(const QByteArray& groupId, const sdl::imtauth::Groups::CGroupData::V1_0& groupData) const
{
	namespace groupssdl = sdl::imtauth::Groups;

	groupssdl::GroupUpdateRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = groupId;
	arguments.input.Version_1_0->item = groupData;

	imtgql::CGqlRequest gqlRequest;
	if (!groupssdl::CGroupUpdateGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload response = SendModelRequest<sdl::imtbase::ImtCollection::CUpdatedNotificationPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!response.Version_1_0.HasValue()){
		return false;
	}

	if (!response.Version_1_0->id){
		return false;
	}

	return !response.Version_1_0->id->isEmpty();
}


} // namespace imtauthgql


