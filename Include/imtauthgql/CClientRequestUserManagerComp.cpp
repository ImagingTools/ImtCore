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

	imtcollection::CGetElementIdsPayload payload;
	bool ok = SendModelRequestInternal<imtcollection::GetElementIdsRequestArguments, imtcollection::CGetElementIdsPayload, imtcollection::CGetElementIdsGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArrayList();
	}

	if (!payload.Version_1_0->elementIds.HasValue()){
		return QByteArrayList();
	}

	return payload.Version_1_0->elementIds->ToList();
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
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::RemoveElementsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->elementIds.Emplace();
	arguments.input.Version_1_0->elementIds->push_back(userId);
	arguments.input.Version_1_0->collectionId = QByteArrayLiteral("Users");

	imtcollection::CRemoveElementsPayload payload;
	bool ok = SendModelRequestInternal<imtcollection::RemoveElementsRequestArguments, imtcollection::CRemoveElementsPayload, imtcollection::CRemoveElementsGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->success.HasValue()){
		return false;
	}

	return *payload.Version_1_0->success;
}


QByteArray CClientRequestUserManagerComp::CreateUser(const QString& userName, const QByteArray& login, const QByteArray& password, const QString& email)
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserAddRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	arguments.input.Version_1_0->typeId = QByteArrayLiteral("User");

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


QByteArrayList CClientRequestUserManagerComp::GetUserPermissions(const QByteArray& userId) const
{
	imtauth::IUserInfoUniquePtr userInfoPtr = GetUser(userId);
	if (!userInfoPtr.IsValid()){
		return QByteArrayList();
	}

	QByteArray productId;
	if (m_applicationInfoCompPtr.IsValid()){
		productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	if (productId.isEmpty()){
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


template<class Arguments, class Payload, class SdlRequest>
bool CClientRequestUserManagerComp::SendModelRequestInternal(Arguments arguments, Payload& payload) const
{
	imtgql::CGqlRequest gqlRequest;

	if (m_accessTokenProviderCompPtr.IsValid()){
		QByteArray accessToken = m_accessTokenProviderCompPtr->GetToken("");
		gqlRequest.SetHeader("x-authentication-token", accessToken);
	}

	if (!SdlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	payload = SendModelRequest<Payload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!payload.Version_1_0.HasValue()){
		return false;
	}

	return true;
}


} // namespace imtauthgql


