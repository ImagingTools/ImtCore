// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestPersonalAccessTokenManagerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens.h>


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtauthgql
{


namespace
{


void AttachCurrentContext(imtgql::CGqlRequest& gqlRequest)
{
	imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	istd::IChangeableUniquePtr clonedPtr = gqlContextPtr->CloneMe();
	imtgql::IGqlContextUniquePtr castedPtr;
	castedPtr.MoveCastedPtr(std::move(clonedPtr));
	gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(castedPtr));
}


} // anonymous namespace


// reimplemented (imtauth::IPersonalAccessTokenManager)

imtauth::IPersonalAccessTokenManager::TokenCreationResult CClientRequestPersonalAccessTokenManagerComp::CreateToken(
			const QByteArray& userId,
			const QByteArray& productId,
			const QString& name,
			const QString& description,
			const QByteArrayList& scopes,
			const QDateTime& expiresAt)
{
	namespace tokensdl = sdl::V1_0::imtauth;

	imtauth::IPersonalAccessTokenManager::TokenCreationResult result;
	result.success = false;

	tokensdl::CreateTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->userId = userId;
	arguments.input->name = name;

	if (!productId.isEmpty()){
		arguments.input->productId = productId;
	}

	if (!description.isEmpty()){
		arguments.input->description = description;
	}

	if (!scopes.isEmpty()){
		arguments.input->scopes.Emplace();
		arguments.input->scopes->FromList(scopes);
	}

	if (expiresAt.isValid()){
		arguments.input->expiresAt = expiresAt.toUTC().toString(Qt::ISODate);
	}

	imtgql::CGqlRequest gqlRequest;
	AttachCurrentContext(gqlRequest);
	if (!tokensdl::CCreateTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return result;
	}

	tokensdl::CCreateTokenGqlRequest createTokenRequest(gqlRequest, false);

	QString errorMessage;
	tokensdl::CCreateTokenPayload payload = OnCreateToken(createTokenRequest, gqlRequest, errorMessage);
	if (!payload.success.has_value() || !*payload.success){
		return result;
	}

	if (!payload.id.has_value() || !payload.token.has_value()){
		return result;
	}

	// Notify observers that the manager state has changed
	istd::CChangeNotifier changeNotifier(this);

	result.success = true;
	result.tokenId = *payload.id;
	result.rawToken = *payload.token;

	return result;
}


