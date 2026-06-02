// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens.h>
#include <imtauthgql/CPersonalAccessTokenControllerComp.h>


namespace imtauthgql
{


// Helper function to convert IPersonalAccessToken to SDL type
static sdl::V1_0::imtauth::CPersonalAccessToken ConvertToSdlToken(
	const imtauth::IPersonalAccessToken& token)
{
	sdl::V1_0::imtauth::CPersonalAccessToken sdlToken;

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

// reimplemented (sdl::V1_0::imtauth::CPersonalAccessTokensGqlHandlerCompBase)

sdl::V1_0::imtauth::CPersonalAccessTokenList CPersonalAccessTokenControllerComp::OnGetTokenList(
			const sdl::V1_0::imtauth::CGetTokenListGqlRequest& getTokenListRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CPersonalAccessTokenList response;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		return response;
	}

	// Get request arguments
	sdl::V1_0::imtauth::GetTokenListRequestArguments arguments = 
		getTokenListRequest.GetRequestedArguments();

	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	auto& inputArgument = *arguments.input;

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
	QList<sdl::V1_0::imtauth::CPersonalAccessToken> tokens;
	for (const QByteArray& tokenId : std::as_const(tokenIds)){
		imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
		if (tokenPtr.IsValid()){
			tokens.append(ConvertToSdlToken(*tokenPtr.GetPtr()));
		}
	}

	response.tokens.Emplace().FromList(tokens);

	return response;
}


sdl::V1_0::imtauth::CPersonalAccessToken CPersonalAccessTokenControllerComp::OnGetToken(
			const sdl::V1_0::imtauth::CGetTokenGqlRequest& getTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CPersonalAccessToken response;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		return response;
	}

	// Get request arguments
	sdl::V1_0::imtauth::GetTokenRequestArguments arguments = 
		getTokenRequest.GetRequestedArguments();

	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	auto& inputArgument = *arguments.input;

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

	return ConvertToSdlToken(*tokenPtr.GetPtr());
}


sdl::V1_0::imtauth::CValidateTokenPayload CPersonalAccessTokenControllerComp::OnValidateToken(
			const sdl::V1_0::imtauth::CValidateTokenGqlRequest& validateTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CValidateTokenPayload response;
	response.valid = false;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.message = errorMessage;
		return response;
	}

	// Get request arguments
	sdl::V1_0::imtauth::ValidateTokenRequestArguments arguments = 
		validateTokenRequest.GetRequestedArguments();

	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	auto& inputArgument = *arguments.input;

	QByteArray rawToken;
	if (inputArgument.token){
		rawToken = *inputArgument.token;
	}

	if (rawToken.isEmpty()){
		errorMessage = "Invalid request: token is required";
		response.message = errorMessage;
		return response;
	}

	// Validate token
	QByteArray userId;
	QByteArray tokenId;
	QByteArrayList scopes;
	bool isValid = m_tokenManagerCompPtr->ValidateToken(rawToken, userId, tokenId, scopes);

	response.valid = isValid;

	if (isValid){
		response.userId = userId;
		response.scopes.Emplace().FromList(scopes);
		response.message = "Token is valid";
	}
	else{
		response.message = "Token is invalid, revoked, or expired";
	}

	return response;
}


sdl::V1_0::imtauth::CCreateTokenPayload CPersonalAccessTokenControllerComp::OnCreateToken(
			const sdl::V1_0::imtauth::CCreateTokenGqlRequest& createTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CCreateTokenPayload response;
	response.success = false;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.message = errorMessage;
		return response;
	}

	// Get request arguments
	sdl::V1_0::imtauth::CreateTokenRequestArguments arguments = 
		createTokenRequest.GetRequestedArguments();

	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	auto& inputArgument = *arguments.input;

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
		response.message = errorMessage;
		return response;
	}

	if (name.isEmpty()){
		errorMessage = "Invalid request: name is required";
		response.message = errorMessage;
		return response;
	}

	// Create token
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerCompPtr->CreateToken(userId, name, description, scopes, expiresAt);

	response.success = result.success;

	if (result.success){
		response.id = result.tokenId;
		response.token = result.rawToken;
		response.message = "Token created successfully";
	}
	else{
		errorMessage = "Failed to create token";
		response.message = errorMessage;
	}

	return response;
}


sdl::V1_0::imtauth::CRevokeTokenPayload CPersonalAccessTokenControllerComp::OnRevokeToken(
			const sdl::V1_0::imtauth::CRevokeTokenGqlRequest& revokeTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CRevokeTokenPayload response;
	response.success = false;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.message = errorMessage;
		return response;
	}

	// Get request arguments
	sdl::V1_0::imtauth::RevokeTokenRequestArguments arguments = 
		revokeTokenRequest.GetRequestedArguments();

	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	auto& inputArgument = *arguments.input;

	QByteArray tokenId;
	if (inputArgument.id){
		tokenId = *inputArgument.id;
	}

	if (tokenId.isEmpty()){
		errorMessage = "Invalid request: id is required";
		response.message = errorMessage;
		return response;
	}

	// Revoke token
	bool success = m_tokenManagerCompPtr->RevokeToken(tokenId);

	response.success = success;

	if (success){
		response.message = "Token revoked successfully";
	}
	else{
		errorMessage = QString("Failed to revoke token with id '%1'").arg(QString::fromUtf8(tokenId));
		response.message = errorMessage;
	}

	return response;
}


sdl::V1_0::imtauth::CDeleteTokenPayload CPersonalAccessTokenControllerComp::OnDeleteToken(
			const sdl::V1_0::imtauth::CDeleteTokenGqlRequest& deleteTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CDeleteTokenPayload response;
	response.success = false;

	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.message = errorMessage;
		return response;
	}

	// Get request arguments
	sdl::V1_0::imtauth::DeleteTokenRequestArguments arguments = 
		deleteTokenRequest.GetRequestedArguments();

	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	auto& inputArgument = *arguments.input;

	QByteArray tokenId;
	if (inputArgument.id){
		tokenId = *inputArgument.id;
	}

	if (tokenId.isEmpty()){
		errorMessage = "Invalid request: id is required";
		response.message = errorMessage;
		return response;
	}

	// Delete token
	bool success = m_tokenManagerCompPtr->DeleteToken(tokenId);

	response.success = success;

	if (success){
		response.message = "Token deleted successfully";
	}
	else{
		errorMessage = QString("Failed to delete token with id '%1'").arg(QString::fromUtf8(tokenId));
		response.message = errorMessage;
	}

	return response;
}


} // namespace imtauthgql


