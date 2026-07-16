// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CAuthenticationManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QMutexLocker>
#include <QtCore/QSet>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

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

	// Resolve tenant ownership flag. Cached alongside the rest of this token's
	// resolved claims (see TokenCacheEntry) instead of being looked up live on
	// every call - CreateGqlContext() runs on every single incoming request AND
	// every WebSocket subscription (re-)registration, and m_tenantManagerCompPtr
	// is a remote proxy (CRemoteTenantControllerComp) that hits the tenant
	// service over the network. Without this cache, a burst of requests (e.g.
	// the dozens fired right after a tenant switch: subscriptions re-registering,
	// per-open-document queries, page reload) each re-triggered their own
	// uncached GetTenant() round-trip - the exact flood seen after SelectTenant.
	if (m_tenantManagerCompPtr.IsValid() && !tenantId.isEmpty() && !resolvedUserId.isEmpty()){
		bool isTenantOwner = false;
		if (!TryGetCachedTenantOwnership(token, isTenantOwner)){
			imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
			isTenantOwner = tenantPtr.IsValid() && tenantPtr->GetOwnerId() == resolvedUserId;
			StoreCachedTenantOwnership(token, isTenantOwner);
		}

		if (isTenantOwner){
			gqlContextPtr->SetIsTenantOwner(true);
		}
	}

	imtgql::CGqlRequestContextManager::SetContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr.GetPtr()));

	if (m_userCollectionCompPtr.IsValid() && !resolvedUserId.isEmpty()){
		imtbase::IObjectCollection::DataPtr userDataPtr;
		if (m_userCollectionCompPtr->GetObjectData(resolvedUserId, userDataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());

			if (isPat && userInfoPtr != nullptr){
				// For PAT: restrict the user's permissions to the intersection with the
				// token's scopes. GetPermissions() now returns role- and local-permission
				// derived permissions combined, so writing the intersection into local
				// permissions (after stripping roles/groups) is enough to make
				// GetPermissions() - the method every permission check in this codebase
				// actually reads - report exactly that intersection. An empty scope list
				// yields zero permissions (fail closed), not the full unrestricted user.
				imtauth::IUserInfo::FeatureIds fullPermissions = userInfoPtr->GetPermissions(productId);
				QSet<QByteArray> scopeSet(scopes.begin(), scopes.end());
				imtauth::IUserInfo::FeatureIds effectivePermissions;
				for (const QByteArray& perm : fullPermissions){
					if (scopeSet.contains(perm)){
						effectivePermissions.append(perm);
					}
				}

				istd::IChangeableUniquePtr clonedPtr = userInfoPtr->CloneMe();
				imtauth::IUserInfo* mutableUserPtr = dynamic_cast<imtauth::IUserInfo*>(clonedPtr.GetPtr());
				if (mutableUserPtr != nullptr){
					// Strip roles/groups so no other code path can expand them back into
					// unrestricted permissions; only the explicit scoped list applies.
					const QByteArrayList products = mutableUserPtr->GetProducts();
					for (const QByteArray& prod : products){
						mutableUserPtr->SetRoles(prod, imtauth::IUserBaseInfo::RoleIds());
					}
					const imtauth::IUserGroupInfo::GroupIds groupIds = mutableUserPtr->GetGroups();
					for (const QByteArray& groupId : groupIds){
						mutableUserPtr->RemoveFromGroup(groupId);
					}
					mutableUserPtr->SetLocalPermissions(productId, effectivePermissions);

					gqlContextPtr->SetUserInfo(mutableUserPtr);
				}
				else{
					gqlContextPtr->SetUserInfo(nullptr);
				}
			}
			else{
				gqlContextPtr->SetUserInfo(userInfoPtr);
			}
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
	JwtState state = m_slaveJwtSessionControllerCompPtr->ValidateJwt(jwt);

	if (state == JwtState::JS_OK){
		QByteArray resolvedUserId = m_slaveJwtSessionControllerCompPtr->GetUserFromJwt(jwt);
		QByteArray resolvedTenantId = m_slaveJwtSessionControllerCompPtr->GetTenantFromJwt(jwt);
		StoreCachedToken(jwt, resolvedUserId, resolvedTenantId, QByteArray(), QByteArrayList(), false, GetJwtExpirationSecs(jwt));
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
	// Note: the slave rotates the session's refresh token but keeps the same
	// session-ID and updates it in place, so a JWT issued before this refresh
	// remains valid (and correctly cached) until its own 'exp' elapses. There
	// is nothing to invalidate here; the new token will be cached on first use.

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
					false,
					GetJwtExpirationSecs(outputData.accessToken));
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


void CAuthenticationManagerComp::InvalidateToken(const QByteArray& token) const
{
	if (token.isEmpty()){
		return;
	}

	InvalidateTokenCache(token);

	if (m_slaveJwtSessionControllerCompPtr.IsValid()){
		m_slaveJwtSessionControllerCompPtr->InvalidateToken(token);
	}
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

		if (!isTokenValid){
			return false;
		}

		StoreCachedToken(token, userId, tenantId, tokenId, scopes, true);
		return true;
	}

	isPat = false;
	using JwtState = imtauth::IJwtSessionController::JwtState;
	JwtState state = JwtState::JS_NONE;
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

	StoreCachedToken(token, userId, tenantId, QByteArray(), QByteArrayList(), false, GetJwtExpirationSecs(token));
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
			bool isPat,
			qint64 jwtExpSecs) const
{
	TokenCacheEntry entry;
	entry.userId = userId;
	entry.tenantId = tenantId;
	entry.tokenId = tokenId;
	entry.scopes = scopes;
	entry.isPat = isPat;
	const qint64 now = QDateTime::currentMSecsSinceEpoch();

	const qint64 tokenCacheTtlMs = m_tokenCacheTtlAttrPtr.IsValid() ? (static_cast<qint64>(*m_tokenCacheTtlAttrPtr) * 1000) : (5 * 60 * 1000);
	entry.expiresAt = now + tokenCacheTtlMs;

	// Never let the cache consider a JWT valid past its own 'exp' claim -
	// otherwise an already-expired token could still be accepted for up to
	// TokenCacheLifetime after it actually expired.
	if (jwtExpSecs > 0){
		entry.expiresAt = std::min(entry.expiresAt, jwtExpSecs * 1000);
	}

	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokenCache.insert(token, entry);
	const int maxTokenCacheSize = m_maxTokenCacheSizeAttrPtr.IsValid() ? *m_maxTokenCacheSizeAttrPtr : 10000;
	if (m_tokenCache.size() <= maxTokenCacheSize){
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

	if (m_tokenCache.size() > maxTokenCacheSize){
		m_tokenCache.clear();
	}
}


void CAuthenticationManagerComp::InvalidateTokenCache(const QByteArray& token) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokenCache.remove(token);
}


bool CAuthenticationManagerComp::TryGetCachedTenantOwnership(const QByteArray& token, bool& isTenantOwner) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	auto iter = m_tokenCache.find(token);
	if (iter == m_tokenCache.end() || !iter->tenantOwnershipResolved){
		return false;
	}

	isTenantOwner = iter->isTenantOwner;
	return true;
}


