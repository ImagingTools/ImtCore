// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens_fwd.h>


namespace imtauthgql
{


class CPersonalAccessTokenControllerComp: 
			public sdl::V1_0::imtauth::CPersonalAccessTokensGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CPersonalAccessTokensGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPersonalAccessTokenControllerComp);
		I_ASSIGN(m_tokenManagerCompPtr, "PersonalAccessTokenManager", "Personal access token manager", true, "PersonalAccessTokenManager");
		I_ASSIGN(m_tokenFactoryCompPtr, "TokenFactory", "Factory for creating token instances", true, "PersonalAccessTokenFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CPersonalAccessTokensGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CPersonalAccessTokenList OnGetTokenList(
				const sdl::V1_0::imtauth::CGetTokenListGqlRequest& getTokenListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CPersonalAccessToken OnGetToken(
				const sdl::V1_0::imtauth::CGetTokenGqlRequest& getTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CValidateTokenPayload OnValidateToken(
				const sdl::V1_0::imtauth::CValidateTokenGqlRequest& validateTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateTokenPayload OnCreateToken(
				const sdl::V1_0::imtauth::CCreateTokenGqlRequest& createTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRevokeTokenPayload OnRevokeToken(
				const sdl::V1_0::imtauth::CRevokeTokenGqlRequest& revokeTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CDeleteTokenPayload OnDeleteToken(
				const sdl::V1_0::imtauth::CDeleteTokenGqlRequest& deleteTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	/**
		Checks whether the caller identified by the request's GraphQL context
		is allowed to manage (list/create/revoke/delete) personal access
		tokens belonging to \p targetUserId.

		Access is granted to system administrators (imtauth::IUserInfo::IsAdmin())
		and to the token owner themselves; anyone else - including
		unauthenticated callers, for whom the request carries no context or no
		resolved user - is denied. ValidateToken() is deliberately not gated by
		this check: presenting the raw token secret is itself the credential.

		\param gqlRequest GraphQL request carrying the caller's context.
		\param targetUserId Owner of the token(s) being accessed.
		\return true if the caller may manage tokens owned by targetUserId.
	*/
	bool IsCallerAuthorizedForUser(const ::imtgql::CGqlRequest& gqlRequest, const QByteArray& targetUserId) const;

	/**
		Checks whether the caller identified by the request's GraphQL context
		is allowed to grant all of the requested permission \p scopes to a
		personal access token.

		Administrators (imtauth::IUserInfo::IsAdmin()) may grant any scope,
		mirroring OnGetUserPermissions() which shows them the full permission
		tree. Any other caller may only grant scopes that are contained in
		their own effective permissions for \p productId - this prevents a
		user from minting a token with permissions they do not have
		themselves (privilege escalation). Callers whose user info cannot be
		resolved are denied (fail closed).

		\param gqlRequest GraphQL request carrying the caller's context.
		\param scopes Requested permission scopes for the new token.
		\param productId Product the token is created for; passed through to
			imtauth::IUserBaseInfo::GetPermissions().
		\return true if the caller may grant all requested scopes.
	*/
	bool AreRequestedScopesAllowed(
				const ::imtgql::CGqlRequest& gqlRequest,
				const QByteArrayList& scopes,
				const QByteArray& productId) const;

private:
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_FACT(imtauth::IPersonalAccessToken, m_tokenFactoryCompPtr);
};


} // namespace imtauthgql


