// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteServerTokenIssuerComp.h>


// ImtCore includes
#include <imtbase/CUrlParam.h>
#include <imtgql/CGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


// public methods

// reimplemented (IServerTokenIssuer)

QByteArray CRemoteServerTokenIssuerComp::SignIn(const ServerInfo& server, const QString& login, const QString& password, QString& errorMessage) const
{
	sdl::V1_0::imtauth::AuthorizationRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->login = login;
	arguments.input->password = password;
	arguments.input->productId = server.productId;

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtauth::CAuthorizationGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	const sdl::V1_0::imtauth::CAuthorizationPayload payload = Send<sdl::V1_0::imtauth::CAuthorizationPayload>(server, QByteArray(), gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArray();
	}

	if (!payload.token || payload.token->isEmpty()){
		errorMessage = QStringLiteral("The server '%1' did not sign the user in").arg(QString::fromUtf8(server.productId));

		return QByteArray();
	}

	return *payload.token;
}


bool CRemoteServerTokenIssuerComp::GetPermissions(
			const ServerInfo& server,
			const QByteArray& sessionToken,
			const QByteArray& clientProductId,
			sdl::V1_0::imtauth::CServerTokenReceiver& permissions,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetServerTokenReceiversRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->clientProductId = clientProductId;

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtauth::CGetServerTokenReceiversGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	const sdl::V1_0::imtauth::CServerTokenReceiverList payload = Send<sdl::V1_0::imtauth::CServerTokenReceiverList>(server, sessionToken, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (payload.productServers){
		for (const sdl::V1_0::imtauth::CServerTokenReceiver& receiver : payload.productServers->ToList()){
			if (receiver.productId && *receiver.productId == server.productId){
				permissions = receiver;

				return true;
			}
		}
	}

	errorMessage = QStringLiteral("The server '%1' issues no tokens for '%2'").arg(QString::fromUtf8(server.productId), QString::fromUtf8(clientProductId));

	return false;
}


bool CRemoteServerTokenIssuerComp::IssueToken(
			const ServerInfo& server,
			const QByteArray& sessionToken,
			const QByteArray& clientProductId,
			const QString& name,
			QByteArrayList& scopes,
			QByteArray& tokenId,
			QByteArray& token,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CreateServerTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->clientProductId = clientProductId;
	arguments.input->productId = server.productId;
	arguments.input->name = name;
	arguments.input->scopes.Emplace().FromList(scopes);

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtauth::CCreateServerTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	const sdl::V1_0::imtauth::CCreateServerTokenPayload payload = Send<sdl::V1_0::imtauth::CCreateServerTokenPayload>(server, sessionToken, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!payload.success || !*payload.success || !payload.id || !payload.token){
		errorMessage = payload.message ? QString(*payload.message) : QStringLiteral("The server '%1' did not issue the token").arg(QString::fromUtf8(server.productId));

		return false;
	}

	tokenId = *payload.id;
	token = *payload.token;

	return GetTokenScopes(server, sessionToken, clientProductId, tokenId, scopes, errorMessage);
}


bool CRemoteServerTokenIssuerComp::SetTokenScopes(
			const ServerInfo& server,
			const QByteArray& sessionToken,
			const QByteArray& clientProductId,
			const QByteArray& tokenId,
			QByteArrayList& scopes,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::UpdateServerTokenScopesRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = tokenId;
	arguments.input->scopes.Emplace().FromList(scopes);

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtauth::CUpdateServerTokenScopesGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	const sdl::V1_0::imtauth::CRevokeServerTokenPayload payload = Send<sdl::V1_0::imtauth::CRevokeServerTokenPayload>(server, sessionToken, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!payload.success || !*payload.success){
		errorMessage = payload.message ? QString(*payload.message) : QStringLiteral("The server '%1' did not change the token").arg(QString::fromUtf8(server.productId));

		return false;
	}

	return GetTokenScopes(server, sessionToken, clientProductId, tokenId, scopes, errorMessage);
}


bool CRemoteServerTokenIssuerComp::RevokeToken(const ServerInfo& server, const QByteArray& sessionToken, const QByteArray& tokenId, QString& errorMessage) const
{
	sdl::V1_0::imtauth::RevokeServerTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = tokenId;

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtauth::CRevokeServerTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	const sdl::V1_0::imtauth::CRevokeServerTokenPayload payload = Send<sdl::V1_0::imtauth::CRevokeServerTokenPayload>(server, sessionToken, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!payload.success || !*payload.success){
		errorMessage = payload.message ? QString(*payload.message) : QStringLiteral("The server '%1' did not revoke the token").arg(QString::fromUtf8(server.productId));

		return false;
	}

	return true;
}


// private methods

template<class Payload>
Payload CRemoteServerTokenIssuerComp::Send(const ServerInfo& server, const QByteArray& sessionToken, imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	errorMessage.clear();

	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	contextPtr->SetProductId(server.productId);
	contextPtr->SetToken(sessionToken);
	gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (!requestPtr.IsValid()){
		errorMessage = QStringLiteral("Request is invalid");

		return Payload();
	}

	imtbase::CUrlParam urlParam;
	urlParam.SetUrl(server.url);

	const imtclientgql::IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr, &urlParam);
	if (!responsePtr.IsValid()){
		errorMessage = QStringLiteral("The server '%1' at %2 did not answer").arg(QString::fromUtf8(server.productId), server.url.toString());

		return Payload();
	}

	return imtclientgql::CClientRequestModelHelpers::ParseModelResponse<Payload>(responsePtr->GetResponseData(), gqlRequest.GetCommandId(), errorMessage);
}


bool CRemoteServerTokenIssuerComp::GetTokenScopes(
			const ServerInfo& server,
			const QByteArray& sessionToken,
			const QByteArray& clientProductId,
			const QByteArray& tokenId,
			QByteArrayList& scopes,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetServerTokenListRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->clientProductId = clientProductId;

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtauth::CGetServerTokenListGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	const sdl::V1_0::imtauth::CServerTokenList payload = Send<sdl::V1_0::imtauth::CServerTokenList>(server, sessionToken, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (payload.tokens){
		for (const sdl::V1_0::imtauth::CServerToken& serverToken : payload.tokens->ToList()){
			if (serverToken.id && *serverToken.id == tokenId){
				scopes = serverToken.scopes ? serverToken.scopes->ToList() : QByteArrayList();

				return true;
			}
		}
	}

	errorMessage = QStringLiteral("The server '%1' has no token '%2'").arg(QString::fromUtf8(server.productId), QString::fromUtf8(tokenId));

	return false;
}


} // namespace imtauthgql


