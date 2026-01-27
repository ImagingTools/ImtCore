#include <imtauthgql/CPersonalAccessTokenControllerComp.h>


// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>


namespace imtauthgql
{


// Helper function to convert IPersonalAccessToken to SDL type
static sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken ConvertToSdlToken(
	const imtauth::IPersonalAccessToken* tokenPtr)
{
	sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken sdlToken;
	
	if (tokenPtr == nullptr){
		return sdlToken;
	}
	
	sdlToken.id = tokenPtr->GetId();
	sdlToken.userId = tokenPtr->GetUserId();
	sdlToken.name = tokenPtr->GetName();
	sdlToken.description = tokenPtr->GetDescription();
	sdlToken.tokenHash = tokenPtr->GetTokenHash();
	sdlToken.scopes = tokenPtr->GetScopes();
	
	QDateTime createdAt = tokenPtr->GetCreatedAt();
	if (createdAt.isValid()){
		sdlToken.createdAt = createdAt.toString(Qt::ISODate);
	}
	
	QDateTime lastUsedAt = tokenPtr->GetLastUsedAt();
	if (lastUsedAt.isValid()){
		sdlToken.lastUsedAt = lastUsedAt.toString(Qt::ISODate);
	}
	
	QDateTime expiresAt = tokenPtr->GetExpiresAt();
	if (expiresAt.isValid()){
		sdlToken.expiresAt = expiresAt.toString(Qt::ISODate);
	}
	
	sdlToken.revoked = tokenPtr->IsRevoked();
	
	return sdlToken;
}


// protected methods

// reimplemented (sdl::imtauth::PersonalAccessTokens::CGraphQlHandlerCompBase)

sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenList CPersonalAccessTokenControllerComp::OnGetTokenList(
			const sdl::imtauth::PersonalAccessTokens::CGetTokenListGqlRequest& getTokenListRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
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
	QList<sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken> tokens;
	for (const QByteArray& tokenId : tokenIds){
		imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
		if (tokenPtr != nullptr){
			tokens.append(ConvertToSdlToken(tokenPtr.GetPtr()));
		}
	}
	
	response.tokens = tokens;
	
	return response;
}


sdl::imtauth::PersonalAccessTokens::CPersonalAccessToken CPersonalAccessTokenControllerComp::OnGetToken(
			const sdl::imtauth::PersonalAccessTokens::CGetTokenGqlRequest& getTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
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
	
	sdl::imtauth::PersonalAccessTokens::CInputId::V1_0 inputArgument = *arguments.input.Version_1_0;
	
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
	
	if (tokenPtr == nullptr){
		errorMessage = QString("Token with id '%1' not found").arg(QString::fromUtf8(tokenId));
		return response;
	}
	
	response = ConvertToSdlToken(tokenPtr.GetPtr());
	
	return response;
}


sdl::imtauth::PersonalAccessTokens::CValidateTokenPayload CPersonalAccessTokenControllerComp::OnValidateToken(
			const sdl::imtauth::PersonalAccessTokens::CValidateTokenGqlRequest& validateTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CValidateTokenPayload response;
	response.valid = false;
	
	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.message = errorMessage;
		return response;
	}
	
	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::ValidateTokenRequestArguments arguments = 
		validateTokenRequest.GetRequestedArguments();
	
	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		response.message = errorMessage;
		return response;
	}
	
	sdl::imtauth::PersonalAccessTokens::CTokenInput::V1_0 inputArgument = *arguments.input.Version_1_0;
	
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
	QByteArrayList scopes;
	bool isValid = m_tokenManagerCompPtr->ValidateToken(rawToken, userId, scopes);
	
	response.valid = isValid;
	
	if (isValid){
		response.userId = userId;
		response.scopes = scopes;
		response.message = "Token is valid";
	}
	else{
		response.message = "Token is invalid, revoked, or expired";
	}
	
	return response;
}


sdl::imtauth::PersonalAccessTokens::CCreateTokenPayload CPersonalAccessTokenControllerComp::OnCreateToken(
			const sdl::imtauth::PersonalAccessTokens::CCreateTokenGqlRequest& createTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CCreateTokenPayload response;
	response.success = false;
	
	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.message = errorMessage;
		return response;
	}
	
	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::CreateTokenRequestArguments arguments = 
		createTokenRequest.GetRequestedArguments();
	
	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		response.message = errorMessage;
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
		scopes = *inputArgument.scopes;
	}
	
	QDateTime expiresAt;
	if (inputArgument.expiresAt){
		QString expiresAtStr = *inputArgument.expiresAt;
		if (!expiresAtStr.isEmpty()){
			expiresAt = QDateTime::fromString(expiresAtStr, Qt::ISODate);
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


sdl::imtauth::PersonalAccessTokens::CRevokeTokenPayload CPersonalAccessTokenControllerComp::OnRevokeToken(
			const sdl::imtauth::PersonalAccessTokens::CRevokeTokenGqlRequest& revokeTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CRevokeTokenPayload response;
	response.success = false;
	
	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.message = errorMessage;
		return response;
	}
	
	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::RevokeTokenRequestArguments arguments = 
		revokeTokenRequest.GetRequestedArguments();
	
	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		response.message = errorMessage;
		return response;
	}
	
	sdl::imtauth::PersonalAccessTokens::CInputId::V1_0 inputArgument = *arguments.input.Version_1_0;
	
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


sdl::imtauth::PersonalAccessTokens::CDeleteTokenPayload CPersonalAccessTokenControllerComp::OnDeleteToken(
			const sdl::imtauth::PersonalAccessTokens::CDeleteTokenGqlRequest& deleteTokenRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CDeleteTokenPayload response;
	response.success = false;
	
	if (!m_tokenManagerCompPtr.IsValid()){
		errorMessage = "PersonalAccessTokenManager not available";
		response.message = errorMessage;
		return response;
	}
	
	// Get request arguments
	sdl::imtauth::PersonalAccessTokens::DeleteTokenRequestArguments arguments = 
		deleteTokenRequest.GetRequestedArguments();
	
	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = "Invalid request: missing input argument";
		response.message = errorMessage;
		return response;
	}
	
	sdl::imtauth::PersonalAccessTokens::CInputId::V1_0 inputArgument = *arguments.input.Version_1_0;
	
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


