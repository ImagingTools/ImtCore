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

	auto& tokens = response.tokens.Emplace();
	for (const QByteArray& tokenId : m_tokenManagerCompPtr->GetServerTokenIds(clientProductId)){
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
	const QByteArray productId = arguments.input->productId ? QByteArray(*arguments.input->productId) : QByteArray();
	const QString name = arguments.input->name ? QString(*arguments.input->name) : QString();
	const QString description = arguments.input->description ? QString(*arguments.input->description) : QString();
	QByteArrayList scopes = arguments.input->scopes ? arguments.input->scopes->ToList() : QByteArrayList();

	if (clientProductId.isEmpty() || productId.isEmpty() || name.isEmpty()){
		errorMessage = QStringLiteral("The client product, the receiving product and the name are required");

		return response;
	}

	if (clientProductId == productId){
		errorMessage = QStringLiteral("A server token is issued for another server");

		return response;
	}

	bool isRead = true;
	if (!m_clientProductCatalogCompPtr->IsClientProduct(clientProductId, &isRead)){
		errorMessage = isRead
					? QStringLiteral("The product '%1' is not a client of this server").arg(QString::fromUtf8(clientProductId))
					: QStringLiteral("The client products of this server could not be read");

		return response;
	}

	if (!NormalizeScopes(productId, scopes, errorMessage)){
		return response;
	}

	// No expiration: it would stop a working server
	const imtauth::IPersonalAccessTokenManager::TokenCreationResult result = m_tokenManagerCompPtr->CreateToken(
				QByteArray(), productId, name, description, scopes, QDateTime(), clientProductId);
	if (!result.success){
		errorMessage = QStringLiteral("Unable to create the server token");

		return response;
	}

	response.success = true;
	response.id = result.tokenId;
	response.token = result.rawToken;

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

	// Personal tokens of users are not revoked here
	const imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
	if (!tokenPtr.IsValid() || tokenPtr->GetClientProductId().isEmpty()){
		errorMessage = QStringLiteral("The server token does not exist");

		return response;
	}

	response.success = m_tokenManagerCompPtr->RevokeToken(tokenId);
	if (!*response.success){
		errorMessage = QStringLiteral("Unable to revoke the server token");
	}

	return response;
}


sdl::V1_0::imtauth::CRevokeServerTokenPayload CServerTokenControllerComp::OnUpdateServerTokenScopes(
			const sdl::V1_0::imtauth::CUpdateServerTokenScopesGqlRequest& updateServerTokenScopesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CRevokeServerTokenPayload response;
	response.success = false;

	if (!CheckSuperuserSession(gqlRequest, errorMessage)){
		return response;
	}

	const auto arguments = updateServerTokenScopesRequest.GetRequestedArguments();
	const QByteArray tokenId = arguments.input.has_value() && arguments.input->id ? QByteArray(*arguments.input->id) : QByteArray();
	QByteArrayList scopes = arguments.input.has_value() && arguments.input->scopes ? arguments.input->scopes->ToList() : QByteArrayList();

	// Personal tokens of users are not changed here
	const imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
	if (!tokenPtr.IsValid() || tokenPtr->GetClientProductId().isEmpty()){
		errorMessage = QStringLiteral("The server token does not exist");

		return response;
	}

	if (tokenPtr->IsRevoked()){
		errorMessage = QStringLiteral("The server token is revoked");

		return response;
	}

	if (!NormalizeScopes(tokenPtr->GetProductId(), scopes, errorMessage)){
		return response;
	}

	response.success = m_tokenManagerCompPtr->SetTokenScopes(tokenId, scopes);
	if (!*response.success){
		errorMessage = QStringLiteral("Unable to change the permissions of the server token");
	}

	return response;
}


