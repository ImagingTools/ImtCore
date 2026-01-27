#include <imtauthgql/CPersonalAccessTokenControllerComp.h>


// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::PersonalAccessTokens::CGraphQlHandlerCompBase)

sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenList CPersonalAccessTokenControllerComp::OnGetTokenList(
			const sdl::imtauth::PersonalAccessTokens::CGetTokenListGqlRequest& getTokenListRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenList response;

	return response;
}


sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken CPersonalAccessTokenControllerComp::OnGetToken(
			const sdl::imtauth::PersonalAccessTokens::CGetTokenGqlRequest& getTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken response;
	return response;
}


sdl::imtauth::PersonalAccessTokens::CValidateTokenPayload CPersonalAccessTokenControllerComp::OnValidateToken(
			const sdl::imtauth::PersonalAccessTokens::CValidateTokenGqlRequest& validateTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CValidateTokenPayload response;
	return response;
}


sdl::imtauth::PersonalAccessTokens::CCreateTokenPayload CPersonalAccessTokenControllerComp::OnCreateToken(
			const sdl::imtauth::PersonalAccessTokens::CCreateTokenGqlRequest& createTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CCreateTokenPayload response;
	return response;
}


sdl::imtauth::PersonalAccessTokens::CRevokeTokenPayload CPersonalAccessTokenControllerComp::OnRevokeToken(
			const sdl::imtauth::PersonalAccessTokens::CRevokeTokenGqlRequest& revokeTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CRevokeTokenPayload response;
	return response;
}


sdl::imtauth::PersonalAccessTokens::CDeleteTokenPayload CPersonalAccessTokenControllerComp::OnDeleteToken(
			const sdl::imtauth::PersonalAccessTokens::CDeleteTokenGqlRequest& deleteTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CDeleteTokenPayload response;
	return response;
}


} // namespace imtauthgql


