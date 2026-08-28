// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestUserManagerComp.h>


// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IUserManager)

QByteArrayList CClientRequestUserManagerComp::GetUserIds() const
{
	return GetElementIds(QByteArrayLiteral("Users"));
}


QList<imtauth::IUserManager::User> CClientRequestUserManagerComp::GetUserList() const
{
	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::UsersListRequestArguments arguments;
	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.emplace();
		arguments.input->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	userssdl::CUsersListPayload payload;
	bool ok = SendModelRequestInternal<userssdl::UsersListRequestArguments, userssdl::CUsersListPayload, userssdl::CUsersListGqlRequest>(arguments, payload);
	if (!ok){
		return {};
	}

	if (!payload.items.HasValue()){
		return {};
	}

	QList<imtauth::IUserManager::User> result;

	for (const auto& item : payload.items->ToList()){
		imtauth::IUserManager::User user;

		if (item.id){
			user.uuid = *item.id;
		}

		if (item.name){
			user.name = *item.name;
		}

		if (item.systemName){
			user.systemName = *item.systemName;
		}

		if (item.mail){
			user.email = *item.mail;
		}

		if (item.userId){
			user.login = *item.userId;
		}

		if (item.roles){
			user.roleIds = (*item.roles).split(';');
		}

		if (item.groups){
			user.groupIds = (*item.groups).split(';');
		}

		result << user;
	}

	return result;
}


QByteArray CClientRequestUserManagerComp::GetUserObjectId(const QByteArray& login) const
{
	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::GetUserObjectIdRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->login = login;

	userssdl::CUserObjectId payload;
	bool ok = SendModelRequestInternal<userssdl::GetUserObjectIdRequestArguments, userssdl::CUserObjectId, userssdl::CGetUserObjectIdGqlRequest>(arguments, payload);
	if (!ok){
		return nullptr;
	}

	if (!payload.objectId.HasValue()){
		return QByteArray();
	}

	return *payload.objectId;
}


imtauth::IUserInfoUniquePtr CClientRequestUserManagerComp::GetUser(const QByteArray& userId) const
{
	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::UserItemRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = userId;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	userssdl::CUserData payload;
	bool ok = SendModelRequestInternal<userssdl::UserItemRequestArguments, userssdl::CUserData, userssdl::CUserItemGqlRequest>(arguments, payload);
	if (!ok){
		return nullptr;
	}

	imtauth::IUserInfoUniquePtr userInfoPtr = m_userFactoryCompPtr.CreateInstance();
	if (!userInfoPtr.IsValid()){
		return nullptr;
	}

	QString errorMessage;
	if (!m_userRepresentationController.FillUserInfoFromRepresentation(payload, *userInfoPtr, nullptr, userId, errorMessage)){
		return nullptr;
	}

	return userInfoPtr;
}


bool CClientRequestUserManagerComp::RemoveUser(const QByteArray& userId)
{
	return RemoveElements(QByteArrayLiteral("Users"), {userId});
}


QByteArray CClientRequestUserManagerComp::CreateUser(const QString& userName, const QByteArray& login, const QByteArray& password, const QString& email)
{
	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::UserAddRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	arguments.input->typeId = QByteArrayLiteral("User");
	arguments.input->name = userName;

	userssdl::CUserData userData;
	userData.name = userName;
	userData.username = login;
	userData.email = email;
	userData.password = password;

	arguments.input->item = userData;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	sdl::V1_0::imtbase::CAddedNotificationPayload payload;
	bool ok = SendModelRequestInternal<userssdl::UserAddRequestArguments, sdl::V1_0::imtbase::CAddedNotificationPayload, userssdl::CUserAddGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArray();
	}

	if (!payload.id.HasValue()){
		return QByteArray();
	}

	return *payload.id;
}


bool CClientRequestUserManagerComp::ChangeUserPassword(const QByteArray& login, const QByteArray& oldPassword, const QByteArray& newPassword)
{
	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::ChangePasswordRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->login = login;
	arguments.input->oldPassword = oldPassword;
	arguments.input->newPassword = newPassword;

	userssdl::CChangePasswordPayload payload;
	bool ok = SendModelRequestInternal<userssdl::ChangePasswordRequestArguments, userssdl::CChangePasswordPayload, userssdl::CChangePasswordGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.success.HasValue()){
		return false;
	}

	return *payload.success;
}


