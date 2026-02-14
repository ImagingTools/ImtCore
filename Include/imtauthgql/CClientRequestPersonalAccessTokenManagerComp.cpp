// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestPersonalAccessTokenManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IPersonalAccessTokenManager)

imtauth::IPersonalAccessTokenManager::TokenCreationResult CClientRequestPersonalAccessTokenManagerComp::CreateToken(
			const QByteArray& userId,
			const QString& name,
			const QString& description,
			const QByteArrayList& scopes,
			const QDateTime& expiresAt)
{
	namespace tokensdl = sdl::imtauth::PersonalAccessTokens;

	tokensdl::CreateTokenRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->userId = userId;
	arguments.input.Version_1_0->name = name;
	
	if (!description.isEmpty()){
		arguments.input.Version_1_0->description = description;
	}
	
	if (!scopes.isEmpty()){
		arguments.input.Version_1_0->scopes.Emplace();
		arguments.input.Version_1_0->scopes->FromList(scopes);
	}
	
	if (expiresAt.isValid()){
		arguments.input.Version_1_0->expiresAt = expiresAt.toUTC().toString(Qt::ISODate);
	}

	tokensdl::CCreateTokenPayload payload;
	bool ok = SendModelRequestInternal<tokensdl::CreateTokenRequestArguments, tokensdl::CCreateTokenPayload, tokensdl::CCreateTokenGqlRequest>(arguments, payload);

	imtauth::IPersonalAccessTokenManager::TokenCreationResult result;
	result.success = false;

	if (!ok){
		return result;
	}

	if (!payload.Version_1_0->success.HasValue() || !*payload.Version_1_0->success){
		return result;
	}

	if (!payload.Version_1_0->id.HasValue() || !payload.Version_1_0->token.HasValue()){
		return result;
	}

	result.success = true;
	result.tokenId = *payload.Version_1_0->id;
	result.rawToken = *payload.Version_1_0->token;

	return result;
}


bool CClientRequestPersonalAccessTokenManagerComp::ValidateToken(const QByteArray& rawToken, QByteArray& userId, QByteArray& tokenId, QByteArrayList& scopes) const
{
	namespace tokensdl = sdl::imtauth::PersonalAccessTokens;

	tokensdl::ValidateTokenRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->token = rawToken;

	tokensdl::CValidateTokenPayload payload;
	bool ok = SendModelRequestInternal<tokensdl::ValidateTokenRequestArguments, tokensdl::CValidateTokenPayload, tokensdl::CValidateTokenGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->valid.HasValue() || !*payload.Version_1_0->valid){
		return false;
	}

	if (payload.Version_1_0->userId.HasValue()){
		userId = *payload.Version_1_0->userId;
	}

	if (payload.Version_1_0->scopes.HasValue()){
		scopes = payload.Version_1_0->scopes->ToList();
	}

	// Note: The SDL schema's ValidateTokenPayload does not include a tokenId field.
	// This is a limitation of the GraphQL client implementation. The tokenId would
	// need to be added to the ValidateTokenPayload type in PersonalAccessTokens.sdl
	// to support this parameter. For now, we return an empty tokenId.
	tokenId = QByteArray();

	return true;
}


QByteArrayList CClientRequestPersonalAccessTokenManagerComp::GetTokenIds(const QByteArray& userId) const
{
	namespace tokensdl = sdl::imtauth::PersonalAccessTokens;

	tokensdl::GetTokenListRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->userId = userId;

	tokensdl::CPersonalAccessTokenList payload;
	bool ok = SendModelRequestInternal<tokensdl::GetTokenListRequestArguments, tokensdl::CPersonalAccessTokenList, tokensdl::CGetTokenListGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArrayList();
	}

	if (!payload.Version_1_0->tokens.HasValue()){
		return QByteArrayList();
	}

	QByteArrayList tokenIds;
	const auto& tokensList = payload.Version_1_0->tokens->ToList();
	for (const auto& token : tokensList){
		if (token.id.HasValue()){
			tokenIds << *token.id;
		}
	}

	return tokenIds;
}


