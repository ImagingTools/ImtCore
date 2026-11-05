// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CServerTokenControllerComp.h>


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>
#include <imtauth/imtauth.h>
#include <imtgql/IGqlContext.h>

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
	const QByteArrayList scopes = arguments.input->scopes ? arguments.input->scopes->ToList() : QByteArrayList();

	if (clientProductId.isEmpty() || productId.isEmpty() || name.isEmpty()){
		errorMessage = QStringLiteral("The client product, the receiving product and the name are required");

		return response;
	}

	if (clientProductId == productId){
		errorMessage = QStringLiteral("A server token is issued for another server");

		return response;
	}

	const QByteArrayList serverPermissions = imtauth::GetServerTokenPermissions();
	for (const QByteArray& scope : scopes){
		if (!serverPermissions.contains(scope)){
			errorMessage = QStringLiteral("The permission '%1' cannot be given to a server").arg(QString::fromUtf8(scope));

			return response;
		}
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


// private methods

bool CServerTokenControllerComp::CheckSuperuserSession(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr != nullptr ? gqlContextPtr->GetUserInfo() : nullptr;

	// A personal token of the superuser must not issue identities of servers
	if (userInfoPtr == nullptr || !userInfoPtr->IsAdmin() || gqlContextPtr->GetToken().startsWith(QByteArrayLiteral("imt_pat_"))){
		errorMessage = QStringLiteral("Server tokens are managed by a superuser session");

		return false;
	}

	return true;
}


} // namespace imtauthgql
