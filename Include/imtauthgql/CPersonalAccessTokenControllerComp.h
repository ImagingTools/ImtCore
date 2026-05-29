// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens>


namespace imtauthgql
{


class CPersonalAccessTokenControllerComp: 
			public sdl::V1_0::imtauth::CGraphQlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPersonalAccessTokenControllerComp);
		I_ASSIGN(m_tokenManagerCompPtr, "PersonalAccessTokenManager", "Personal access token manager", true, "PersonalAccessTokenManager");
		I_ASSIGN(m_tokenFactoryCompPtr, "TokenFactory", "Factory for creating token instances", true, "PersonalAccessTokenFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CGraphQlHandlerCompBase)
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
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_FACT(imtauth::IPersonalAccessToken, m_tokenFactoryCompPtr);
};


} // namespace imtauthgql


