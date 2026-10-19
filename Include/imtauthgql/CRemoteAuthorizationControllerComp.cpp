// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteAuthorizationControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


// ImtCore includes
#include <imtgql/imtgql.h>


namespace imtauthgql
{


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


} // namespace imtauthgql