bool CClientRequestPersonalAccessTokenManagerComp::ValidateToken(const QByteArray& rawToken, QByteArray& userId, QByteArray& tokenId, QByteArrayList& scopes) const
{
	namespace tokensdl = sdl::V1_0::imtauth;

	tokensdl::ValidateTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->token = rawToken;

	imtgql::CGqlRequest gqlRequest;
	AttachCurrentContext(gqlRequest);
	if (!tokensdl::CValidateTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tokensdl::CValidateTokenGqlRequest validateTokenRequest(gqlRequest, false);

	QString errorMessage;
	tokensdl::CValidateTokenPayload payload = OnValidateToken(validateTokenRequest, gqlRequest, errorMessage);
	if (!payload.valid.has_value() || !*payload.valid){
		return false;
	}

	if (payload.userId.has_value()){
		userId = *payload.userId;
	}

	if (payload.scopes.has_value()){
		scopes = payload.scopes->ToList();
	}

	// The SDL schema's ValidateTokenPayload does not include a tokenId field
	tokenId = QByteArray();

	return true;
}


QByteArrayList CClientRequestPersonalAccessTokenManagerComp::GetTokenIds(const QByteArray& userId) const
{
	namespace tokensdl = sdl::V1_0::imtauth;

	tokensdl::GetTokenListRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->userId = userId;

	imtgql::CGqlRequest gqlRequest;
	AttachCurrentContext(gqlRequest);
	if (!tokensdl::CGetTokenListGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArrayList();
	}

	tokensdl::CGetTokenListGqlRequest getTokenListRequest(gqlRequest, false);

	QString errorMessage;
	tokensdl::CPersonalAccessTokenList payload = OnGetTokenList(getTokenListRequest, gqlRequest, errorMessage);
	if (!payload.tokens.has_value()){
		return QByteArrayList();
	}

	QByteArrayList tokenIds;
	const auto& tokensList = payload.tokens->ToList();
	for (const auto& token : tokensList){
		if (token.id.has_value()){
			tokenIds << *token.id;
		}
	}

	return tokenIds;
}


imtauth::IPersonalAccessTokenSharedPtr CClientRequestPersonalAccessTokenManagerComp::GetToken(const QByteArray& tokenId) const
{
	namespace tokensdl = sdl::V1_0::imtauth;

	if (!m_tokenFactoryCompPtr.IsValid()){
		return nullptr;
	}

	tokensdl::GetTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = tokenId;

	imtgql::CGqlRequest gqlRequest;
	AttachCurrentContext(gqlRequest);
	if (!tokensdl::CGetTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	tokensdl::CGetTokenGqlRequest getTokenRequest(gqlRequest, false);

	QString errorMessage;
	tokensdl::CPersonalAccessToken payload = OnGetToken(getTokenRequest, gqlRequest, errorMessage);
	if (!payload.id.has_value()){
		return nullptr;
	}

	imtauth::IPersonalAccessTokenUniquePtr tokenPtr = m_tokenFactoryCompPtr.CreateInstance();
	if (!tokenPtr.IsValid()){
		return nullptr;
	}

	tokenPtr->SetId(*payload.id);

	if (payload.userId.has_value()){
		tokenPtr->SetUserId(*payload.userId);
	}
	if (payload.productId.has_value()){
		tokenPtr->SetProductId(*payload.productId);
	}
	if (payload.name.has_value()){
		tokenPtr->SetName(*payload.name);
	}
	if (payload.description.has_value()){
		tokenPtr->SetDescription(*payload.description);
	}
	if (payload.tokenHash.has_value()){
		tokenPtr->SetTokenHash(*payload.tokenHash);
	}
	if (payload.scopes.has_value()){
		tokenPtr->SetScopes(payload.scopes->ToList());
	}
	if (payload.createdAt.has_value()){
		tokenPtr->SetCreatedAt(QDateTime::fromString(*payload.createdAt, Qt::ISODate).toUTC());
	}
	if (payload.lastUsedAt.has_value()){
		tokenPtr->SetLastUsedAt(QDateTime::fromString(*payload.lastUsedAt, Qt::ISODate).toUTC());
	}
	if (payload.expiresAt.has_value()){
		tokenPtr->SetExpiresAt(QDateTime::fromString(*payload.expiresAt, Qt::ISODate).toUTC());
	}
	if (payload.revoked.has_value()){
		tokenPtr->SetRevoked(*payload.revoked);
	}

	return imtauth::IPersonalAccessTokenSharedPtr::CreateFromUnique(tokenPtr);
}


bool CClientRequestPersonalAccessTokenManagerComp::RevokeToken(const QByteArray& tokenId)
{
	namespace tokensdl = sdl::V1_0::imtauth;

	tokensdl::RevokeTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = tokenId;

	imtgql::CGqlRequest gqlRequest;
	AttachCurrentContext(gqlRequest);
	if (!tokensdl::CRevokeTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tokensdl::CRevokeTokenGqlRequest revokeTokenRequest(gqlRequest, false);

	QString errorMessage;
	tokensdl::CRevokeTokenPayload payload = OnRevokeToken(revokeTokenRequest, gqlRequest, errorMessage);
	if (!payload.success.has_value()){
		return false;
	}

	bool success = *payload.success;

	// Notify observers if the operation succeeded
	if (success){
		istd::CChangeNotifier changeNotifier(this);
	}

	return success;
}


bool CClientRequestPersonalAccessTokenManagerComp::UpdateLastUsedAt(const QByteArray& tokenId)
{
	// UpdateLastUsedAt has no corresponding mutation in the PersonalAccessTokens.sdl schema;
	// the server updates lastUsedAt automatically when ValidateToken is called.
	Q_UNUSED(tokenId);
	return false;
}


bool CClientRequestPersonalAccessTokenManagerComp::DeleteToken(const QByteArray& tokenId)
{
	namespace tokensdl = sdl::V1_0::imtauth;

	tokensdl::DeleteTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = tokenId;

	imtgql::CGqlRequest gqlRequest;
	AttachCurrentContext(gqlRequest);
	if (!tokensdl::CDeleteTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tokensdl::CDeleteTokenGqlRequest deleteTokenRequest(gqlRequest, false);

	QString errorMessage;
	tokensdl::CDeleteTokenPayload payload = OnDeleteToken(deleteTokenRequest, gqlRequest, errorMessage);
	if (!payload.success.has_value()){
		return false;
	}

	bool success = *payload.success;

	// Notify observers if the operation succeeded
	if (success){
		istd::CChangeNotifier changeNotifier(this);
	}

	return success;
}


// reimplemented (iser::ISerializable)

bool CClientRequestPersonalAccessTokenManagerComp::Serialize(iser::IArchive& /*archive*/)
{
	return true;
}


// reimplemented (sdl::V1_0::imtauth::CPersonalAccessTokensGqlHandlerCompBase)

sdl::V1_0::imtauth::CPersonalAccessTokenList CClientRequestPersonalAccessTokenManagerComp::OnGetTokenList(
			const sdl::V1_0::imtauth::CGetTokenListGqlRequest& /*getTokenListRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CPersonalAccessTokenList>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CPersonalAccessToken CClientRequestPersonalAccessTokenManagerComp::OnGetToken(
			const sdl::V1_0::imtauth::CGetTokenGqlRequest& /*getTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CPersonalAccessToken>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CValidateTokenPayload CClientRequestPersonalAccessTokenManagerComp::OnValidateToken(
			const sdl::V1_0::imtauth::CValidateTokenGqlRequest& /*validateTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CValidateTokenPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateTokenPayload CClientRequestPersonalAccessTokenManagerComp::OnCreateToken(
			const sdl::V1_0::imtauth::CCreateTokenGqlRequest& /*createTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateTokenPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRevokeTokenPayload CClientRequestPersonalAccessTokenManagerComp::OnRevokeToken(
			const sdl::V1_0::imtauth::CRevokeTokenGqlRequest& /*revokeTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRevokeTokenPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CDeleteTokenPayload CClientRequestPersonalAccessTokenManagerComp::OnDeleteToken(
			const sdl::V1_0::imtauth::CDeleteTokenGqlRequest& /*deleteTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CDeleteTokenPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
