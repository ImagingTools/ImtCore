// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtauth/IExternalLoginCallbackHandler.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization_fwd.h>


namespace imtauthgql
{


/**
	Forwards the authorization requests to the authorization server.
	As callback handler it passes the redirect of an external identity provider, received by
	the callback servlet of this server, to the authorization server that started the login.
*/
class CRemoteAuthorizationControllerComp:
		virtual public imtauth::IExternalLoginCallbackHandler,
		public imtclientgql::TClientRequestManagerCompWrap<
					sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteAuthorizationControllerComp);
		I_REGISTER_INTERFACE(imtauth::IExternalLoginCallbackHandler);
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalLoginCallbackHandler)
	virtual bool HandleCallback(
				const QByteArray& state,
				const QByteArray& code,
				const QString& providerError,
				QString& message) const override;

protected:
	// reimplemented (sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CAuthorizationPayload OnAuthorization(
				const sdl::V1_0::imtauth::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAuthorizationPayload OnUserToken(
				const sdl::V1_0::imtauth::CUserTokenGqlRequest& userTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CLogoutPayload OnLogout(
				const sdl::V1_0::imtauth::CLogoutGqlRequest& logoutRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CPermissionList OnGetPermissions(
				const sdl::V1_0::imtauth::CGetPermissionsGqlRequest& getPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::V1_0::imtauth::CExternalLoginProviderList OnGetExternalLoginProviders(
				const sdl::V1_0::imtauth::CGetExternalLoginProvidersGqlRequest& getExternalLoginProvidersRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CExternalIdentityList OnGetExternalIdentities(
				const sdl::V1_0::imtauth::CGetExternalIdentitiesGqlRequest& getExternalIdentitiesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CBeginExternalLoginPayload OnBeginExternalLogin(
				const sdl::V1_0::imtauth::CBeginExternalLoginGqlRequest& beginExternalLoginRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CExternalLoginPayload OnCompleteExternalLogin(
				const sdl::V1_0::imtauth::CCompleteExternalLoginGqlRequest& completeExternalLoginRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CExternalLoginCallbackPayload OnHandleExternalLoginCallback(
				const sdl::V1_0::imtauth::CHandleExternalLoginCallbackGqlRequest& handleExternalLoginCallbackRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUnlinkExternalIdentityPayload OnUnlinkExternalIdentity(
				const sdl::V1_0::imtauth::CUnlinkExternalIdentityGqlRequest& unlinkExternalIdentityRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	// The accounts service knows no product tree, so what the permissions include is added here.
	void AddImpliedPermissions(const imtgql::CGqlRequest& gqlRequest, sdl::V1_0::imtauth::CAuthorizationPayload& payload) const;
};


} // namespace imtauthgql

