// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// ImtCore includes
#include <imtauth/IClientProductCatalog.h>
#include <imtauth/IPersonalAccessToken.h>
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtlic/CProductInfo.h>
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
	The server issues tokens for itself only. A token holds the permissions of the tree given for the servers of products,
	or of the permission tree of this server when no such tree is given.
*/
class CServerTokenControllerComp: public sdl::V1_0::imtauth::CServerTokensGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CServerTokensGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CServerTokenControllerComp);
		I_ASSIGN(m_tokenManagerCompPtr, "PersonalAccessTokenManager", "Personal access token manager", true, "PersonalAccessTokenManager");
		I_ASSIGN(m_clientProductCatalogCompPtr, "ClientProductCatalog", "Products whose servers may be given a token of this server", true, "ClientProductCatalog");
		I_ASSIGN(m_permissionTreeCompPtr, "PermissionTree", "Permissions of this server, given to the roles of the products", true, "PermissionTree");
		I_ASSIGN(m_serverPermissionsCompPtr, "ProductServerPermissions", "What a product server may do here: the permissions of its token; without it the permission tree of this server", false, "ProductServerPermissions");
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
	virtual sdl::V1_0::imtauth::CUpdateServerTokenScopesPayload OnUpdateServerTokenScopes(
				const sdl::V1_0::imtauth::CUpdateServerTokenScopesGqlRequest& updateServerTokenScopesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAddServerTokenClientPayload OnAddServerTokenClient(
				const sdl::V1_0::imtauth::CAddServerTokenClientGqlRequest& addServerTokenClientRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CServerTokenReceiver OnGetServerTokenReceiver(
				const sdl::V1_0::imtauth::CGetServerTokenReceiverGqlRequest& getServerTokenReceiverRequest,
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
		Check that the product is another product than this server.
	*/
	bool CheckAnotherProduct(const QByteArray& clientProductId, QString& errorMessage) const;

	/**
		Check that this server issues tokens to the product: one of its clients, not this server itself.
	*/
	bool CheckClientProduct(const QByteArray& clientProductId, QString& errorMessage) const;

	/**
		The permissions every token for this server holds.
	*/
	QByteArrayList GetRequiredPermissions() const;

	/**
		The server token of the id. Personal tokens of users are not server tokens.
	*/
	imtauth::IPersonalAccessTokenSharedPtr GetServerToken(const QByteArray& tokenId, QString& errorMessage) const;

	/**
		Check the permissions of a token and complete them: a group stands for its permissions,
		then the ones every token holds and the ones they require are added.
	*/
	bool NormalizeScopes(QByteArrayList& scopes, QString& errorMessage) const;

	/**
		The tree a token is given from: this server with the roots of the permissions a token may hold.
	*/
	void BuildTokenTree(imtlic::CProductInfo& tree) const;

	/**
		Calls the visitor for each root of the permission tree of this server.
		\return false if this server has no permission tree.
	*/
	bool VisitTreeFeatures(const std::function<void(const QByteArray&, const imtlic::IFeatureInfo&)>& visitor) const;

private:
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_REF(imtauth::IClientProductCatalog, m_clientProductCatalogCompPtr);
	I_REF(imtlic::IProductInfo, m_permissionTreeCompPtr);
	I_REF(imtlic::IFeatureInfoProvider, m_serverPermissionsCompPtr);
	I_MULTIATTR(QByteArray, m_requiredPermissionsAttrPtr);
};


} // namespace imtauthgql


