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
		arguments.input.Version_1_0->expiresAt = expiresAt.toString(Qt::ISODate);
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

	// Note: tokenId is not returned in the SDL schema's ValidateTokenPayload
	// This is intentional for security - the token ID may be derived internally
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
		QDateTime createdAt = QDateTime::fromString(*payload.Version_1_0->createdAt, Qt::ISODate);
		tokenPtr->SetCreatedAt(createdAt);
	}

	if (payload.Version_1_0->lastUsedAt.HasValue()){
		QDateTime lastUsedAt = QDateTime::fromString(*payload.Version_1_0->lastUsedAt, Qt::ISODate);
		tokenPtr->SetLastUsedAt(lastUsedAt);
	}

	if (payload.Version_1_0->expiresAt.HasValue()){
		QDateTime expiresAt = QDateTime::fromString(*payload.Version_1_0->expiresAt, Qt::ISODate);
		tokenPtr->SetExpiresAt(expiresAt);
	}

	if (payload.Version_1_0->revoked.HasValue()){
		tokenPtr->SetRevoked(*payload.Version_1_0->revoked);
	}

	return tokenPtr.PopInterfacePtr();
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
	// Note: UpdateLastUsedAt is typically handled server-side during token validation
	// For the client implementation, we don't have a direct GraphQL mutation for this
	// This would typically be called internally by the server when ValidateToken is called
	
	// Since the SDL schema doesn't define an UpdateLastUsedAt mutation,
	// we return false to indicate this operation is not supported in the client
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