sdl::V1_0::imtauth::CServerTokenReceiverList CServerTokenControllerComp::OnGetServerTokenReceivers(
			const sdl::V1_0::imtauth::CGetServerTokenReceiversGqlRequest& getServerTokenReceiversRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CServerTokenReceiverList response;

	if (!CheckSuperuserSession(gqlRequest, errorMessage)){
		return response;
	}

	// A server takes no token of its own
	const auto arguments = getServerTokenReceiversRequest.GetRequestedArguments();
	const QByteArray clientProductId = arguments.input.has_value() && arguments.input->clientProductId ? QByteArray(*arguments.input->clientProductId) : QByteArray();

	// This server issues tokens for itself only: another server manages its own connections
	auto& receivers = response.productServers.Emplace();
	if (clientProductId == GetOwnProductId()){
		return response;
	}

	sdl::V1_0::imtauth::CServerTokenReceiver receiver;
	receiver.productId = GetOwnProductId();
	if (!GetReceiverGroups(GetOwnProductId(), receiver.groups.Emplace(), errorMessage)){
		return sdl::V1_0::imtauth::CServerTokenReceiverList();
	}

	QByteArrayList requiredPermissions;
	for (int i = 0; i < m_requiredPermissionsAttrPtr.GetCount(); i++){
		requiredPermissions.append(m_requiredPermissionsAttrPtr[i]);
	}
	receiver.requiredPermissions.Emplace().FromList(requiredPermissions);

	receivers.append(receiver);

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

	imtbase::IObjectCollection* featureCollectionPtr = m_permissionTreeCompPtr->GetFeatures();
	if (featureCollectionPtr == nullptr){
		errorMessage = QStringLiteral("This server has no permission tree");

		return response;
	}

	auto& features = response.features.Emplace();
	for (const imtbase::ICollectionInfo::Id& elementId : featureCollectionPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtlic::IFeatureInfo* featureInfoPtr = featureCollectionPtr->GetObjectData(elementId, dataPtr)
					? dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr())
					: nullptr;
		if (featureInfoPtr != nullptr){
			sdl::V1_0::imtauth::CServerFeature feature;
			CopyFeature<sdl::V1_0::imtauth::CServerFeature>(*featureInfoPtr, feature);
			features.append(feature);
		}
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


bool CServerTokenControllerComp::GetReceiverGroups(
			const QByteArray& receiverProductId,
			imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionGroup>& groups,
			QString& errorMessage) const
{
	groups = imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionGroup>();

	if (receiverProductId != GetOwnProductId()){
		errorMessage = QStringLiteral("This server issues tokens for itself only: '%1' manages its own connections").arg(QString::fromUtf8(receiverProductId));

		return false;
	}

	// The tree resolves the requirements
	imtlic::CProductInfo tree;
	tree.SetProductId(receiverProductId);
	AddTokenFeatures(tree);

	BuildPermissionGroups<sdl::V1_0::imtauth::CPermissionGroup, sdl::V1_0::imtauth::CPermissionEntry>(&tree, nullptr, groups, QByteArray(), nullptr);

	return true;
}


bool CServerTokenControllerComp::NormalizeScopes(const QByteArray& receiverProductId, QByteArrayList& scopes, QString& errorMessage) const
{
	imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionGroup> groups;
	if (!GetReceiverGroups(receiverProductId, groups, errorMessage)){
		return false;
	}

	// Only the permissions of the tree are held, in its order
	QByteArrayList catalog;
	imtlic::PermissionRequirements requirements;
	for (const sdl::V1_0::imtauth::CPermissionGroup& group : groups.ToList()){
		if (!group.entries){
			continue;
		}

		for (const sdl::V1_0::imtauth::CPermissionEntry& entry : group.entries->ToList()){
			if (!entry.permissionId){
				continue;
			}

			catalog.append(*entry.permissionId);
			if (entry.requirements){
				requirements.insert(*entry.permissionId, entry.requirements->ToList());
			}
		}
	}

	QByteArrayList requested = scopes;
	for (int i = 0; i < m_requiredPermissionsAttrPtr.GetCount(); i++){
		requested.append(m_requiredPermissionsAttrPtr[i]);
	}

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
			errorMessage = QStringLiteral("The permission '%1' cannot be given to a server of '%2'").arg(QString::fromUtf8(permissionId), QString::fromUtf8(receiverProductId));

			return false;
		}
	}

	held += imtlic::CollectImpliedPermissions(requirements, held);

	const QSet<QByteArray> heldSet(held.cbegin(), held.cend());
	scopes.clear();
	for (const QByteArray& catalogId : catalog){
		if (heldSet.contains(catalogId)){
			scopes.append(catalogId);
		}
	}

	return true;
}


void CServerTokenControllerComp::AddTokenFeatures(imtlic::IProductInfo& tree) const
{
	// A product takes only an identifiable feature at its root
	auto addFeature = [&tree](const QByteArray& featureId, const imtlic::IFeatureInfo& sourceInfo){
		imtlic::CIdentifiableFeatureInfo featureInfo;
		if (featureInfo.imtlic::CFeatureInfo::CopyFrom(sourceInfo)){
			featureInfo.SetObjectUuid(featureId);
			tree.AddFeature(featureId, featureInfo);
		}
	};

	if (m_serverPermissionsCompPtr.IsValid()){
		for (const imtbase::ICollectionInfo::Id& featureId : m_serverPermissionsCompPtr->GetFeatureList().GetElementIds()){
			const imtlic::IFeatureInfoSharedPtr featureInfoPtr = m_serverPermissionsCompPtr->GetFeatureInfo(featureId);
			if (featureInfoPtr.IsValid()){
				addFeature(featureId, *featureInfoPtr);
			}
		}

		return;
	}

	imtbase::IObjectCollection* featureCollectionPtr = m_permissionTreeCompPtr->GetFeatures();
	if (featureCollectionPtr == nullptr){
		return;
	}

	for (const imtbase::ICollectionInfo::Id& featureId : featureCollectionPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtlic::IFeatureInfo* featureInfoPtr = featureCollectionPtr->GetObjectData(featureId, dataPtr)
					? dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr())
					: nullptr;
		if (featureInfoPtr != nullptr){
			addFeature(featureId, *featureInfoPtr);
		}
	}
}


} // namespace imtauthgql
