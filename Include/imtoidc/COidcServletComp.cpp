// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoidc/COidcServletComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QUuid>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QDateTime>
#include <QtCore/QCryptographicHash>

// ImtCore includes
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtauth/IOidcClient.h>
#include <imtauth/IOidcAuthorizationCode.h>
#include <imtauth/IOidcTokenInfo.h>


namespace imtoidc
{


// public methods

// reimplemented (imtrest::IRequestServlet)

bool COidcServletComp::IsCommandSupported(const QByteArray& commandId) const
{
	QByteArray endpoint = ExtractCommandEndpoint(commandId);

	return endpoint == "openid-configuration"
		|| endpoint == "authorize"
		|| endpoint == "token"
		|| endpoint == "userinfo"
		|| endpoint == "jwks"
		|| endpoint == "revoke"
		|| endpoint == "introspect";
}


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

COidcServletComp::ConstResponsePtr COidcServletComp::OnGet(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	QByteArray endpoint = ExtractCommandEndpoint(commandId);

	if (endpoint == "openid-configuration"){
		return HandleDiscovery(request);
	}

	if (endpoint == "authorize"){
		return HandleAuthorize(commandParams, headers, request);
	}

	if (endpoint == "userinfo"){
		return HandleUserInfo(headers, request);
	}

	if (endpoint == "jwks"){
		return HandleJwks(request);
	}

	return CreateErrorResponse("invalid_request", "Unknown endpoint", imtrest::IProtocolEngine::SC_NOT_FOUND, request);
}


COidcServletComp::ConstResponsePtr COidcServletComp::OnPost(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandParams);

	QByteArray endpoint = ExtractCommandEndpoint(commandId);

	if (endpoint == "token"){
		return HandleToken(headers, request);
	}

	if (endpoint == "revoke"){
		return HandleRevoke(headers, request);
	}

	if (endpoint == "introspect"){
		return HandleIntrospect(headers, request);
	}

	return CreateErrorResponse("invalid_request", "Unknown endpoint", imtrest::IProtocolEngine::SC_NOT_FOUND, request);
}


// private methods - Endpoint handlers

COidcServletComp::ConstResponsePtr COidcServletComp::HandleDiscovery(const imtrest::CHttpRequest& request) const
{
	QString issuer = QString::fromUtf8(*m_issuerUrlAttrPtr);

	QJsonObject discovery;
	discovery["issuer"] = issuer;
	discovery["authorization_endpoint"] = issuer + "/oauth/authorize";
	discovery["token_endpoint"] = issuer + "/oauth/token";
	discovery["userinfo_endpoint"] = issuer + "/oauth/userinfo";
	discovery["jwks_uri"] = issuer + "/oauth/jwks";
	discovery["revocation_endpoint"] = issuer + "/oauth/revoke";
	discovery["introspection_endpoint"] = issuer + "/oauth/introspect";

	QJsonArray responseTypes;
	responseTypes.append("code");
	discovery["response_types_supported"] = responseTypes;

	QJsonArray grantTypes;
	grantTypes.append("authorization_code");
	grantTypes.append("client_credentials");
	grantTypes.append("refresh_token");
	discovery["grant_types_supported"] = grantTypes;

	QJsonArray subjectTypes;
	subjectTypes.append("public");
	discovery["subject_types_supported"] = subjectTypes;

	QJsonArray idTokenSigningAlg;
	idTokenSigningAlg.append("RS256");
	discovery["id_token_signing_alg_values_supported"] = idTokenSigningAlg;

	if (m_scopeMapperCompPtr.IsValid()){
		QByteArrayList scopes = m_scopeMapperCompPtr->GetSupportedScopes();
		QJsonArray scopesArray;
		for (const QByteArray& scope : ::std::as_const(scopes)){
			scopesArray.append(QString::fromUtf8(scope));
		}
		discovery["scopes_supported"] = scopesArray;

		QByteArrayList claims = m_scopeMapperCompPtr->GetSupportedClaims();
		QJsonArray claimsArray;
		for (const QByteArray& claim : ::std::as_const(claims)){
			claimsArray.append(QString::fromUtf8(claim));
		}
		discovery["claims_supported"] = claimsArray;
	}

	QJsonArray tokenEndpointAuthMethods;
	tokenEndpointAuthMethods.append("client_secret_basic");
	tokenEndpointAuthMethods.append("client_secret_post");
	discovery["token_endpoint_auth_methods_supported"] = tokenEndpointAuthMethods;

	QJsonArray codeChallengeMethodsSupported;
	codeChallengeMethodsSupported.append("S256");
	codeChallengeMethodsSupported.append("plain");
	discovery["code_challenge_methods_supported"] = codeChallengeMethodsSupported;

	return CreateJsonResponse(discovery, imtrest::IProtocolEngine::SC_OK, request);
}


