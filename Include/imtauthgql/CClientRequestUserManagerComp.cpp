#include <imtauthgql/CClientRequestUserManagerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IUserManager)

QByteArrayList CClientRequestUserManagerComp::GetUserIds() const
{
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::GetElementIdsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->collectionId = QByteArrayLiteral("Users");

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


imtauth::IUserInfoUniquePtr CClientRequestUserManagerComp::GetUser(const QByteArray& userId, const QByteArray& productId) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = userId;
	arguments.input.Version_1_0->productId = productId;

	imtgql::CGqlRequest gqlRequest;
	if (!userssdl::CUserItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	QString errorMessage;
	userssdl::CUserData payload = SendModelRequest<userssdl::CUserData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (!payload.Version_1_0.HasValue()){
		return nullptr;
	}

	imtauth::IUserInfoUniquePtr userInfoPtr = m_userFactoryCompPtr.CreateInstance();
	if (!userInfoPtr.IsValid()){
		return nullptr;
	}

	if (!m_userRepresentationController.FillUserInfoFromRepresentation(*payload.Version_1_0, *userInfoPtr, nullptr, userId, errorMessage)){
		return nullptr;
	}

	return userInfoPtr.PopInterfacePtr();
}


bool CClientRequestUserManagerComp::RemoveUser(const QByteArray& userId)
{
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::RemoveElementsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->elementIds.Emplace();
	arguments.input.Version_1_0->elementIds->push_back(userId);
	arguments.input.Version_1_0->collectionId = QByteArrayLiteral("Users");

	imtgql::CGqlRequest gqlRequest;
	if (!imtcollection::CRemoveElementsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	imtcollection::CRemoveElementsPayload response = SendModelRequest<imtcollection::CRemoveElementsPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!response.Version_1_0.HasValue()){
		return false;
	}

	if (!response.Version_1_0->success.HasValue()){
		return false;
	}

	return *response.Version_1_0->success;
}


QByteArray CClientRequestUserManagerComp::CreateUser(const QString& userName, const QByteArray& login, const QByteArray& password)
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserAddRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	userssdl::CUserData::V1_0 userData;
	userData.name = userName;
	userData.username = login;

	arguments.input.Version_1_0->item = userData;

	imtgql::CGqlRequest gqlRequest;
	if (!userssdl::CUserAddGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
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


bool CClientRequestUserManagerComp::ChangeUserPassword(const QByteArray& login, const QByteArray& oldPassword, const QByteArray& newPassword)
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::ChangePasswordRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->login = login;
	arguments.input.Version_1_0->oldPassword = oldPassword;
	arguments.input.Version_1_0->newPassword = newPassword;

	imtgql::CGqlRequest gqlRequest;
	if (!userssdl::CChangePasswordGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	userssdl::CChangePasswordPayload response = SendModelRequest<userssdl::CChangePasswordPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!response.Version_1_0.HasValue()){
		return false;
	}

	if (!response.Version_1_0->success.HasValue()){
		return false;
	}

	return *response.Version_1_0->success;
}


bool CClientRequestUserManagerComp::AddRolesToUser(const QByteArray& userId, const QByteArrayList& roleIds)
{
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


bool CClientRequestUserManagerComp::RemoveRolesFromUser(const QByteArray& userId, const QByteArrayList& roleIds)
{
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
	imtauth::IUserInfoUniquePtr userInfoPtr = GetUser(userId, productId);
	if (!userInfoPtr.IsValid()){
		return QByteArrayList();
	}

	return userInfoPtr->GetPermissions(productId);
}


// private methods

bool CClientRequestUserManagerComp::GetUserDataSdl(const QByteArray& userId, sdl::imtauth::Users::CUserData::V1_0& userData) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = userId;

	imtgql::CGqlRequest gqlRequest;
	if (!userssdl::CUserItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	sdl::imtauth::Users::CUserData response = SendModelRequest<userssdl::CUserData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!response.Version_1_0.HasValue()){
		return false;
	}

	userData = *response.Version_1_0;

	return true;
}


bool CClientRequestUserManagerComp::SetUserDataSdl(const QByteArray& userId, const sdl::imtauth::Users::CUserData::V1_0& userData) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserUpdateRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = userId;
	arguments.input.Version_1_0->item = userData;

	imtgql::CGqlRequest gqlRequest;
	if (!userssdl::CUserUpdateGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
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


