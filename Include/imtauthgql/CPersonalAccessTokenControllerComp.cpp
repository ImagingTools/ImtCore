// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CPersonalAccessTokenControllerComp.h>


namespace imtauthgql
{


// Helper function to convert IPersonalAccessToken to SDL type
static sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken::V1_0 ConvertToSdlToken(
	const imtauth::IPersonalAccessToken& token)
{
	sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken::V1_0 sdlToken;

	sdlToken.id = token.GetId();
	sdlToken.userId = token.GetUserId();
	sdlToken.name = token.GetName();
	sdlToken.description = token.GetDescription();
	sdlToken.tokenHash = token.GetTokenHash();
	sdlToken.scopes.Emplace().FromList(token.GetScopes());

	QDateTime createdAt = token.GetCreatedAt();
	if (createdAt.isValid()){
		// Convert datetime to UTC before converting to string
		sdlToken.createdAt = createdAt.toUTC().toString(Qt::ISODate);
	}

	QDateTime lastUsedAt = token.GetLastUsedAt();
	if (lastUsedAt.isValid()){
		// Convert datetime to UTC before converting to string
		sdlToken.lastUsedAt = lastUsedAt.toUTC().toString(Qt::ISODate);
	}

	QDateTime expiresAt = token.GetExpiresAt();
	if (expiresAt.isValid()){
		// Convert datetime to UTC before converting to string
		sdlToken.expiresAt = expiresAt.toUTC().toString(Qt::ISODate);
	}
	else{
		sdlToken.expiresAt = QString();
	}

	sdlToken.revoked = token.IsRevoked();

	return sdlToken;
}


// protected methods

// reimplemented (sdl::imtauth::PersonalAccessTokens::CGraphQlHandlerCompBase)

sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenList CPersonalAccessTokenControllerComp::OnGetTokenList(
			const sdl::imtauth::PersonalAccessTokens::CGetTokenListGqlRequest& getTokenListRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenList response;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		return response;
	}

	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::GetTokenListRequestArguments arguments = 
		getTokenListRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		return response;
	}

	sdl::imtauth::PersonalAccessTokens::CUserIdInput::V1_0 inputArgument = *arguments.input.Version_1_0;

	QByteArray userId;
	if (inputArgument.userId){
		userId = *inputArgument.userId;
	}

	if (userId.isEmpty()){
		errorMessage = "Invalid request: userId is required";
		return response;
	}

	// Get token IDs for user
	QByteArrayList tokenIds = m_tokenManagerCompPtr->GetTokenIds(userId);

	// Get each token and convert to SDL type
	QList<sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken::V1_0> tokens;
	for (const QByteArray& tokenId : std::as_const(tokenIds)){
		imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
		if (tokenPtr.IsValid()){
			tokens.append(ConvertToSdlToken(*tokenPtr.GetPtr()));
		}
	}

	response.Version_1_0.Emplace();
	response.Version_1_0->tokens.Emplace().FromList(tokens);

	return response;
}


sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken CPersonalAccessTokenControllerComp::OnGetToken(
			const sdl::imtauth::PersonalAccessTokens::CGetTokenGqlRequest& getTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken response;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		return response;
	}

	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::GetTokenRequestArguments arguments = 
		getTokenRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		return response;
	}

	sdl::imtbase::ImtCollection::CInputId::V1_0 inputArgument = *arguments.input.Version_1_0;

	QByteArray tokenId;
	if (inputArgument.id){
		tokenId = *inputArgument.id;
	}

	if (tokenId.isEmpty()){
		errorMessage = "Invalid request: id is required";
		return response;
	}

	// Get token from manager
	imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
	if (!tokenPtr.IsValid()){
		errorMessage = QString("Token with id '%1' not found").arg(QString::fromUtf8(tokenId));
		return response;
	}

	response.Version_1_0 = ConvertToSdlToken(*tokenPtr.GetPtr());

	return response;
}


