// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CServerTokenControllerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QSet>

// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>
#include <imtauth/imtauth.h>
#include <imtauthgql/imtauthgql.h>
#include <imtgql/IGqlContext.h>
#include <imtlic/CProductInfo.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ServerTokens.h>


namespace imtauthgql
{


namespace
{


// Copy a feature with its sub-features
void CopyFeature(const imtlic::IFeatureInfo& featureInfo, sdl::V1_0::imtauth::CServerFeature& feature)
{
	feature.featureId = featureInfo.GetFeatureId();
	feature.name = featureInfo.GetFeatureName();
	feature.description = featureInfo.GetFeatureDescription();
	feature.requirements.Emplace().FromList(featureInfo.GetRequirements());

	imtsdl::TElementList<sdl::V1_0::imtauth::CServerFeature> subFeatures;
	for (const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr : featureInfo.GetSubFeatures()){
		if (subFeaturePtr.IsValid()){
			sdl::V1_0::imtauth::CServerFeature subFeature;
			CopyFeature(*subFeaturePtr, subFeature);
			subFeatures.append(subFeature);
		}
	}

	feature.subFeatures = subFeatures;
}


} // namespace


// protected methods

// reimplemented (sdl::V1_0::imtauth::CServerTokensGqlHandlerCompBase)

sdl::V1_0::imtauth::CServerTokenList CServerTokenControllerComp::OnGetServerTokenList(
			const sdl::V1_0::imtauth::CGetServerTokenListGqlRequest& getServerTokenListRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CServerTokenList response;

	if (!CheckSuperuserSession(gqlRequest, errorMessage)){
		return response;
	}

	const auto arguments = getServerTokenListRequest.GetRequestedArguments();
	const QByteArray clientProductId = arguments.input.has_value() && arguments.input->clientProductId ? QByteArray(*arguments.input->clientProductId) : QByteArray();

	bool isRead = false;
	const QByteArrayList tokenIds = m_tokenManagerCompPtr->GetServerTokenIds(clientProductId, &isRead);
	if (!isRead){
		errorMessage = QStringLiteral("The server tokens could not be read");

		return response;
	}

	auto& tokens = response.tokens.Emplace();
	for (const QByteArray& tokenId : tokenIds){
		const imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
		if (!tokenPtr.IsValid()){
			errorMessage = QStringLiteral("Unable to read the server token '%1'").arg(QString::fromUtf8(tokenId));

			return sdl::V1_0::imtauth::CServerTokenList();
		}

		sdl::V1_0::imtauth::CServerToken token;
		token.id = tokenPtr->GetId();
		token.clientProductId = tokenPtr->GetClientProductId();
		token.productId = tokenPtr->GetProductId();
		token.name = tokenPtr->GetName();
		token.description = tokenPtr->GetDescription();
		token.scopes.Emplace().FromList(tokenPtr->GetScopes());
		token.createdAt = tokenPtr->GetCreatedAt().toString(Qt::ISODate);
		token.lastUsedAt = tokenPtr->GetLastUsedAt().toString(Qt::ISODate);
		token.revoked = tokenPtr->IsRevoked();
		tokens.append(token);
	}

	return response;
}


sdl::V1_0::imtauth::CCreateServerTokenPayload CServerTokenControllerComp::OnCreateServerToken(
			const sdl::V1_0::imtauth::CCreateServerTokenGqlRequest& createServerTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CCreateServerTokenPayload response;
	response.success = false;

	if (!CheckSuperuserSession(gqlRequest, errorMessage)){
		return response;
	}

	const auto arguments = createServerTokenRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		errorMessage = QStringLiteral("The input is missing");

		return response;
	}

	const QByteArray clientProductId = arguments.input->clientProductId ? QByteArray(*arguments.input->clientProductId) : QByteArray();
	const QString name = arguments.input->name ? QString(*arguments.input->name) : QString();
	const QString description = arguments.input->description ? QString(*arguments.input->description) : QString();
	QByteArrayList scopes = arguments.input->scopes ? arguments.input->scopes->ToList() : QByteArrayList();

