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


QByteArray CClientRequestUserManagerComp::GetUserObjectId(const QByteArray& login) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::GetUserObjectIdRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->login = login;

	userssdl::CUserObjectId payload;
	bool ok = SendModelRequestInternal<userssdl::GetUserObjectIdRequestArguments, userssdl::CUserObjectId, userssdl::CGetUserObjectIdGqlRequest>(arguments, payload);
	if (!ok){
		return nullptr;
	}

	if (!payload.Version_1_0->objectId.HasValue()){
		return QByteArray();
	}

	return *payload.Version_1_0->objectId;
}


imtauth::IUserInfoUniquePtr CClientRequestUserManagerComp::GetUser(const QByteArray& userId) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = userId;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
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
	if (!m_userRepresentationController.FillUserInfoFromRepresentation(*payload.Version_1_0, *userInfoPtr, nullptr, userId, errorMessage)){
		return nullptr;
	}

	return userInfoPtr.PopInterfacePtr();
}


bool CClientRequestUserManagerComp::RemoveUser(const QByteArray& userId)
{
	return RemoveElements(QByteArrayLiteral("Users"), {userId});
}


QByteArray CClientRequestUserManagerComp::CreateUser(const QString& userName, const QByteArray& login, const QByteArray& password, const QString& email)
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserAddRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	arguments.input.Version_1_0->typeId = QByteArrayLiteral("User");
	arguments.input.Version_1_0->name = userName;

	userssdl::CUserData::V1_0 userData;
	userData.name = userName;
	userData.username = login;
	userData.email = email;
	userData.password = password;

	arguments.input.Version_1_0->item = userData;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	sdl::imtbase::ImtCollection::CAddedNotificationPayload payload;
	bool ok = SendModelRequestInternal<userssdl::UserAddRequestArguments, sdl::imtbase::ImtCollection::CAddedNotificationPayload, userssdl::CUserAddGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArray();
	}

	if (!payload.Version_1_0->id.HasValue()){
		return QByteArray();
	}

	return *payload.Version_1_0->id;
}


bool CClientRequestUserManagerComp::ChangeUserPassword(const QByteArray& login, const QByteArray& oldPassword, const QByteArray& newPassword)
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::ChangePasswordRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->login = login;
	arguments.input.Version_1_0->oldPassword = oldPassword;
	arguments.input.Version_1_0->newPassword = newPassword;

	userssdl::CChangePasswordPayload payload;
	bool ok = SendModelRequestInternal<userssdl::ChangePasswordRequestArguments, userssdl::CChangePasswordPayload, userssdl::CChangePasswordGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->success.HasValue()){
		return false;
	}

	return *payload.Version_1_0->success;
}


bool CClientRequestUserManagerComp::AddRolesToUser(const QByteArray& userId, const QByteArray& /*productId*/, const QByteArrayList& roleIds)
{
	if (roleIds.isEmpty()){
		return false;
	}

	sdl::imtauth::Users::CUserData::V1_0 userData;
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

	sdl::imtauth::Users::CUserData::V1_0 userData;
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

	sdl::imtauth::Users::CUserData::V1_0 userData;
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

	QList<sdl::imtauth::Users::CSystemInfo::V1_0> infos = userData.systemInfos->ToList();
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

bool CClientRequestUserManagerComp::GetUserDataSdl(const QByteArray& userId, sdl::imtauth::Users::CUserData::V1_0& userData) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = userId;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	sdl::imtauth::Users::CUserData payload;
	bool ok = SendModelRequestInternal<userssdl::UserItemRequestArguments, sdl::imtauth::Users::CUserData, userssdl::CUserItemGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	userData = *payload.Version_1_0;

	return true;
}


bool CClientRequestUserManagerComp::SetUserDataSdl(const QByteArray& userId, const sdl::imtauth::Users::CUserData::V1_0& userData) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserUpdateRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = userId;
	arguments.input.Version_1_0->typeId = QByteArray("User");

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	arguments.input.Version_1_0->item = userData;

	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload payload;
	bool ok = SendModelRequestInternal<userssdl::UserUpdateRequestArguments, sdl::imtbase::ImtCollection::CUpdatedNotificationPayload, userssdl::CUserUpdateGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->id){
		return false;
	}

	return !payload.Version_1_0->id->isEmpty();
}


} // namespace imtauthgql