void CAuthenticationManagerComp::StoreCachedTenantOwnership(const QByteArray& token, bool isTenantOwner) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	auto iter = m_tokenCache.find(token);
	if (iter == m_tokenCache.end()){
		// ResolveUserId() always caches the token before this is reached; a
		// missing entry here means it expired/was invalidated in the tiny
		// window since then. Nothing to attach the flag to - next call will
		// simply resolve (and cache) it again from scratch.
		return;
	}

	iter->isTenantOwner = isTenantOwner;
	iter->tenantOwnershipResolved = true;
}


imtgql::IGqlContextUniquePtr CAuthenticationManagerComp::CreateContextInstance() const
{
	return m_gqlContextFactCompPtr.CreateInstance();
}


bool CAuthenticationManagerComp::IsPatToken(const QByteArray& token) const
{
	const QByteArray& patPrefix = m_patPrefixAttrPtr.IsValid() ? *m_patPrefixAttrPtr : QByteArrayLiteral("imt_pat_");
	return !patPrefix.isEmpty() && token.size() > patPrefix.size() && token.startsWith(patPrefix);
}


qint64 CAuthenticationManagerComp::GetJwtExpirationSecs(const QByteArray& jwt) const
{
	QByteArrayList parts = jwt.split('.');
	if (parts.size() != 3){
		return 0;
	}

	QByteArray json = QByteArray::fromBase64(
				parts[1],
				QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
	QJsonObject payloadObj = QJsonDocument::fromJson(json).object();
	if (!payloadObj.contains("exp")){
		return 0;
	}

	return static_cast<qint64>(payloadObj["exp"].toDouble());
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