	if (name.isEmpty()){
		errorMessage = QStringLiteral("The name of the token is required");

		return response;
	}

	if (!CheckClientProduct(clientProductId, errorMessage) || !NormalizeScopes(scopes, errorMessage)){
		return response;
	}

	// No expiration: it would stop a working server
	const imtauth::IPersonalAccessTokenManager::TokenCreationResult result = m_tokenManagerCompPtr->CreateToken(
				QByteArray(), GetOwnProductId(), name, description, scopes, QDateTime(), clientProductId);
	if (!result.success){
		errorMessage = QStringLiteral("Unable to create the server token");

		return response;
	}

	response.success = true;
	response.id = result.tokenId;
	response.token = result.rawToken;
	response.scopes.Emplace().FromList(scopes);

	return response;
}


sdl::V1_0::imtauth::CRevokeServerTokenPayload CServerTokenControllerComp::OnRevokeServerToken(
			const sdl::V1_0::imtauth::CRevokeServerTokenGqlRequest& revokeServerTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CRevokeServerTokenPayload response;
	response.success = false;

	if (!CheckSuperuserSession(gqlRequest, errorMessage)){
		return response;
	}

	const auto arguments = revokeServerTokenRequest.GetRequestedArguments();
	const QByteArray tokenId = arguments.input.has_value() && arguments.input->id ? QByteArray(*arguments.input->id) : QByteArray();

	if (!GetServerToken(tokenId, errorMessage).IsValid()){
		return response;
	}

	response.success = m_tokenManagerCompPtr->RevokeToken(tokenId);
	if (!*response.success){
		errorMessage = QStringLiteral("Unable to revoke the server token");
	}

	return response;
}


sdl::V1_0::imtauth::CDeleteServerTokenPayload CServerTokenControllerComp::OnDeleteServerToken(
			const sdl::V1_0::imtauth::CDeleteServerTokenGqlRequest& deleteServerTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CDeleteServerTokenPayload response;
	response.success = false;

	if (!CheckSuperuserSession(gqlRequest, errorMessage)){
		return response;
	}

	const auto arguments = deleteServerTokenRequest.GetRequestedArguments();
	const QByteArray tokenId = arguments.input.has_value() && arguments.input->id ? QByteArray(*arguments.input->id) : QByteArray();

	const imtauth::IPersonalAccessTokenSharedPtr tokenPtr = GetServerToken(tokenId, errorMessage);
	if (!tokenPtr.IsValid()){
		return response;
	}

	// A working server is stopped by the revocation, which its subscriptions see
	if (!tokenPtr->IsRevoked()){
		errorMessage = QStringLiteral("Revoke the server token before deleting it");

		return response;
	}

	response.success = m_tokenManagerCompPtr->DeleteToken(tokenId);
	if (!*response.success){
		errorMessage = QStringLiteral("Unable to delete the server token");
	}

	return response;
}


sdl::V1_0::imtauth::CAddServerTokenClientPayload CServerTokenControllerComp::OnAddServerTokenClient(
			const sdl::V1_0::imtauth::CAddServerTokenClientGqlRequest& addServerTokenClientRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CAddServerTokenClientPayload response;
	response.success = false;

	if (!CheckSuperuserSession(gqlRequest, errorMessage)){
		return response;
	}

	const auto arguments = addServerTokenClientRequest.GetRequestedArguments();
	const QByteArray clientProductId = arguments.input.has_value() && arguments.input->clientProductId ? QByteArray(*arguments.input->clientProductId) : QByteArray();
	const QString name = arguments.input.has_value() && arguments.input->name ? QString(*arguments.input->name) : QString();
	if (!CheckAnotherProduct(clientProductId, errorMessage)){
		return response;
	}

	bool isAdded = false;
	response.success = m_clientProductCatalogCompPtr->AddClientProduct(clientProductId, name, errorMessage, &isAdded);
	response.added = isAdded;

	return response;
}


