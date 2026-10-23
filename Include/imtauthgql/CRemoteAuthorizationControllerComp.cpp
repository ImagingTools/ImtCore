// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteAuthorizationControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


// ImtCore includes
#include <imtgql/imtgql.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IExternalLoginCallbackHandler)

bool CRemoteAuthorizationControllerComp::HandleCallback(
			const QByteArray& state,
			const QByteArray& code,
			const QString& providerError,
			QString& message) const
{
	namespace authsdl = sdl::V1_0::imtauth;

	authsdl::HandleExternalLoginCallbackRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->state = state;
	arguments.input->code = QString::fromUtf8(code);
	arguments.input->error = providerError;

	imtgql::CGqlRequest gqlRequest;
	if (!authsdl::CHandleExternalLoginCallbackGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		message = QStringLiteral("The sign-in could not be passed to the authorization server.");

		return false;
	}

	authsdl::CHandleExternalLoginCallbackGqlRequest callbackRequest(gqlRequest, false);

	QString errorMessage;
	authsdl::CExternalLoginCallbackPayload payload = OnHandleExternalLoginCallback(callbackRequest, gqlRequest, errorMessage);
	if (!payload.ok.HasValue()){
		SendErrorMessage(0, QStringLiteral("External sign-in callback was not accepted by the authorization server: %1").arg(errorMessage));

		message = QStringLiteral("The authorization server is not available. Please try again later.");

		return false;
	}

	message = payload.message.HasValue() ? *payload.message : QString();

	return *payload.ok;
}


// protected methods

// reimplemented (sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase)

sdl::V1_0::imtauth::CAuthorizationPayload CRemoteAuthorizationControllerComp::OnAuthorization(
			const sdl::V1_0::imtauth::CAuthorizationGqlRequest& /*authorizationRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CAuthorizationPayload payload = SendModelRequest<sdl::V1_0::imtauth::CAuthorizationPayload>(gqlRequest, errorMessage);
	AddImpliedPermissions(gqlRequest, payload);

	return payload;
}


sdl::V1_0::imtauth::CAuthorizationPayload CRemoteAuthorizationControllerComp::OnUserToken(
			const sdl::V1_0::imtauth::CUserTokenGqlRequest& /*userTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CAuthorizationPayload payload = SendModelRequest<sdl::V1_0::imtauth::CAuthorizationPayload>(gqlRequest, errorMessage);
	AddImpliedPermissions(gqlRequest, payload);

	return payload;
}


sdl::V1_0::imtauth::CLogoutPayload CRemoteAuthorizationControllerComp::OnLogout(
			const sdl::V1_0::imtauth::CLogoutGqlRequest& /*logoutRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CLogoutPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CPermissionList CRemoteAuthorizationControllerComp::OnGetPermissions(
			const sdl::V1_0::imtauth::CGetPermissionsGqlRequest& /*getPermissionsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CPermissionList response = SendModelRequest<sdl::V1_0::imtauth::CPermissionList>(gqlRequest, errorMessage);

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr && response.permissions.HasValue()){
		QByteArrayList permissions = response.permissions->ToList();
		permissions += imtgql::CollectImpliedPermissions(*gqlContextPtr, permissions);
		const imtauth::IUserInfo::FeatureIds grantedPermissions = imtgql::CollectTenantOwnerPermissions(*gqlContextPtr)
					+ imtgql::CollectOrganizationAccessPermissions(*gqlContextPtr, gqlContextPtr->GetUserId());
		for (const QByteArray& permission : grantedPermissions){
			if (!permissions.contains(permission)){
				permissions.append(permission);
			}
		}
		response.permissions->FromList(permissions);
	}

	return response;
}


// private methods

void CRemoteAuthorizationControllerComp::AddImpliedPermissions(
			const imtgql::CGqlRequest& gqlRequest,
			sdl::V1_0::imtauth::CAuthorizationPayload& payload) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr || !payload.permissions.HasValue() || payload.permissions->isEmpty()){
		return;
	}

	QByteArrayList permissions = payload.permissions->split(';');
	permissions += imtgql::CollectImpliedPermissions(*gqlContextPtr, permissions);
	payload.permissions = permissions.join(';');
}


sdl::V1_0::imtauth::CExternalLoginProviderList CRemoteAuthorizationControllerComp::OnGetExternalLoginProviders(
			const sdl::V1_0::imtauth::CGetExternalLoginProvidersGqlRequest& /*getExternalLoginProvidersRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CExternalLoginProviderList>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CExternalIdentityList CRemoteAuthorizationControllerComp::OnGetExternalIdentities(
			const sdl::V1_0::imtauth::CGetExternalIdentitiesGqlRequest& /*getExternalIdentitiesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CExternalIdentityList>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CBeginExternalLoginPayload CRemoteAuthorizationControllerComp::OnBeginExternalLogin(
			const sdl::V1_0::imtauth::CBeginExternalLoginGqlRequest& /*beginExternalLoginRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CBeginExternalLoginPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CExternalLoginPayload CRemoteAuthorizationControllerComp::OnCompleteExternalLogin(
			const sdl::V1_0::imtauth::CCompleteExternalLoginGqlRequest& /*completeExternalLoginRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CExternalLoginPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CExternalLoginCallbackPayload CRemoteAuthorizationControllerComp::OnHandleExternalLoginCallback(
			const sdl::V1_0::imtauth::CHandleExternalLoginCallbackGqlRequest& /*handleExternalLoginCallbackRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CExternalLoginCallbackPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CUnlinkExternalIdentityPayload CRemoteAuthorizationControllerComp::OnUnlinkExternalIdentity(
			const sdl::V1_0::imtauth::CUnlinkExternalIdentityGqlRequest& /*unlinkExternalIdentityRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CUnlinkExternalIdentityPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


