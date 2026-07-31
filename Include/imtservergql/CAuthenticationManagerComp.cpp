// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CAuthenticationManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QMutexLocker>
#include <QtCore/QScopeGuard>
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
		SendWarningMessage(
					0,
					QStringLiteral("ValidateJwt: slave JWT session controller is not configured (token %1)")
							.arg(MaskToken(jwt)),
					QStringLiteral("CAuthenticationManagerComp"));
		return imtauth::IJwtSessionController::JS_INVALID;
	}

	QByteArray userId;
	QByteArray tenantId;
	QByteArrayList scopes;
	bool isPat = false;
	JwtState cachedState = JwtState::JS_NONE;
	if (TryAwaitOrClaimTokenResolution(jwt, userId, tenantId, scopes, isPat, cachedState)){
		return cachedState;
	}

	auto claimGuard = qScopeGuard([this, jwt](){ ReleaseTokenResolutionClaim(jwt); });
	JwtState state = m_slaveJwtSessionControllerCompPtr->ValidateJwt(jwt);

	if (state == JwtState::JS_OK){
		QByteArray resolvedUserId;
		QByteArray resolvedTenantId;
		QByteArray sessionId;
		qint64 expSecs = 0;
		// Prefer local claim decode to avoid extra remote round-trips
		// (GetUserFromJwt / GetTenantFromJwt) through CRemoteJwtSessionController.
		if (!ParseJwtClaims(jwt, resolvedUserId, resolvedTenantId, sessionId, expSecs)
					|| resolvedUserId.isEmpty()){
			resolvedUserId = m_slaveJwtSessionControllerCompPtr->GetUserFromJwt(jwt);
			resolvedTenantId = m_slaveJwtSessionControllerCompPtr->GetTenantFromJwt(jwt);
			expSecs = GetJwtExpirationSecs(jwt);
		}
		StoreCachedToken(jwt, resolvedUserId, resolvedTenantId, QByteArray(), QByteArrayList(), false, expSecs);
	}
	else if (state == JwtState::JS_EXPIRED){
		StoreExpiredToken(jwt);
	}
	else{
		SendWarningMessage(
					0,
					QStringLiteral("ValidateJwt: slave rejected token %1 with state %2")
							.arg(MaskToken(jwt))
							.arg(static_cast<int>(state)),
					QStringLiteral("CAuthenticationManagerComp"));
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
	// remains valid (and correctly cached) until its own 'exp' elapses.
	if (result && !outputData.accessToken.isEmpty()){
		QByteArray claimUserId;
		QByteArray claimTenantId;
		QByteArray sessionId;
		qint64 expSecs = 0;
		if (!ParseJwtClaims(outputData.accessToken, claimUserId, claimTenantId, sessionId, expSecs)){
			expSecs = GetJwtExpirationSecs(outputData.accessToken);
		}
		StoreCachedToken(
					outputData.accessToken,
					outputData.userId.isEmpty() ? claimUserId : outputData.userId,
					outputData.tenantId.isEmpty() ? claimTenantId : outputData.tenantId,
					QByteArray(),
					QByteArrayList(),
					false,
					expSecs);
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
		QByteArray claimUserId;
		QByteArray claimTenantId;
		QByteArray sessionId;
		qint64 expSecs = 0;
		if (!ParseJwtClaims(outputData.accessToken, claimUserId, claimTenantId, sessionId, expSecs)){
			expSecs = GetJwtExpirationSecs(outputData.accessToken);
		}
		StoreCachedToken(
					outputData.accessToken,
					outputData.userId.isEmpty() ? claimUserId : outputData.userId,
					outputData.tenantId.isEmpty() ? claimTenantId : outputData.tenantId,
					QByteArray(),
					QByteArrayList(),
					false,
					expSecs);
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

	QByteArray claimUserId;
	QByteArray claimTenantId;
	QByteArray sessionId;
	qint64 expSecs = 0;
	if (ParseJwtClaims(jwt, claimUserId, claimTenantId, sessionId, expSecs) && !claimUserId.isEmpty()){
		return claimUserId;
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

	QByteArray claimUserId;
	QByteArray claimTenantId;
	QByteArray sessionId;
	qint64 expSecs = 0;
	if (ParseJwtClaims(jwt, claimUserId, claimTenantId, sessionId, expSecs)){
		return claimTenantId;
	}

	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		return QByteArray();
	}

	return m_slaveJwtSessionControllerCompPtr->GetTenantFromJwt(jwt);
}


QByteArray CAuthenticationManagerComp::GetSessionFromJwt(const QByteArray& jwt) const
{
	QByteArray claimUserId;
	QByteArray claimTenantId;
	QByteArray sessionId;
	qint64 expSecs = 0;
	if (ParseJwtClaims(jwt, claimUserId, claimTenantId, sessionId, expSecs) && !sessionId.isEmpty()){
		return sessionId;
	}

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
	JwtState cachedState = JwtState::JS_NONE;
	if (TryAwaitOrClaimTokenResolution(token, userId, tenantId, scopes, isPat, cachedState)){
		if (cachedState == JwtState::JS_EXPIRED){
			errorMessage = QStringLiteral("JWT token expired.");
			status = imtgql::IGqlContextCreator::CCS_UNAUTHORIZED;
			return false;
		}
		status = imtgql::IGqlContextCreator::CCS_OK;
		return true;
	}

	// This thread now holds the exclusive claim (among concurrent callers) to
	// resolve this exact token. Release it - and wake any threads waiting on
	// it above - no matter which path below returns.
	auto claimGuard = qScopeGuard([this, token](){ ReleaseTokenResolutionClaim(token); });

	if (IsPatToken(token)){
		tenantId.clear();
		QByteArray tokenId;
		bool isTokenValid = false;
		isPat = true;
		scopes.clear();
		if (!m_patManagerCompPtr.IsValid()){
			errorMessage = QStringLiteral("Personal access token manager is not configured.");
			// Misconfiguration is an operational fault, not a client auth failure.
			status = imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
			SendErrorMessage(
						0,
						QStringLiteral("ResolveUserId: PAT manager not configured (token %1)")
								.arg(MaskToken(token)),
						QStringLiteral("CAuthenticationManagerComp"));
			return false;
		}

		if (!m_patManagerCompPtr->ValidateToken(token, userId, tokenId, scopes)){
			errorMessage = QStringLiteral("Invalid personal access token.");
			status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
			SendWarningMessage(
						0,
						QStringLiteral("ResolveUserId: invalid PAT %1")
								.arg(MaskToken(token)),
						QStringLiteral("CAuthenticationManagerComp"));
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
		status = imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
		SendErrorMessage(
					0,
					QStringLiteral("ResolveUserId: slave JWT session controller is not configured (token %1)")
							.arg(MaskToken(token)),
					QStringLiteral("CAuthenticationManagerComp"));
		return false;
	}

	state = m_slaveJwtSessionControllerCompPtr->ValidateJwt(token);
	qint64 expSecs = 0;
	if (state == JwtState::JS_OK){
		QByteArray sessionId;
		// Local claim decode avoids 2 extra remote GetUser/GetTenant round-trips
		// through CRemoteJwtSessionController on every cache miss.
		if (!ParseJwtClaims(token, userId, tenantId, sessionId, expSecs) || userId.isEmpty()){
			userId = m_slaveJwtSessionControllerCompPtr->GetUserFromJwt(token);
			tenantId = m_slaveJwtSessionControllerCompPtr->GetTenantFromJwt(token);
			expSecs = GetJwtExpirationSecs(token);
		}
	}

	if (state == JwtState::JS_EXPIRED){
		StoreExpiredToken(token);
		errorMessage = QStringLiteral("JWT token expired.");
		status = imtgql::IGqlContextCreator::CCS_UNAUTHORIZED;
		SendWarningMessage(
					0,
					QStringLiteral("ResolveUserId: token %1 expired (client should refresh)")
							.arg(MaskToken(token)),
					QStringLiteral("CAuthenticationManagerComp"));
		return false;
	}
	if (state == JwtState::JS_NONE){
		// Slave could not complete validation (typically a remote transport /
		// timeout / parse failure). This is NOT a permanent client credential
		// error — map to 500 so the client does not force-logout.
		errorMessage = QStringLiteral("JWT validation temporarily unavailable.");
		status = imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
		SendErrorMessage(
					0,
					QStringLiteral("ResolveUserId: slave returned JS_NONE for token %1 "
								"(remote validation failure / transport error) — returning INTERNAL_ERROR, not FORBIDDEN")
							.arg(MaskToken(token)),
					QStringLiteral("CAuthenticationManagerComp"));
		return false;
	}
	if (state != JwtState::JS_OK){
		errorMessage = QStringLiteral("Invalid JWT token.");
		status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
		SendWarningMessage(
					0,
					QStringLiteral("ResolveUserId: token %1 rejected as invalid (state %2)")
							.arg(MaskToken(token))
							.arg(static_cast<int>(state)),
					QStringLiteral("CAuthenticationManagerComp"));
		return false;
	}

	StoreCachedToken(token, userId, tenantId, QByteArray(), QByteArrayList(), false, expSecs);
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
		SendWarningMessage(
					0,
					QStringLiteral("Token cache miss (expired entry): token %1 expired %2 ms ago")
							.arg(MaskToken(token))
							.arg(now - iter->expiresAt),
					QStringLiteral("CAuthenticationManagerComp"));
		m_tokenCache.erase(iter);
		return false;
	}

	userId = iter->userId;
	tenantId = iter->tenantId;
	scopes = iter->scopes;
	isPat = iter->isPat;
	return true;
}


bool CAuthenticationManagerComp::TryReadCachedTokenLocked(
			const QByteArray& token,
			QByteArray& userId,
			QByteArray& tenantId,
			QByteArrayList& scopes,
			bool& isPat) const
{
	// Precondition: m_tokenCacheMutex is already held by the caller.
	const qint64 now = QDateTime::currentMSecsSinceEpoch();
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


bool CAuthenticationManagerComp::TryAwaitOrClaimTokenResolution(
			const QByteArray& token,
			QByteArray& userId,
			QByteArray& tenantId,
			QByteArrayList& scopes,
			bool& isPat,
			JwtState& state) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);

	while (true){
		if (TryReadCachedTokenLocked(token, userId, tenantId, scopes, isPat)){
			state = JwtState::JS_OK;
			return true;
		}

		const qint64 now = QDateTime::currentMSecsSinceEpoch();
		auto expiredIter = m_expiredTokenCache.find(token);
		if (expiredIter != m_expiredTokenCache.end()){
			if (*expiredIter > now){
				state = JwtState::JS_EXPIRED;
				return true;
			}
			m_expiredTokenCache.erase(expiredIter);
		}

		if (!m_tokensBeingResolved.contains(token)){
			m_tokensBeingResolved.insert(token);
			state = JwtState::JS_NONE;
			return false;
		}

		m_tokenResolutionWaitCondition.wait(&m_tokenCacheMutex);
	}
}