sdl::V1_0::imtauth::CUpdateServerTokenScopesPayload CServerTokenControllerComp::OnUpdateServerTokenScopes(
			const sdl::V1_0::imtauth::CUpdateServerTokenScopesGqlRequest& updateServerTokenScopesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CUpdateServerTokenScopesPayload response;
	response.success = false;

	if (!CheckSuperuserSession(gqlRequest, errorMessage)){
		return response;
	}

	const auto arguments = updateServerTokenScopesRequest.GetRequestedArguments();
	const QByteArray tokenId = arguments.input.has_value() && arguments.input->id ? QByteArray(*arguments.input->id) : QByteArray();
	QByteArrayList scopes = arguments.input.has_value() && arguments.input->scopes ? arguments.input->scopes->ToList() : QByteArrayList();

	const imtauth::IPersonalAccessTokenSharedPtr tokenPtr = GetServerToken(tokenId, errorMessage);
	if (!tokenPtr.IsValid()){
		return response;
	}

	if (tokenPtr->IsRevoked()){
		errorMessage = QStringLiteral("The server token is revoked");

		return response;
	}

	if (!NormalizeScopes(scopes, errorMessage)){
		return response;
	}

	response.success = m_tokenManagerCompPtr->SetTokenScopes(tokenId, scopes);
	if (!*response.success){
		errorMessage = QStringLiteral("Unable to change the permissions of the server token");

		return response;
	}

	response.scopes.Emplace().FromList(scopes);

	return response;
}


sdl::V1_0::imtauth::CServerTokenReceiver CServerTokenControllerComp::OnGetServerTokenReceiver(
			const sdl::V1_0::imtauth::CGetServerTokenReceiverGqlRequest& getServerTokenReceiverRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CServerTokenReceiver response;

	const auto arguments = getServerTokenReceiverRequest.GetRequestedArguments();
	const QByteArray clientProductId = arguments.input.has_value() && arguments.input->clientProductId ? QByteArray(*arguments.input->clientProductId) : QByteArray();
	if (!CheckSuperuserSession(gqlRequest, errorMessage) || !CheckClientProduct(clientProductId, errorMessage)){
		return response;
	}

	imtlic::CProductInfo tree;
	BuildTokenTree(tree);

	imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionGroup> groups;
	BuildPermissionGroups<sdl::V1_0::imtauth::CPermissionGroup, sdl::V1_0::imtauth::CPermissionEntry>(&tree, nullptr, groups, QByteArray(), nullptr);

	response.productId = GetOwnProductId();
	response.groups = groups;

	return response;
}


sdl::V1_0::imtauth::CServerFeatureList CServerTokenControllerComp::OnGetServerTokenFeatures(
			const sdl::V1_0::imtauth::CGetServerTokenFeaturesGqlRequest& /*getServerTokenFeaturesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CServerFeatureList response;

	// The tree goes to the roles of a connected product; what reaches this server is bound by the token of the product
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr || gqlContextPtr->GetServerProductId().isEmpty()){
		errorMessage = QStringLiteral("The permissions of this server are given to the servers of products");

		return response;
	}

	auto& features = response.features.Emplace();
	const bool hasTree = VisitTreeFeatures([&features](const QByteArray& /*featureId*/, const imtlic::IFeatureInfo& featureInfo){
		sdl::V1_0::imtauth::CServerFeature feature;
		CopyFeature(featureInfo, feature);
		features.append(feature);
	});
	if (!hasTree){
		errorMessage = QStringLiteral("This server has no permission tree");

		return sdl::V1_0::imtauth::CServerFeatureList();
	}

	return response;
}


// private methods

bool CServerTokenControllerComp::CheckSuperuserSession(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!IsSuperuserSessionRequest(gqlRequest)){
		errorMessage = QStringLiteral("Server tokens are managed by a superuser session");

		return false;
	}

	return true;
}


QByteArray CServerTokenControllerComp::GetOwnProductId() const
{
	return m_permissionTreeCompPtr->GetProductId();
}


