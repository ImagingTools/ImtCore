// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteServerTokenIssuerComp.h>


// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtbase/CUrlParam.h>
#include <imtgql/CGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


// public methods

// reimplemented (IServerTokenIssuer)

QByteArray CRemoteServerTokenIssuerComp::SignInSuperuser(const ServerInfo& server, const QString& password, QString& errorMessage) const
{
	sdl::V1_0::imtauth::AuthorizationRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->login = QString::fromUtf8(imtauth::s_superuserLogin);
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


bool CRemoteServerTokenIssuerComp::AddClient(
			const ServerInfo& server,
			const QByteArray& sessionToken,
			const QByteArray& clientProductId,
			const QString& name,
			bool& isAdded,
			QString& errorMessage) const
{
	isAdded = false;

	sdl::V1_0::imtauth::AddServerTokenClientRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->clientProductId = clientProductId;
	arguments.input->name = name;

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtauth::CAddServerTokenClientGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	const sdl::V1_0::imtauth::CAddServerTokenClientPayload payload = Send<sdl::V1_0::imtauth::CAddServerTokenClientPayload>(server, sessionToken, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!payload.success || !*payload.success){
		errorMessage = QStringLiteral("The server '%1' did not add the product '%2'").arg(QString::fromUtf8(server.productId), QString::fromUtf8(clientProductId));

		return false;
	}

	isAdded = payload.added && *payload.added;

	return true;
}


bool CRemoteServerTokenIssuerComp::GetPermissions(
			const ServerInfo& server,
			const QByteArray& sessionToken,
			const QByteArray& clientProductId,
			sdl::V1_0::imtauth::CServerTokenReceiver& permissions,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetServerTokenReceiverRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->clientProductId = clientProductId;

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtauth::CGetServerTokenReceiverGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	permissions = Send<sdl::V1_0::imtauth::CServerTokenReceiver>(server, sessionToken, gqlRequest, errorMessage);

	return errorMessage.isEmpty();
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
		errorMessage = QStringLiteral("The server '%1' did not issue the token").arg(QString::fromUtf8(server.productId));

		return false;
	}

	tokenId = *payload.id;
	token = *payload.token;
	scopes = payload.scopes ? payload.scopes->ToList() : QByteArrayList();

	return true;
}


bool CRemoteServerTokenIssuerComp::SetTokenScopes(
			const ServerInfo& server,
			const QByteArray& sessionToken,
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

	const sdl::V1_0::imtauth::CUpdateServerTokenScopesPayload payload = Send<sdl::V1_0::imtauth::CUpdateServerTokenScopesPayload>(server, sessionToken, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!payload.success || !*payload.success){
		errorMessage = QStringLiteral("The server '%1' did not change the token").arg(QString::fromUtf8(server.productId));

		return false;
	}

	scopes = payload.scopes ? payload.scopes->ToList() : QByteArrayList();

	return true;
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
		errorMessage = QStringLiteral("The server '%1' did not revoke the token").arg(QString::fromUtf8(server.productId));

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


} // namespace imtauthgql