sdl::imtauth::PersonalAccessTokens::CValidateTokenPayload CPersonalAccessTokenControllerComp::OnValidateToken(
			const sdl::imtauth::PersonalAccessTokens::CValidateTokenGqlRequest& validateTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CValidateTokenPayload response;
	response.Version_1_0.Emplace();
	response.Version_1_0->valid = false;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::ValidateTokenRequestArguments arguments = 
		validateTokenRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	sdl::imtauth::PersonalAccessTokens::CTokenInput::V1_0 inputArgument = *arguments.input.Version_1_0;

	QByteArray rawToken;
	if (inputArgument.token){
		rawToken = *inputArgument.token;
	}

	if (rawToken.isEmpty()){
		errorMessage = "Invalid request: token is required";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	// Validate token
	QByteArray userId;
	QByteArray tokenId;
	QByteArrayList scopes;
	bool isValid = m_tokenManagerCompPtr->ValidateToken(rawToken, userId, tokenId, scopes);

	response.Version_1_0->valid = isValid;

	if (isValid){
		response.Version_1_0->userId = userId;
		response.Version_1_0->scopes.Emplace().FromList(scopes);
		response.Version_1_0->message = "Token is valid";
	}
	else{
		response.Version_1_0->message = "Token is invalid, revoked, or expired";
	}

	return response;
}


sdl::imtauth::PersonalAccessTokens::CCreateTokenPayload CPersonalAccessTokenControllerComp::OnCreateToken(
			const sdl::imtauth::PersonalAccessTokens::CCreateTokenGqlRequest& createTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CCreateTokenPayload response;
	response.Version_1_0.Emplace();
	response.Version_1_0->success = false;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::CreateTokenRequestArguments arguments = 
		createTokenRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	sdl::imtauth::PersonalAccessTokens::CCreateTokenInput::V1_0 inputArgument = *arguments.input.Version_1_0;

	QByteArray userId;
	if (inputArgument.userId){
		userId = *inputArgument.userId;
	}

	QString name;
	if (inputArgument.name){
		name = *inputArgument.name;
	}

	QString description;
	if (inputArgument.description){
		description = *inputArgument.description;
	}

	QByteArrayList scopes;
	if (inputArgument.scopes){
		scopes = inputArgument.scopes->ToList();
	}

	QDateTime expiresAt;
	if (inputArgument.expiresAt){
		QString expiresAtStr = *inputArgument.expiresAt;
		if (!expiresAtStr.isEmpty()){
			expiresAt = QDateTime::fromString(expiresAtStr, Qt::ISODate);
			// Ensure the datetime is properly converted to UTC
			if (expiresAt.isValid()){
				expiresAt = expiresAt.toUTC();
			}
		}
	}

	if (userId.isEmpty()){
		errorMessage = "Invalid request: userId is required";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	if (name.isEmpty()){
		errorMessage = "Invalid request: name is required";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	// Create token
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerCompPtr->CreateToken(userId, name, description, scopes, expiresAt);

	response.Version_1_0->success = result.success;

	if (result.success){
		response.Version_1_0->id = result.tokenId;
		response.Version_1_0->token = result.rawToken;
		response.Version_1_0->message = "Token created successfully";
	}
	else{
		errorMessage = "Failed to create token";
		response.Version_1_0->message = errorMessage;
	}

	return response;
}


sdl::imtauth::PersonalAccessTokens::CRevokeTokenPayload CPersonalAccessTokenControllerComp::OnRevokeToken(
			const sdl::imtauth::PersonalAccessTokens::CRevokeTokenGqlRequest& revokeTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CRevokeTokenPayload response;
	response.Version_1_0.Emplace();
	response.Version_1_0->success = false;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::RevokeTokenRequestArguments arguments = 
		revokeTokenRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	sdl::imtbase::ImtCollection::CInputId::V1_0 inputArgument = *arguments.input.Version_1_0;

	QByteArray tokenId;
	if (inputArgument.id){
		tokenId = *inputArgument.id;
	}

	if (tokenId.isEmpty()){
		errorMessage = "Invalid request: id is required";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	// Revoke token
	bool success = m_tokenManagerCompPtr->RevokeToken(tokenId);

	response.Version_1_0->success = success;

	if (success){
		response.Version_1_0->message = "Token revoked successfully";
	}
	else{
		errorMessage = QString("Failed to revoke token with id '%1'").arg(QString::fromUtf8(tokenId));
		response.Version_1_0->message = errorMessage;
	}

	return response;
}


sdl::imtauth::PersonalAccessTokens::CDeleteTokenPayload CPersonalAccessTokenControllerComp::OnDeleteToken(
			const sdl::imtauth::PersonalAccessTokens::CDeleteTokenGqlRequest& deleteTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CDeleteTokenPayload response;
	response.Version_1_0.Emplace();
	response.Version_1_0->success = false;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::DeleteTokenRequestArguments arguments = 
		deleteTokenRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	sdl::imtbase::ImtCollection::CInputId::V1_0 inputArgument = *arguments.input.Version_1_0;

	QByteArray tokenId;
	if (inputArgument.id){
		tokenId = *inputArgument.id;
	}

	if (tokenId.isEmpty()){
		errorMessage = "Invalid request: id is required";
		response.Version_1_0->message = errorMessage;
		return response;
	}

	// Delete token
	bool success = m_tokenManagerCompPtr->DeleteToken(tokenId);

	response.Version_1_0->success = success;

	if (success){
		response.Version_1_0->message = "Token deleted successfully";
	}
	else{
		errorMessage = QString("Failed to delete token with id '%1'").arg(QString::fromUtf8(tokenId));
		response.Version_1_0->message = errorMessage;
	}

	return response;
}


} // namespace imtauthgql


