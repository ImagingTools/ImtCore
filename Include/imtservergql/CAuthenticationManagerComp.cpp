// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CAuthenticationManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QMutexLocker>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/IUserSettings.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtservergql
{


namespace
{

// Support both the current PAT prefix and the older prefix accepted by legacy callers.
constexpr const char* s_deprecatedPatPrefix = "pat_";
constexpr const char* s_patPrefix = "imt_pat_";
constexpr int s_deprecatedPatPrefixLength = 4;
constexpr int s_patPrefixLength = 8;
constexpr int s_maxTokenCacheSize = 10000;
// Keep the token cache short-lived: it avoids bursts of remote validation calls
// while limiting stale authorization data for both JWT and PAT tokens. Revocation
// becomes visible after the TTL unless the component is recreated earlier.
constexpr qint64 s_tokenCacheTtlMs = 5 * 60 * 1000;

} // namespace


// public methods

// reimplemented (imtgql::IGqlContextCreator)

imtgql::IGqlContextUniquePtr CAuthenticationManagerComp::CreateGqlContext(
			const imtgql::IGqlContext::Headers& headers,
			imtgql::IGqlContextCreator::ContextCreationError& error) const
{
	SetError(error, imtgql::IGqlContextCreator::CCS_OK);

	if (!m_gqlContextFactCompPtr.IsValid()){
		SetError(
					error,
					imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR,
					QStringLiteral("GraphQL context factory is not configured."));
		return nullptr;
	}

	const QByteArray token = headers.value(imtbase::s_authenticationTokenHeaderId);
	const QByteArray productId = headers.value(imtbase::s_productIdHeaderId);
	QByteArray resolvedUserId;
	QByteArray tenantId;
	QByteArrayList scopes;
	bool isPat = false;
	if (!token.isEmpty()){
		imtgql::IGqlContextCreator::ContextCreationStatus authStatus = imtgql::IGqlContextCreator::CCS_OK;
		QString authErrorMessage;
		if (!ResolveUserId(token, resolvedUserId, tenantId, scopes, isPat, authErrorMessage, authStatus)){
			SetError(error, authStatus, authErrorMessage);
			return nullptr;
		}
	}

	imtgql::IGqlContextUniquePtr gqlContextPtr = CreateContextInstance();
	if (!gqlContextPtr.IsValid()){
		SetError(
					error,
					imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR,
					QStringLiteral("Unable to create GraphQL context instance."));
		return nullptr;
	}

	gqlContextPtr->SetUserId(resolvedUserId);
	gqlContextPtr->SetTenantId(tenantId);
	gqlContextPtr->SetToken(token);
	gqlContextPtr->SetHeaders(headers);
	gqlContextPtr->SetProductId(productId);
	if (isPat){
		gqlContextPtr->SetScopes(scopes);
	}

	imtgql::CGqlRequestContextManager::SetContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr.GetPtr()));

	if (m_userCollectionCompPtr.IsValid() && !resolvedUserId.isEmpty()){
		imtbase::IObjectCollection::DataPtr userDataPtr;
		if (m_userCollectionCompPtr->GetObjectData(resolvedUserId, userDataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
			gqlContextPtr->SetUserInfo(userInfoPtr);
		}
	}

	if (m_userSettingsCollectionCompPtr.IsValid() && !resolvedUserId.isEmpty()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userSettingsCollectionCompPtr->GetObjectData(resolvedUserId, dataPtr)){
			imtauth::IUserSettings* userSettingsPtr = dynamic_cast<imtauth::IUserSettings*>(dataPtr.GetPtr());
			if (userSettingsPtr != nullptr){
				iprm::IParamsSet* paramsSetPtr = userSettingsPtr->GetSettings();
				if (paramsSetPtr != nullptr){
					iprm::TParamsPtr<iprm::ISelectionParam> languageParamPtr(paramsSetPtr, "Language");
					if (languageParamPtr.IsValid()){
						const iprm::IOptionsList* optionListPtr = languageParamPtr->GetSelectionConstraints();
						if (optionListPtr != nullptr){
							int index = languageParamPtr->GetSelectedOptionIndex();
							if (index >= 0){
								QByteArray languageId = optionListPtr->GetOptionId(index);
								gqlContextPtr->SetLanguageId(languageId);
							}
						}
					}

					iprm::TParamsPtr<iprm::ISelectionParam> designParamPtr(paramsSetPtr, "DesignSchema");
					if (designParamPtr.IsValid()){
						const iprm::IOptionsList* optionListPtr = designParamPtr->GetSelectionConstraints();
						if (optionListPtr != nullptr){
							int index = designParamPtr->GetSelectedOptionIndex();
							if (index >= 0){
								QByteArray designSchemeId = optionListPtr->GetOptionId(index);
								gqlContextPtr->SetDesignScheme(designSchemeId);
							}
						}
					}
				}
			}
		}
	}

	return gqlContextPtr;
}