void CAuthenticationManagerComp::ReleaseTokenResolutionClaim(const QByteArray& token) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokensBeingResolved.remove(token);
	m_tokenResolutionWaitCondition.wakeAll();
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

	const qint64 tokenCacheTtlMs = m_tokenCacheTtlAttrPtr.IsValid() ? (static_cast<qint64>(*m_tokenCacheTtlAttrPtr) * 1000) : (30 * 1000);
	entry.expiresAt = now + tokenCacheTtlMs;

	if (jwtExpSecs > 0){
		const qint64 jwtExpMs = jwtExpSecs * 1000;
		if (jwtExpMs < entry.expiresAt){
			entry.expiresAt = jwtExpMs;
		}
	}

	const qint64 cacheTtlSecs = (entry.expiresAt - now) / 1000;

	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_expiredTokenCache.remove(token);
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

	// Prefer dropping a single arbitrary live entry over wiping the whole cache
	// (a full clear after idle causes a remote-validation storm that previously
	// surfaced as mass Forbidden when the slave returned JS_NONE under load).
	while (m_tokenCache.size() > maxTokenCacheSize){
		auto victim = m_tokenCache.begin();
		if (victim == m_tokenCache.end()){
			break;
		}
		m_tokenCache.erase(victim);
	}
}


void CAuthenticationManagerComp::StoreExpiredToken(const QByteArray& token) const
{
	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	const qint64 expiresAt = now + 10 * 1000;
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokenCache.remove(token);
	for (auto iter = m_expiredTokenCache.begin(); iter != m_expiredTokenCache.end(); ){
		if (*iter <= now){
			iter = m_expiredTokenCache.erase(iter);
		}
		else{
			++iter;
		}
	}
	m_expiredTokenCache.insert(token, expiresAt);

	const int maxTokenCacheSize = m_maxTokenCacheSizeAttrPtr.IsValid() ? *m_maxTokenCacheSizeAttrPtr : 10000;
	while (m_expiredTokenCache.size() > maxTokenCacheSize){
		auto victim = m_expiredTokenCache.begin();
		if (victim == m_expiredTokenCache.end()){
			break;
		}
		m_expiredTokenCache.erase(victim);
	}
}