imtauth::IPersonalAccessTokenSharedPtr CClientRequestPersonalAccessTokenManagerComp::GetToken(const QByteArray& tokenId) const
{
	if (!m_tokenFactoryCompPtr.IsValid()){
		return nullptr;
	}

	namespace tokensdl = sdl::imtauth::PersonalAccessTokens;

	tokensdl::GetTokenRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = tokenId;

	tokensdl::CPersonalAccessToken payload;
	bool ok = SendModelRequestInternal<tokensdl::GetTokenRequestArguments, tokensdl::CPersonalAccessToken, tokensdl::CGetTokenGqlRequest>(arguments, payload);
	if (!ok){
		return nullptr;
	}

	imtauth::IPersonalAccessTokenUniquePtr tokenPtr = m_tokenFactoryCompPtr.CreateInstance();
	if (!tokenPtr.IsValid()){
		return nullptr;
	}

	// Populate token from SDL payload
	if (payload.Version_1_0->id.HasValue()){
		tokenPtr->SetId(*payload.Version_1_0->id);
	}

	if (payload.Version_1_0->userId.HasValue()){
		tokenPtr->SetUserId(*payload.Version_1_0->userId);
	}

	if (payload.Version_1_0->name.HasValue()){
		tokenPtr->SetName(*payload.Version_1_0->name);
	}

	if (payload.Version_1_0->description.HasValue()){
		tokenPtr->SetDescription(*payload.Version_1_0->description);
	}

	if (payload.Version_1_0->tokenHash.HasValue()){
		tokenPtr->SetTokenHash(*payload.Version_1_0->tokenHash);
	}

	if (payload.Version_1_0->scopes.HasValue()){
		tokenPtr->SetScopes(payload.Version_1_0->scopes->ToList());
	}

	if (payload.Version_1_0->createdAt.HasValue()){
		QDateTime createdAt = QDateTime::fromString(*payload.Version_1_0->createdAt, Qt::ISODate).toUTC();
		tokenPtr->SetCreatedAt(createdAt);
	}

	if (payload.Version_1_0->lastUsedAt.HasValue()){
		QDateTime lastUsedAt = QDateTime::fromString(*payload.Version_1_0->lastUsedAt, Qt::ISODate).toUTC();
		tokenPtr->SetLastUsedAt(lastUsedAt);
	}

	if (payload.Version_1_0->expiresAt.HasValue()){
		QDateTime expiresAt = QDateTime::fromString(*payload.Version_1_0->expiresAt, Qt::ISODate).toUTC();
		tokenPtr->SetExpiresAt(expiresAt);
	}

	if (payload.Version_1_0->revoked.HasValue()){
		tokenPtr->SetRevoked(*payload.Version_1_0->revoked);
	}

	return imtauth::IPersonalAccessTokenSharedPtr::CreateFromUnique(tokenPtr);
}


bool CClientRequestPersonalAccessTokenManagerComp::RevokeToken(const QByteArray& tokenId)
{
	namespace tokensdl = sdl::imtauth::PersonalAccessTokens;

	tokensdl::RevokeTokenRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = tokenId;

	tokensdl::CRevokeTokenPayload payload;
	bool ok = SendModelRequestInternal<tokensdl::RevokeTokenRequestArguments, tokensdl::CRevokeTokenPayload, tokensdl::CRevokeTokenGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->success.HasValue()){
		return false;
	}

	return *payload.Version_1_0->success;
}


bool CClientRequestPersonalAccessTokenManagerComp::UpdateLastUsedAt(const QByteArray& tokenId)
{
	// Note: UpdateLastUsedAt is not supported in the GraphQL client implementation.
	// This operation is typically handled server-side during token validation and does
	// not have a corresponding GraphQL mutation in the PersonalAccessTokens.sdl schema.
	// 
	// In a typical deployment:
	// - The server automatically updates lastUsedAt when ValidateToken is called
	// - Clients do not need to explicitly call this method
	// 
	// If explicit client-side control of lastUsedAt is required, a new mutation
	// would need to be added to the SDL schema.
	
	Q_UNUSED(tokenId);
	return false;
}


bool CClientRequestPersonalAccessTokenManagerComp::DeleteToken(const QByteArray& tokenId)
{
	namespace tokensdl = sdl::imtauth::PersonalAccessTokens;

	tokensdl::DeleteTokenRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = tokenId;

	tokensdl::CDeleteTokenPayload payload;
	bool ok = SendModelRequestInternal<tokensdl::DeleteTokenRequestArguments, tokensdl::CDeleteTokenPayload, tokensdl::CDeleteTokenGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->success.HasValue()){
		return false;
	}

	return *payload.Version_1_0->success;
}


} // namespace imtauthgql