COidcServletComp::ConstResponsePtr COidcServletComp::HandleAuthorize(
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	QByteArray clientId = commandParams.value("client_id");
	QByteArray redirectUri = commandParams.value("redirect_uri");
	QByteArray responseType = commandParams.value("response_type");
	QByteArray scope = commandParams.value("scope");
	QByteArray state = commandParams.value("state");
	QByteArray codeChallenge = commandParams.value("code_challenge");
	QByteArray codeChallengeMethod = commandParams.value("code_challenge_method", "plain");

	if (clientId.isEmpty() || redirectUri.isEmpty()){
		return CreateErrorResponse("invalid_request", "Missing required parameters: client_id, redirect_uri",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	if (responseType != "code"){
		return CreateErrorResponse("unsupported_response_type", "Only 'code' response_type is supported",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	// Validate client exists
	if (m_clientCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr clientDataPtr;
		if (!m_clientCollectionCompPtr->GetObjectData(clientId, clientDataPtr)){
			return CreateErrorResponse("invalid_client", "Unknown client_id",
						imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
		}

		// Validate redirect_uri
		const imtauth::IOidcClient* oidcClientPtr = dynamic_cast<const imtauth::IOidcClient*>(clientDataPtr.GetPtr());
		if (oidcClientPtr != nullptr){
			QStringList allowedUris = oidcClientPtr->GetRedirectUris();
			if (!allowedUris.contains(QString::fromUtf8(redirectUri))){
				return CreateErrorResponse("invalid_request", "redirect_uri not registered for this client",
							imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
			}
		}
	}

	// Generate authorization code
	QByteArray code = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	// Store authorization code
	if (m_authCodeCollectionCompPtr.IsValid()){
		imtauth::COidcAuthorizationCode authCode;
		authCode.SetCode(code);
		authCode.SetClientId(clientId);
		authCode.SetScope(QString::fromUtf8(scope));
		authCode.SetRedirectUri(QString::fromUtf8(redirectUri));
		authCode.SetExpirationDate(QDateTime::currentDateTimeUtc().addSecs(600)); // 10 minutes
		authCode.SetCodeChallenge(codeChallenge);
		authCode.SetCodeChallengeMethod(codeChallengeMethod);
		authCode.SetUsed(false);

		m_authCodeCollectionCompPtr->InsertNewObject("OidcAuthorizationCode", "", "", &authCode);
	}

	// Build redirect response
	QUrl redirectUrl(QString::fromUtf8(redirectUri));
	QUrlQuery query;
	query.addQueryItem("code", QString::fromUtf8(code));
	if (!state.isEmpty()){
		query.addQueryItem("state", QString::fromUtf8(state));
	}
	redirectUrl.setQuery(query);

	// Return redirect response
	QJsonObject redirectResponse;
	redirectResponse["redirect_uri"] = redirectUrl.toString();
	redirectResponse["code"] = QString::fromUtf8(code);

	return CreateJsonResponse(redirectResponse, imtrest::IProtocolEngine::SC_OK, request);
}


COidcServletComp::ConstResponsePtr COidcServletComp::HandleToken(
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	QMap<QByteArray, QByteArray> params = ParseFormUrlEncoded(request.GetBody());

	QByteArray grantType = params.value("grant_type");
	QByteArray clientId = params.value("client_id");
	QByteArray clientSecret = params.value("client_secret");

	// Try Basic auth if client credentials not in body
	if (clientId.isEmpty()){
		QByteArray authHeader = headers.value("Authorization");
		if (authHeader.startsWith("Basic ")){
			QByteArray decoded = QByteArray::fromBase64(authHeader.mid(6));
			int colonPos = decoded.indexOf(':');
			if (colonPos > 0){
				clientId = decoded.left(colonPos);
				clientSecret = decoded.mid(colonPos + 1);
			}
		}
	}

	if (clientId.isEmpty()){
		return CreateErrorResponse("invalid_client", "Client authentication required",
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	if (!ValidateClient(clientId, clientSecret)){
		return CreateErrorResponse("invalid_client", "Invalid client credentials",
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	if (grantType == "authorization_code"){
		return HandleAuthorizationCodeGrant(params, clientId, request);
	}
	else if (grantType == "client_credentials"){
		return HandleClientCredentialsGrant(params, clientId, request);
	}
	else if (grantType == "refresh_token"){
		return HandleRefreshTokenGrant(params, clientId, request);
	}

	return CreateErrorResponse("unsupported_grant_type", "Unsupported grant_type",
				imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
}


COidcServletComp::ConstResponsePtr COidcServletComp::HandleUserInfo(
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	QByteArray accessToken = ExtractBearerToken(headers);
	if (accessToken.isEmpty()){
		return CreateErrorResponse("invalid_token", "Bearer token required",
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	if (!m_jwtTokenProviderCompPtr.IsValid()){
		return CreateErrorResponse("server_error", "JWT provider not configured",
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	QJsonObject claims;
	if (!m_jwtTokenProviderCompPtr->ValidateToken(accessToken, claims)){
		return CreateErrorResponse("invalid_token", "Invalid or expired access token",
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	QByteArray userId = claims["sub"].toString().toUtf8();
	if (userId.isEmpty()){
		return CreateErrorResponse("invalid_token", "Token missing subject claim",
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	// Get scopes from the token
	QByteArrayList scopes;
	QString scopeStr = claims["scope"].toString();
	if (!scopeStr.isEmpty()){
		QStringList scopeList = scopeStr.split(' ');
		for (const QString& s : ::std::as_const(scopeList)){
			scopes.append(s.toUtf8());
		}
	}

	// Map scopes to claims
	QJsonObject userClaims;
	userClaims["sub"] = QString::fromUtf8(userId);

	if (m_scopeMapperCompPtr.IsValid()){
		QJsonObject mappedClaims = m_scopeMapperCompPtr->MapScopesToClaims(userId, scopes);
		for (auto it = mappedClaims.begin(); it != mappedClaims.end(); ++it){
			userClaims[it.key()] = it.value();
		}
	}

	return CreateJsonResponse(userClaims, imtrest::IProtocolEngine::SC_OK, request);
}


COidcServletComp::ConstResponsePtr COidcServletComp::HandleJwks(const imtrest::CHttpRequest& request) const
{
	if (!m_jwtTokenProviderCompPtr.IsValid()){
		return CreateErrorResponse("server_error", "JWT provider not configured",
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	QByteArray jwks = m_jwtTokenProviderCompPtr->GetJwks();
	if (jwks.isEmpty()){
		return CreateErrorResponse("server_error", "Failed to generate JWKS",
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	ConstResponsePtr retVal;
	retVal.SetPtr(request.GetProtocolEngine().CreateResponse(
				request,
				imtrest::IProtocolEngine::SC_OK,
				jwks,
				QByteArray("application/json; charset=utf-8")).PopInterfacePtr());

	return retVal;
}


COidcServletComp::ConstResponsePtr COidcServletComp::HandleRevoke(
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	QMap<QByteArray, QByteArray> params = ParseFormUrlEncoded(request.GetBody());
	QByteArray token = params.value("token");

	if (token.isEmpty()){
		return CreateErrorResponse("invalid_request", "Token parameter required",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	// Find and revoke the token in the collection
	if (m_tokenCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr tokenDataPtr;
		if (m_tokenCollectionCompPtr->GetObjectData(token, tokenDataPtr)){
			imtauth::IOidcTokenInfo* tokenInfoPtr = dynamic_cast<imtauth::IOidcTokenInfo*>(
				const_cast<istd::IChangeable*>(tokenDataPtr.GetPtr()));
			if (tokenInfoPtr != nullptr){
				tokenInfoPtr->SetRevoked(true);
				m_tokenCollectionCompPtr->UpdateObject(token, *tokenInfoPtr);
			}
		}
	}

	// RFC 7009: Always return 200 OK even if token not found
	QJsonObject response;
	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


COidcServletComp::ConstResponsePtr COidcServletComp::HandleIntrospect(
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	QMap<QByteArray, QByteArray> params = ParseFormUrlEncoded(request.GetBody());
	QByteArray token = params.value("token");

	if (token.isEmpty()){
		return CreateErrorResponse("invalid_request", "Token parameter required",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	QJsonObject introspection;
	introspection["active"] = false;

	if (m_jwtTokenProviderCompPtr.IsValid()){
		QJsonObject claims;
		if (m_jwtTokenProviderCompPtr->ValidateToken(token, claims)){
			introspection["active"] = true;
			introspection["sub"] = claims.value("sub");
			introspection["client_id"] = claims.value("client_id");
			introspection["scope"] = claims.value("scope");
			introspection["token_type"] = "Bearer";

			if (claims.contains("exp")){
				introspection["exp"] = claims["exp"];
			}
			if (claims.contains("iat")){
				introspection["iat"] = claims["iat"];
			}
			if (claims.contains("iss")){
				introspection["iss"] = claims["iss"];
			}

			// Check if token is revoked in storage
			if (m_tokenCollectionCompPtr.IsValid()){
				QByteArray tokenId = claims["jti"].toString().toUtf8();
				if (!tokenId.isEmpty()){
					imtbase::IObjectCollection::DataPtr tokenDataPtr;
					if (m_tokenCollectionCompPtr->GetObjectData(tokenId, tokenDataPtr)){
						const imtauth::IOidcTokenInfo* tokenInfoPtr =
							dynamic_cast<const imtauth::IOidcTokenInfo*>(tokenDataPtr.GetPtr());
						if (tokenInfoPtr != nullptr && tokenInfoPtr->IsRevoked()){
							introspection["active"] = false;
						}
					}
				}
			}
		}
	}

	return CreateJsonResponse(introspection, imtrest::IProtocolEngine::SC_OK, request);
}


// Helper methods

COidcServletComp::ConstResponsePtr COidcServletComp::CreateJsonResponse(
			const QJsonObject& json,
			imtrest::IProtocolEngine::StatusCode statusCode,
			const imtrest::CHttpRequest& request) const
{
	QByteArray payload = QJsonDocument(json).toJson(QJsonDocument::Compact);

	ConstResponsePtr retVal;
	retVal.SetPtr(request.GetProtocolEngine().CreateResponse(
				request,
				statusCode,
				payload,
				QByteArray("application/json; charset=utf-8")).PopInterfacePtr());

	return retVal;
}


COidcServletComp::ConstResponsePtr COidcServletComp::CreateErrorResponse(
			const QString& error,
			const QString& description,
			imtrest::IProtocolEngine::StatusCode statusCode,
			const imtrest::CHttpRequest& request) const
{
	QJsonObject errorJson;
	errorJson["error"] = error;
	errorJson["error_description"] = description;

	return CreateJsonResponse(errorJson, statusCode, request);
}


QByteArray COidcServletComp::ExtractBearerToken(const HeadersMap& headers) const
{
	QByteArray authHeader = headers.value("Authorization");
	if (authHeader.startsWith("Bearer ")){
		return authHeader.mid(7);
	}

	return QByteArray();
}


QMap<QByteArray, QByteArray> COidcServletComp::ParseFormUrlEncoded(const QByteArray& body) const
{
	QMap<QByteArray, QByteArray> params;
	QByteArrayList pairs = body.split('&');
	for (const QByteArray& pair : ::std::as_const(pairs)){
		int eqPos = pair.indexOf('=');
		if (eqPos > 0){
			QByteArray key = QByteArray::fromPercentEncoding(pair.left(eqPos));
			QByteArray value = QByteArray::fromPercentEncoding(pair.mid(eqPos + 1));
			params.insert(key, value);
		}
	}
	return params;
}


bool COidcServletComp::ValidateClient(const QByteArray& clientId, const QByteArray& clientSecret) const
{
	if (!m_clientCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr clientDataPtr;
	if (!m_clientCollectionCompPtr->GetObjectData(clientId, clientDataPtr)){
		return false;
	}

	const imtauth::IOidcClient* clientPtr = dynamic_cast<const imtauth::IOidcClient*>(clientDataPtr.GetPtr());
	if (clientPtr == nullptr){
		return false;
	}

	// If client has a secret hash, validate it
	QByteArray storedHash = clientPtr->GetClientSecretHash();
	if (!storedHash.isEmpty()){
		QByteArray providedHash = QCryptographicHash::hash(clientSecret, QCryptographicHash::Sha256).toHex();
		return storedHash == providedHash;
	}

	// Public client (no secret required)
	return clientSecret.isEmpty();
}


QByteArray COidcServletComp::ExtractCommandEndpoint(const QByteArray& commandId) const
{
	QByteArray cleanCommand = commandId;
	if (cleanCommand.startsWith('/')){
		cleanCommand = cleanCommand.mid(1);
	}
	if (cleanCommand.endsWith('/')){
		cleanCommand.chop(1);
	}

	// Handle .well-known/openid-configuration
	if (cleanCommand.contains("openid-configuration")){
		return "openid-configuration";
	}

	// Handle oauth/* endpoints
	int lastSlash = cleanCommand.lastIndexOf('/');
	if (lastSlash >= 0){
		return cleanCommand.mid(lastSlash + 1);
	}

	return cleanCommand;
}


// Token grant handlers

COidcServletComp::ConstResponsePtr COidcServletComp::HandleAuthorizationCodeGrant(
			const QMap<QByteArray, QByteArray>& params,
			const QByteArray& clientId,
			const imtrest::CHttpRequest& request) const
{
	QByteArray code = params.value("code");
	QByteArray redirectUri = params.value("redirect_uri");
	QByteArray codeVerifier = params.value("code_verifier");

	if (code.isEmpty()){
		return CreateErrorResponse("invalid_request", "Authorization code required",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	if (!m_authCodeCollectionCompPtr.IsValid()){
		return CreateErrorResponse("server_error", "Auth code collection not configured",
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	// Look up auth code
	imtbase::IObjectCollection::DataPtr codeDataPtr;
	if (!m_authCodeCollectionCompPtr->GetObjectData(code, codeDataPtr)){
		return CreateErrorResponse("invalid_grant", "Invalid authorization code",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	imtauth::IOidcAuthorizationCode* authCodePtr = dynamic_cast<imtauth::IOidcAuthorizationCode*>(
		const_cast<istd::IChangeable*>(codeDataPtr.GetPtr()));
	if (authCodePtr == nullptr){
		return CreateErrorResponse("invalid_grant", "Invalid authorization code",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	// Validate code
	if (authCodePtr->IsUsed()){
		return CreateErrorResponse("invalid_grant", "Authorization code already used",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	if (authCodePtr->GetExpirationDate() < QDateTime::currentDateTimeUtc()){
		return CreateErrorResponse("invalid_grant", "Authorization code expired",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	if (authCodePtr->GetClientId() != clientId){
		return CreateErrorResponse("invalid_grant", "Client ID mismatch",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	// PKCE verification
	QByteArray storedChallenge = authCodePtr->GetCodeChallenge();
	if (!storedChallenge.isEmpty()){
		if (codeVerifier.isEmpty()){
			return CreateErrorResponse("invalid_grant", "Code verifier required for PKCE",
						imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
		}

		QByteArray challengeMethod = authCodePtr->GetCodeChallengeMethod();
		QByteArray computedChallenge;
		if (challengeMethod == "S256"){
			computedChallenge = QCryptographicHash::hash(codeVerifier, QCryptographicHash::Sha256)
				.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
		}
		else{
			computedChallenge = codeVerifier;
		}

		if (computedChallenge != storedChallenge){
			return CreateErrorResponse("invalid_grant", "Code verifier does not match challenge",
						imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
		}
	}

	// Mark code as used
	authCodePtr->SetUsed(true);
	m_authCodeCollectionCompPtr->UpdateObject(code, *authCodePtr);

	// Generate tokens
	QByteArray userId = authCodePtr->GetUserId();
	QString scope = authCodePtr->GetScope();

	QByteArray tokenId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QByteArray refreshTokenId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QJsonObject accessClaims;
	accessClaims["sub"] = QString::fromUtf8(userId);
	accessClaims["client_id"] = QString::fromUtf8(clientId);
	accessClaims["scope"] = scope;
	accessClaims["jti"] = QString::fromUtf8(tokenId);
	accessClaims["token_type"] = "access_token";

	QByteArray accessToken = m_jwtTokenProviderCompPtr->GenerateToken(accessClaims);

	// Generate ID token
	QJsonObject idTokenClaims;
	idTokenClaims["sub"] = QString::fromUtf8(userId);
	idTokenClaims["aud"] = QString::fromUtf8(clientId);
	idTokenClaims["nonce"] = QString::fromUtf8(params.value("nonce"));

	// Add user claims based on scope
	if (m_scopeMapperCompPtr.IsValid()){
		QByteArrayList scopeList;
		QStringList scopeParts = scope.split(' ');
		for (const QString& s : ::std::as_const(scopeParts)){
			scopeList.append(s.toUtf8());
		}

		QJsonObject userClaims = m_scopeMapperCompPtr->MapScopesToClaims(userId, scopeList);
		for (auto it = userClaims.begin(); it != userClaims.end(); ++it){
			idTokenClaims[it.key()] = it.value();
		}
	}

	QByteArray idToken = m_jwtTokenProviderCompPtr->GenerateToken(idTokenClaims);

	// Generate refresh token
	QJsonObject refreshClaims;
	refreshClaims["sub"] = QString::fromUtf8(userId);
	refreshClaims["client_id"] = QString::fromUtf8(clientId);
	refreshClaims["scope"] = scope;
	refreshClaims["jti"] = QString::fromUtf8(refreshTokenId);
	refreshClaims["token_type"] = "refresh_token";
	refreshClaims["exp"] = QDateTime::currentSecsSinceEpoch() + 86400; // 24 hours

	QByteArray refreshToken = m_jwtTokenProviderCompPtr->GenerateToken(refreshClaims);

	// Store token info
	if (m_tokenCollectionCompPtr.IsValid()){
		imtauth::COidcTokenInfo tokenInfo;
		tokenInfo.SetTokenId(tokenId);
		tokenInfo.SetClientId(clientId);
		tokenInfo.SetUserId(userId);
		tokenInfo.SetScope(scope);
		tokenInfo.SetTokenType(imtauth::IOidcTokenInfo::OTT_ACCESS);
		tokenInfo.SetExpirationDate(QDateTime::currentDateTimeUtc().addSecs(3600));
		tokenInfo.SetCreatedAt(QDateTime::currentDateTimeUtc());
		m_tokenCollectionCompPtr->InsertNewObject("OidcToken", "", "", &tokenInfo);
	}

	QJsonObject response;
	response["access_token"] = QString::fromUtf8(accessToken);
	response["token_type"] = "Bearer";
	response["expires_in"] = 3600;
	response["refresh_token"] = QString::fromUtf8(refreshToken);
	response["id_token"] = QString::fromUtf8(idToken);
	response["scope"] = scope;

	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


COidcServletComp::ConstResponsePtr COidcServletComp::HandleClientCredentialsGrant(
			const QMap<QByteArray, QByteArray>& params,
			const QByteArray& clientId,
			const imtrest::CHttpRequest& request) const
{
	QString scope = QString::fromUtf8(params.value("scope"));

	QByteArray tokenId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QJsonObject accessClaims;
	accessClaims["sub"] = QString::fromUtf8(clientId);
	accessClaims["client_id"] = QString::fromUtf8(clientId);
	accessClaims["scope"] = scope;
	accessClaims["jti"] = QString::fromUtf8(tokenId);
	accessClaims["token_type"] = "access_token";

	QByteArray accessToken = m_jwtTokenProviderCompPtr->GenerateToken(accessClaims);

	// Store token info
	if (m_tokenCollectionCompPtr.IsValid()){
		imtauth::COidcTokenInfo tokenInfo;
		tokenInfo.SetTokenId(tokenId);
		tokenInfo.SetClientId(clientId);
		tokenInfo.SetScope(scope);
		tokenInfo.SetTokenType(imtauth::IOidcTokenInfo::OTT_ACCESS);
		tokenInfo.SetExpirationDate(QDateTime::currentDateTimeUtc().addSecs(3600));
		tokenInfo.SetCreatedAt(QDateTime::currentDateTimeUtc());
		m_tokenCollectionCompPtr->InsertNewObject("OidcToken", "", "", &tokenInfo);
	}

	QJsonObject response;
	response["access_token"] = QString::fromUtf8(accessToken);
	response["token_type"] = "Bearer";
	response["expires_in"] = 3600;
	if (!scope.isEmpty()){
		response["scope"] = scope;
	}

	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


COidcServletComp::ConstResponsePtr COidcServletComp::HandleRefreshTokenGrant(
			const QMap<QByteArray, QByteArray>& params,
			const QByteArray& clientId,
			const imtrest::CHttpRequest& request) const
{
	QByteArray refreshTokenJwt = params.value("refresh_token");
	if (refreshTokenJwt.isEmpty()){
		return CreateErrorResponse("invalid_request", "refresh_token parameter required",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	if (!m_jwtTokenProviderCompPtr.IsValid()){
		return CreateErrorResponse("server_error", "JWT provider not configured",
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	QJsonObject refreshClaims;
	if (!m_jwtTokenProviderCompPtr->ValidateToken(refreshTokenJwt, refreshClaims)){
		return CreateErrorResponse("invalid_grant", "Invalid or expired refresh token",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	if (refreshClaims["token_type"].toString() != "refresh_token"){
		return CreateErrorResponse("invalid_grant", "Token is not a refresh token",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	if (refreshClaims["client_id"].toString().toUtf8() != clientId){
		return CreateErrorResponse("invalid_grant", "Client ID mismatch",
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	// Generate new access token
	QByteArray userId = refreshClaims["sub"].toString().toUtf8();
	QString scope = refreshClaims["scope"].toString();
	QByteArray newTokenId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QJsonObject newAccessClaims;
	newAccessClaims["sub"] = QString::fromUtf8(userId);
	newAccessClaims["client_id"] = QString::fromUtf8(clientId);
	newAccessClaims["scope"] = scope;
	newAccessClaims["jti"] = QString::fromUtf8(newTokenId);
	newAccessClaims["token_type"] = "access_token";

	QByteArray newAccessToken = m_jwtTokenProviderCompPtr->GenerateToken(newAccessClaims);

	// Store new token
	if (m_tokenCollectionCompPtr.IsValid()){
		imtauth::COidcTokenInfo tokenInfo;
		tokenInfo.SetTokenId(newTokenId);
		tokenInfo.SetClientId(clientId);
		tokenInfo.SetUserId(userId);
		tokenInfo.SetScope(scope);
		tokenInfo.SetTokenType(imtauth::IOidcTokenInfo::OTT_ACCESS);
		tokenInfo.SetExpirationDate(QDateTime::currentDateTimeUtc().addSecs(3600));
		tokenInfo.SetCreatedAt(QDateTime::currentDateTimeUtc());
		m_tokenCollectionCompPtr->InsertNewObject("OidcToken", "", "", &tokenInfo);
	}

	QJsonObject response;
	response["access_token"] = QString::fromUtf8(newAccessToken);
	response["token_type"] = "Bearer";
	response["expires_in"] = 3600;
	response["scope"] = scope;

	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


} // namespace imtoidc


