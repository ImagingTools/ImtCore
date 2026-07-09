// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestPersonalAccessTokenManagerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens.h>


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauthgql
{


// public methods

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

	tokensdl::CCreateTokenPayload payload;
	bool ok = SendModelRequestInternal<tokensdl::CreateTokenRequestArguments, tokensdl::CCreateTokenPayload, tokensdl::CCreateTokenGqlRequest>(arguments, payload);

	imtauth::IPersonalAccessTokenManager::TokenCreationResult result;
	result.success = false;

	if (!ok){
		return result;
	}

	if (!payload.success.HasValue() || !*payload.success){
		return result;
	}

	if (!payload.id.HasValue() || !payload.token.HasValue()){
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

	tokensdl::CValidateTokenPayload payload;
	bool ok = SendModelRequestInternal<tokensdl::ValidateTokenRequestArguments, tokensdl::CValidateTokenPayload, tokensdl::CValidateTokenGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.valid.HasValue() || !*payload.valid){
		return false;
	}

	if (payload.userId.HasValue()){
		userId = *payload.userId;
	}

	if (payload.scopes.HasValue()){
		scopes = payload.scopes->ToList();
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
	namespace tokensdl = sdl::V1_0::imtauth;

	tokensdl::GetTokenListRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->userId = userId;

	tokensdl::CPersonalAccessTokenList payload;
	bool ok = SendModelRequestInternal<tokensdl::GetTokenListRequestArguments, tokensdl::CPersonalAccessTokenList, tokensdl::CGetTokenListGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArrayList();
	}

	if (!payload.tokens.HasValue()){
		return QByteArrayList();
	}

	QByteArrayList tokenIds;
	const auto& tokensList = payload.tokens->ToList();
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

	namespace tokensdl = sdl::V1_0::imtauth;

	tokensdl::GetTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = tokenId;

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
	if (payload.id.HasValue()){
		tokenPtr->SetId(*payload.id);
	}

	if (payload.userId.HasValue()){
		tokenPtr->SetUserId(*payload.userId);
	}

	if (payload.productId.HasValue()){
		tokenPtr->SetProductId(*payload.productId);
	}

	if (payload.name.HasValue()){
		tokenPtr->SetName(*payload.name);
	}

	if (payload.description.HasValue()){
		tokenPtr->SetDescription(*payload.description);
	}

	if (payload.tokenHash.HasValue()){
		tokenPtr->SetTokenHash(*payload.tokenHash);
	}

	if (payload.scopes.HasValue()){
		tokenPtr->SetScopes(payload.scopes->ToList());
	}

	if (payload.createdAt.HasValue()){
		QDateTime createdAt = QDateTime::fromString(*payload.createdAt, Qt::ISODate).toUTC();
		tokenPtr->SetCreatedAt(createdAt);
	}

	if (payload.lastUsedAt.HasValue()){
		QDateTime lastUsedAt = QDateTime::fromString(*payload.lastUsedAt, Qt::ISODate).toUTC();
		tokenPtr->SetLastUsedAt(lastUsedAt);
	}

	if (payload.expiresAt.HasValue()){
		QDateTime expiresAt = QDateTime::fromString(*payload.expiresAt, Qt::ISODate).toUTC();
		tokenPtr->SetExpiresAt(expiresAt);
	}

	if (payload.revoked.HasValue()){
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

	tokensdl::CRevokeTokenPayload payload;
	bool ok = SendModelRequestInternal<tokensdl::RevokeTokenRequestArguments, tokensdl::CRevokeTokenPayload, tokensdl::CRevokeTokenGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.success.HasValue()){
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
	namespace tokensdl = sdl::V1_0::imtauth;

	tokensdl::DeleteTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->id = tokenId;

	tokensdl::CDeleteTokenPayload payload;
	bool ok = SendModelRequestInternal<tokensdl::DeleteTokenRequestArguments, tokensdl::CDeleteTokenPayload, tokensdl::CDeleteTokenGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.success.HasValue()){
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


} // namespace imtauthgql