bool CServerTokenControllerComp::CheckAnotherProduct(const QByteArray& clientProductId, QString& errorMessage) const
{
	if (clientProductId.isEmpty() || clientProductId == GetOwnProductId()){
		errorMessage = QStringLiteral("A server token is issued to the server of another product");

		return false;
	}

	return true;
}


bool CServerTokenControllerComp::CheckClientProduct(const QByteArray& clientProductId, QString& errorMessage) const
{
	if (!CheckAnotherProduct(clientProductId, errorMessage)){
		return false;
	}

	bool isRead = true;
	if (!m_clientProductCatalogCompPtr->IsClientProduct(clientProductId, &isRead)){
		errorMessage = isRead
					? QStringLiteral("The product '%1' is not a client of this server").arg(QString::fromUtf8(clientProductId))
					: QStringLiteral("The client products of this server could not be read");

		return false;
	}

	return true;
}


imtauth::IPersonalAccessTokenSharedPtr CServerTokenControllerComp::GetServerToken(const QByteArray& tokenId, QString& errorMessage) const
{
	// Personal tokens of users are not touched here
	const imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
	if (!tokenPtr.IsValid() || tokenPtr->GetClientProductId().isEmpty()){
		errorMessage = QStringLiteral("The server token does not exist");

		return imtauth::IPersonalAccessTokenSharedPtr();
	}

	return tokenPtr;
}


bool CServerTokenControllerComp::NormalizeScopes(QByteArrayList& scopes, QString& errorMessage) const
{
	imtlic::CProductInfo tree;
	BuildTokenTree(tree);

	// Only the permissions of the tree are held, in its order
	const QByteArrayList catalog = imtlic::CollectProductPermissions(tree);

	const QByteArrayList requested = scopes;

	// A group stands for the permissions in it
	QByteArrayList held;
	for (const QByteArray& permissionId : requested){
		bool isKnown = false;
		for (const QByteArray& catalogId : catalog){
			if (imtauth::HasPermission(QByteArrayList{catalogId}, permissionId)){
				held.append(catalogId);
				isKnown = true;
			}
		}

		if (!isKnown){
			errorMessage = QStringLiteral("The permission '%1' cannot be given to a server token").arg(QString::fromUtf8(permissionId));

			return false;
		}
	}

	held += imtlic::CollectImpliedPermissions(tree, held);

	const QSet<QByteArray> heldSet(held.cbegin(), held.cend());
	scopes.clear();
	for (const QByteArray& catalogId : catalog){
		if (heldSet.contains(catalogId)){
			scopes.append(catalogId);
		}
	}

	return true;
}


void CServerTokenControllerComp::BuildTokenTree(imtlic::CProductInfo& tree) const
{
	// The tree resolves the requirements
	tree.SetProductId(GetOwnProductId());

	// A product takes only an identifiable feature at its root
	auto addFeature = [&tree](const QByteArray& featureId, const imtlic::IFeatureInfo& sourceInfo){
		imtlic::CIdentifiableFeatureInfo featureInfo;
		if (featureInfo.imtlic::CFeatureInfo::CopyFrom(sourceInfo)){
			featureInfo.SetObjectUuid(featureId);
			tree.AddFeature(featureId, featureInfo);
		}
	};

	VisitTreeFeatures(addFeature);
}


bool CServerTokenControllerComp::VisitTreeFeatures(const std::function<void(const QByteArray&, const imtlic::IFeatureInfo&)>& visitor) const
{
	imtbase::IObjectCollection* featureCollectionPtr = m_permissionTreeCompPtr->GetFeatures();
	if (featureCollectionPtr == nullptr){
		return false;
	}

	for (const imtbase::ICollectionInfo::Id& featureId : featureCollectionPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtlic::IFeatureInfo* featureInfoPtr = featureCollectionPtr->GetObjectData(featureId, dataPtr)
					? dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr())
					: nullptr;
		if (featureInfoPtr != nullptr){
			visitor(featureId, *featureInfoPtr);
		}
	}

	return true;
}


} // namespace imtauthgql