// reimplemented (imtauth::IJwtSessionController)

bool CAuthenticationManagerComp::ValidateSession(const QByteArray& sessionId) const
{
	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return false;
	}

	return m_slaveJwtSessionControllerCompPtr->ValidateSession(sessionId);
}


imtauth::IJwtSessionController::JwtState CAuthenticationManagerComp::ValidateJwt(const QByteArray& jwt) const
{
	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return imtauth::IJwtSessionController::JS_INVALID;
	}

	// Check the cache first — avoid calling the slave controller if we already know the result.
	QByteArray userId;
	QByteArray tenantId;
	QByteArrayList scopes;
	bool isPat = false;
	if (TryGetCachedToken(jwt, userId, tenantId, scopes, isPat)){
		return imtauth::IJwtSessionController::JS_OK;
	}

	// Cache miss — delegate to the slave and populate the cache.
	JwtState state;
	{
		QMutexLocker locker(&m_tokenValidationMutex);
		state = m_slaveJwtSessionControllerCompPtr->ValidateJwt(jwt);
	}

	if (state == JwtState::JS_OK){
		QByteArray resolvedUserId;
		QByteArray resolvedTenantId;
		{
			QMutexLocker locker(&m_tokenValidationMutex);
			resolvedUserId = m_slaveJwtSessionControllerCompPtr->GetUserFromJwt(jwt);
			resolvedTenantId = m_slaveJwtSessionControllerCompPtr->GetTenantFromJwt(jwt);
		}
		StoreCachedToken(jwt, resolvedUserId, resolvedTenantId, QByteArray(), QByteArrayList(), false);
	}

	return state;
}


bool CAuthenticationManagerComp::RefreshToken(
			const QByteArray& refreshToken,
			imtauth::IJwtSessionController::UserSession& outputData) const
{
	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return false;
	}

	bool result = m_slaveJwtSessionControllerCompPtr->RefreshToken(refreshToken, outputData);
	if (result){
		// The old JWT is no longer valid — invalidate it from cache.
		// We cannot easily know the old JWT here, so we don't invalidate.
		// The new token will be cached on its first use.
	}

	return result;
}


bool CAuthenticationManagerComp::CreateNewSession(
			const QByteArray& userId,
			const QByteArray& tenantId,
			imtauth::IJwtSessionController::UserSession& outputData) const
{
	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return false;
	}

	bool result = m_slaveJwtSessionControllerCompPtr->CreateNewSession(userId, tenantId, outputData);
	if (result && !outputData.accessToken.isEmpty()){
		// Pre-populate the cache for the newly created JWT so subsequent
		// requests using this token hit the cache instead of calling the slave.
		StoreCachedToken(
					outputData.accessToken,
					outputData.userId,
					outputData.tenantId,
					QByteArray(),
					QByteArrayList(),
					false);
	}

	return result;
}


imtauth::ISessionSharedPtr CAuthenticationManagerComp::GetSession(const QByteArray& sessionId) const
{
	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return nullptr;
	}

	return m_slaveJwtSessionControllerCompPtr->GetSession(sessionId);
}


bool CAuthenticationManagerComp::RemoveSession(const QByteArray& sessionId) const
{
	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return false;
	}

	return m_slaveJwtSessionControllerCompPtr->RemoveSession(sessionId);
}


QByteArray CAuthenticationManagerComp::GetUserFromJwt(const QByteArray& jwt) const
{
	// Try cache first to avoid parsing the JWT again.
	QByteArray userId;
	QByteArray tenantId;
	QByteArrayList scopes;
	bool isPat = false;
	if (TryGetCachedToken(jwt, userId, tenantId, scopes, isPat)){
		return userId;
	}

	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return QByteArray();
	}

	return m_slaveJwtSessionControllerCompPtr->GetUserFromJwt(jwt);
}


QByteArray CAuthenticationManagerComp::GetTenantFromJwt(const QByteArray& jwt) const
{
	// Try cache first to avoid parsing the JWT again.
	QByteArray userId;
	QByteArray tenantId;
	QByteArrayList scopes;
	bool isPat = false;
	if (TryGetCachedToken(jwt, userId, tenantId, scopes, isPat)){
		return tenantId;
	}

	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return QByteArray();
	}

	return m_slaveJwtSessionControllerCompPtr->GetTenantFromJwt(jwt);
}


QByteArray CAuthenticationManagerComp::GetSessionFromJwt(const QByteArray& jwt) const
{
	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return QByteArray();
	}

	return m_slaveJwtSessionControllerCompPtr->GetSessionFromJwt(jwt);
}


// private methods

