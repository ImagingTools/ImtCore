// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens.h>


namespace imtauthgql
{


class CPersonalAccessTokenControllerComp: 
			public sdl::imtauth::PersonalAccessTokens::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::PersonalAccessTokens::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPersonalAccessTokenControllerComp);
		I_ASSIGN(m_tokenManagerCompPtr, "PersonalAccessTokenManager", "Personal access token manager", true, "PersonalAccessTokenManager");
		I_ASSIGN(m_tokenFactoryCompPtr, "TokenFactory", "Factory for creating token instances", true, "PersonalAccessTokenFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::PersonalAccessTokens::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenList OnGetTokenList(
				const sdl::imtauth::PersonalAccessTokens::CGetTokenListGqlRequest& getTokenListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken OnGetToken(
				const sdl::imtauth::PersonalAccessTokens::CGetTokenGqlRequest& getTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::PersonalAccessTokens::CValidateTokenPayload OnValidateToken(
				const sdl::imtauth::PersonalAccessTokens::CValidateTokenGqlRequest& validateTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::PersonalAccessTokens::CCreateTokenPayload OnCreateToken(
				const sdl::imtauth::PersonalAccessTokens::CCreateTokenGqlRequest& createTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::PersonalAccessTokens::CRevokeTokenPayload OnRevokeToken(
				const sdl::imtauth::PersonalAccessTokens::CRevokeTokenGqlRequest& revokeTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::PersonalAccessTokens::CDeleteTokenPayload OnDeleteToken(
				const sdl::imtauth::PersonalAccessTokens::CDeleteTokenGqlRequest& deleteTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_FACT(imtauth::IPersonalAccessToken, m_tokenFactoryCompPtr);
};


} // namespace imtauthgql