bool CClientRequestUserManagerComp::AddRolesToUser(const QByteArray& userId, const QByteArray& /*productId*/, const QByteArrayList& roleIds)
{
	if (roleIds.isEmpty()){
		return false;
	}

	sdl::V1_0::imtauth::CUserData userData;
	bool ok = GetUserDataSdl(userId, userData);
	if (!ok){
		return false;
	}

	if (!userData.roles.HasValue()){
		return false;
	}

	QByteArrayList currentRoleIds = userData.roles->ToList();

	QByteArrayList result = currentRoleIds;
	for (const QByteArray& roleId : roleIds){
		if (!result.contains(roleId)){
			result << roleId;
		}
	}

	userData.roles->FromList(result);

	return SetUserDataSdl(userId, userData);
}


bool CClientRequestUserManagerComp::RemoveRolesFromUser(const QByteArray& userId, const QByteArray& /*productId*/, const QByteArrayList& roleIds)
{
	if (roleIds.isEmpty()){
		return false;
	}

	sdl::V1_0::imtauth::CUserData userData;
	bool ok = GetUserDataSdl(userId, userData);
	if (!ok){
		return false;
	}

	if (!userData.roles.HasValue()){
		return false;
	}

	QByteArrayList currentRoleIds = userData.roles->ToList();

	QByteArrayList result = currentRoleIds;
	for (const QByteArray& roleId : roleIds){
		if (result.contains(roleId)){
			result.removeAll(roleId);
		}
	}

	userData.roles->FromList(result);

	return SetUserDataSdl(userId, userData);
}


QByteArrayList CClientRequestUserManagerComp::GetUserPermissions(const QByteArray& userId, const QByteArray& productId) const
{
	imtauth::IUserInfoUniquePtr userInfoPtr = GetUser(userId);
	if (!userInfoPtr.IsValid()){
		return QByteArrayList();
	}

	if (productId.isEmpty()){
		return QByteArrayList();
	}

	QByteArrayList products = userInfoPtr->GetProducts();
	QByteArrayList permissions = userInfoPtr->GetPermissions(productId);

	return userInfoPtr->GetPermissions(productId);
}


bool CClientRequestUserManagerComp::GetUserAuthSystem(const QByteArray& login, imtauth::IUserInfo::SystemInfo& systemInfo) const
{
	QByteArray objectId = GetUserObjectId(login);
	if (objectId.isEmpty()){
		return false;
	}

	sdl::V1_0::imtauth::CUserData userData;
	bool ok = GetUserDataSdl(objectId, userData);
	if (!ok){
		return false;
	}

	if (!userData.systemInfos.HasValue()){
		return false;
	}

	if (userData.systemInfos->isEmpty()){
		return false;
	}

	QList<sdl::V1_0::imtauth::CSystemInfo> infos = userData.systemInfos->ToList();
	if (infos[0].id){
		systemInfo.systemId = *infos[0].id;
	}

	if (infos[0].name){
		systemInfo.systemName = *infos[0].name;
	}

	if (infos[0].enabled){
		systemInfo.enabled = *infos[0].enabled;
	}

	return true;
}


// private methods

bool CClientRequestUserManagerComp::GetUserDataSdl(const QByteArray& userId, sdl::V1_0::imtauth::CUserData& userData) const
{
	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::UserItemRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = userId;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	sdl::V1_0::imtauth::CUserData payload;
	bool ok = SendModelRequestInternal<userssdl::UserItemRequestArguments, sdl::V1_0::imtauth::CUserData, userssdl::CUserItemGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	userData = payload;

	return true;
}


bool CClientRequestUserManagerComp::SetUserDataSdl(const QByteArray& userId, const sdl::V1_0::imtauth::CUserData& userData) const
{
	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::UserUpdateRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = userId;
	arguments.input->typeId = QByteArrayLiteral("User");

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	arguments.input->item = userData;

	sdl::V1_0::imtbase::CUpdatedNotificationPayload payload;
	bool ok = SendModelRequestInternal<userssdl::UserUpdateRequestArguments, sdl::V1_0::imtbase::CUpdatedNotificationPayload, userssdl::CUserUpdateGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.id){
		return false;
	}

	return !payload.id->isEmpty();
}


} // namespace imtauthgql