bool CAuthenticationManagerComp::ResolveUserId(
			const QByteArray& token,
			QByteArray& userId,
			QByteArray& tenantId,
			QByteArrayList& scopes,
			bool& isPat,
			QString& errorMessage,
			imtgql::IGqlContextCreator::ContextCreationStatus& status) const
{
	if (TryGetCachedToken(token, userId, tenantId, scopes, isPat)){
		status = imtgql::IGqlContextCreator::CCS_OK;
		return true;
	}

	if (IsPatToken(token)){
		tenantId.clear();
		QByteArray tokenId;
		bool isTokenValid = false;
		isPat = true;
		scopes.clear();
		{
			QMutexLocker locker(&m_tokenValidationMutex);
			if (!m_patManagerCompPtr.IsValid()){
				errorMessage = QStringLiteral("Personal access token manager is not configured.");
				status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
				return false;
			}

			if (!m_patManagerCompPtr->ValidateToken(token, userId, tokenId, scopes)){
				errorMessage = QStringLiteral("Invalid personal access token.");
				status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
			}
			else{
				isTokenValid = true;
				m_patManagerCompPtr->UpdateLastUsedAt(tokenId);
			}
		}

		if (!isTokenValid){
			return false;
		}

		StoreCachedToken(token, userId, tenantId, tokenId, scopes, true);
		return true;
	}

	isPat = false;
	using JwtState = imtauth::IJwtSessionController::JwtState;
	JwtState state = JwtState::JS_NONE;
	{
		QMutexLocker locker(&m_tokenValidationMutex);
		if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
			errorMessage = QStringLiteral("JWT session controller is not configured.");
			status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
			return false;
		}

		state = m_slaveJwtSessionControllerCompPtr->ValidateJwt(token);
		if (state == JwtState::JS_OK){
			userId = m_slaveJwtSessionControllerCompPtr->GetUserFromJwt(token);
			tenantId = m_slaveJwtSessionControllerCompPtr->GetTenantFromJwt(token);
		}
	}

	if (state == JwtState::JS_EXPIRED){
		errorMessage = QStringLiteral("JWT token expired.");
		status = imtgql::IGqlContextCreator::CCS_UNAUTHORIZED;
		return false;
	}
	if (state != JwtState::JS_OK){
		errorMessage = QStringLiteral("Invalid JWT token.");
		status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
		return false;
	}

	StoreCachedToken(token, userId, tenantId, QByteArray(), QByteArrayList(), false);
	return true;
}


bool CAuthenticationManagerComp::TryGetCachedToken(
			const QByteArray& token,
			QByteArray& userId,
			QByteArray& tenantId,
			QByteArrayList& scopes,
			bool& isPat) const
{
	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	auto iter = m_tokenCache.find(token);
	if (iter == m_tokenCache.end()){
		return false;
	}

	if (iter->expiresAt <= now){
		m_tokenCache.erase(iter);
		return false;
	}

	userId = iter->userId;
	tenantId = iter->tenantId;
	scopes = iter->scopes;
	isPat = iter->isPat;
	return true;
}


void CAuthenticationManagerComp::StoreCachedToken(
			const QByteArray& token,
			const QByteArray& userId,
			const QByteArray& tenantId,
			const QByteArray& tokenId,
			const QByteArrayList& scopes,
			bool isPat) const
{
	TokenCacheEntry entry;
	entry.userId = userId;
	entry.tenantId = tenantId;
	entry.tokenId = tokenId;
	entry.scopes = scopes;
	entry.isPat = isPat;
	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	entry.expiresAt = now + s_tokenCacheTtlMs;

	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokenCache.insert(token, entry);
	if (m_tokenCache.size() <= s_maxTokenCacheSize){
		return;
	}

	for (auto iter = m_tokenCache.begin(); iter != m_tokenCache.end(); ){
		if (iter->expiresAt <= now){
			iter = m_tokenCache.erase(iter);
		}
		else{
			++iter;
		}
	}

	if (m_tokenCache.size() > s_maxTokenCacheSize){
		m_tokenCache.clear();
	}
}


void CAuthenticationManagerComp::InvalidateTokenCache(const QByteArray& token) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokenCache.remove(token);
}


imtgql::IGqlContextUniquePtr CAuthenticationManagerComp::CreateContextInstance() const
{
	QMutexLocker locker(&m_contextCreationMutex);
	return m_gqlContextFactCompPtr.CreateInstance();
}


bool CAuthenticationManagerComp::IsPatToken(const QByteArray& token) const
{
	return (token.size() > s_deprecatedPatPrefixLength && token.startsWith(s_deprecatedPatPrefix))
			|| (token.size() > s_patPrefixLength && token.startsWith(s_patPrefix));
}


void CAuthenticationManagerComp::SetError(
			imtgql::IGqlContextCreator::ContextCreationError& error,
			imtgql::IGqlContextCreator::ContextCreationStatus status,
			const QString& message) const
{
	error.status = status;
	error.message = message;
}


} // namespace imtservergql
