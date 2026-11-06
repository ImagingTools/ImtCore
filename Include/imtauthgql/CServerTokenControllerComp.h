// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/IProductInfo.h>
#include <imtsdl/TElementList.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ServerTokens_fwd.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Permissions.h>


namespace imtauthgql
{


/**
	Tokens of the servers of products. Available on the authorization server only, to a superuser signed in with a session.
	It is a handler of its own so that product servers, which forward the personal access token commands, do not forward these.
	The server issues tokens for itself only; a token holds the product server permissions.
*/
class CServerTokenControllerComp: public sdl::V1_0::imtauth::CServerTokensGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CServerTokensGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CServerTokenControllerComp);
		I_ASSIGN(m_tokenManagerCompPtr, "PersonalAccessTokenManager", "Personal access token manager", true, "PersonalAccessTokenManager");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Catalog of connected products: a token is issued for a product of the catalog", true, "ProductCollection");
		I_ASSIGN(m_permissionTreeCompPtr, "PermissionTree", "Permissions of this server, given to the roles of the products", true, "PermissionTree");
		I_ASSIGN(m_serverPermissionsCompPtr, "ProductServerPermissions", "What a product server may do here: the permissions of its token", true, "ProductServerPermissions");
		I_ASSIGN_MULTI_0(m_requiredPermissionsAttrPtr, "RequiredPermissions", "Permissions every token for this server holds", false);
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
	virtual sdl::V1_0::imtauth::CServerFeatureList OnGetServerTokenFeatures(
				const sdl::V1_0::imtauth::CGetServerTokenFeaturesGqlRequest& getServerTokenFeaturesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	bool CheckSuperuserSession(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	QByteArray GetOwnProductId() const;

	/**
		The permissions a token for the receiving server can hold, as groups of the tree of that server.
		\return false if the tree could not be read or the server takes no tokens.
	*/
	bool GetReceiverGroups(
				const QByteArray& receiverProductId,
				imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionGroup>& groups,
				QString& errorMessage) const;

	/**
		Check the permissions of a token for a receiving server and complete them: a group stands for its permissions,
		then the ones every token holds and the ones they require are added.
	*/
	bool NormalizeScopes(const QByteArray& receiverProductId, QByteArrayList& scopes, QString& errorMessage) const;

private:
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtlic::IProductInfo, m_permissionTreeCompPtr);
	I_REF(imtlic::IFeatureInfoProvider, m_serverPermissionsCompPtr);
	I_MULTIATTR(QByteArray, m_requiredPermissionsAttrPtr);
};


} // namespace imtauthgql