void CAuthenticationManagerComp::InvalidateTokenCache(const QByteArray& token) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokenCache.remove(token);
	m_expiredTokenCache.remove(token);
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
	QByteArray userId;
	QByteArray tenantId;
	QByteArray sessionId;
	qint64 expSecs = 0;
	if (!ParseJwtClaims(jwt, userId, tenantId, sessionId, expSecs)){
		return 0;
	}

	return expSecs;
}


bool CAuthenticationManagerComp::ParseJwtClaims(
			const QByteArray& jwt,
			QByteArray& userId,
			QByteArray& tenantId,
			QByteArray& sessionId,
			qint64& expSecs) const
{
	userId.clear();
	tenantId.clear();
	sessionId.clear();
	expSecs = 0;

	const QByteArrayList parts = jwt.split('.');
	if (parts.size() != 3){
		return false;
	}

	const QByteArray json = QByteArray::fromBase64(
				parts[1],
				QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
	const QJsonObject payloadObj = QJsonDocument::fromJson(json).object();
	if (payloadObj.isEmpty()){
		return false;
	}

	if (payloadObj.contains(QStringLiteral("userId"))){
		userId = payloadObj.value(QStringLiteral("userId")).toString().toUtf8();
	}
	if (payloadObj.contains(QStringLiteral("tenantId"))){
		tenantId = payloadObj.value(QStringLiteral("tenantId")).toString().toUtf8();
	}
	if (payloadObj.contains(QStringLiteral("sessionId"))){
		sessionId = payloadObj.value(QStringLiteral("sessionId")).toString().toUtf8();
	}
	if (payloadObj.contains(QStringLiteral("exp"))){
		// Prefer double/long conversion over toInt() — toInt() truncates and
		// returns 0 for values outside 32-bit range.
		expSecs = static_cast<qint64>(payloadObj.value(QStringLiteral("exp")).toDouble());
	}

	return true;
}


QString CAuthenticationManagerComp::MaskToken(const QByteArray& token)
{
	if (token.isEmpty()){
		return QStringLiteral("<empty>");
	}

	return QStringLiteral("***") + QString::fromUtf8(token.right(4));
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
