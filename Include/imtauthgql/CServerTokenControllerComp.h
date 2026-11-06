// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtbase/IObjectCollection.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ServerTokens_fwd.h>


namespace imtauthgql
{


/**
	Tokens of the servers of products. Available on the authorization server only, to a superuser signed in with a session.
	It is a handler of its own so that product servers, which forward the personal access token commands, do not forward these.
*/
class CServerTokenControllerComp: public sdl::V1_0::imtauth::CServerTokensGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CServerTokensGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CServerTokenControllerComp);
		I_ASSIGN(m_tokenManagerCompPtr, "PersonalAccessTokenManager", "Personal access token manager", true, "PersonalAccessTokenManager");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Catalog of connected products: a token is issued for a product of the catalog", true, "ProductCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CServerTokensGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CServerTokenList OnGetServerTokenList(
				const sdl::V1_0::imtauth::CGetServerTokenListGqlRequest& getServerTokenListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateServerTokenPayload OnCreateServerToken(
				const sdl::V1_0::imtauth::CCreateServerTokenGqlRequest& createServerTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRevokeServerTokenPayload OnRevokeServerToken(
				const sdl::V1_0::imtauth::CRevokeServerTokenGqlRequest& revokeServerTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRevokeServerTokenPayload OnUpdateServerTokenScopes(
				const sdl::V1_0::imtauth::CUpdateServerTokenScopesGqlRequest& updateServerTokenScopesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CServerTokenReceiverList OnGetServerTokenReceivers(
				const sdl::V1_0::imtauth::CGetServerTokenReceiversGqlRequest& getServerTokenReceiversRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	bool CheckSuperuserSession(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

private:
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtauthgql
