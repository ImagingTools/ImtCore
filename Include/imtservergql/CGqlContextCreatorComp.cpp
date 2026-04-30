// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlContextCreatorComp.h>


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

constexpr const char* s_deprecatedPatPrefix = "pat_";
constexpr const char* s_patPrefix = "imt_pat_";
constexpr int s_deprecatedPatPrefixLength = 4;
constexpr int s_patPrefixLength = 8;
constexpr int s_maxTokenCacheSize = 10000;
// Support both the current PAT prefix and the older prefix accepted by legacy callers.
// Keep the token cache short-lived: it avoids bursts of remote validation calls
// while limiting stale authorization data for both JWT and PAT tokens. Revocation
// becomes visible after the TTL unless the component is recreated earlier.
constexpr qint64 s_tokenCacheTtlMs = 5 * 60 * 1000;
constexpr qint64 s_negativeTokenCacheTtlMs = 30 * 1000;

} // namespace


// public methods

// reimplemented (imtgql::IGqlContextCreator)

imtgql::IGqlContextUniquePtr CGqlContextCreatorComp::CreateGqlContext(
			const QByteArray& token,
			const QByteArray& productId,
			const QByteArray& userId,
			const imtgql::IGqlContext::Headers& headers,
			QString& errorMessage,
			imtgql::IGqlContextCreator::ContextCreationStatus* statusPtr) const
{
	SetStatus(statusPtr, imtgql::IGqlContextCreator::CCS_OK);

	if (!m_gqlContextFactCompPtr.IsValid()){
		errorMessage = QStringLiteral("GraphQL context factory is not configured.");
		SetStatus(statusPtr, imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR);
		return nullptr;
	}

	QByteArray resolvedUserId = userId;
	QByteArrayList scopes;
	bool isPat = false;
	if (!token.isEmpty() && resolvedUserId.isEmpty()){
		imtgql::IGqlContextCreator::ContextCreationStatus authStatus = imtgql::IGqlContextCreator::CCS_OK;
		if (!ResolveUserId(token, resolvedUserId, scopes, isPat, errorMessage, authStatus)){
			SetStatus(statusPtr, authStatus);
			return nullptr;
		}
	}

	imtgql::IGqlContextUniquePtr gqlContextPtr = CreateContextInstance();
	if (!gqlContextPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create GraphQL context instance.");
		SetStatus(statusPtr, imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR);
		return nullptr;
	}

	gqlContextPtr->SetUserId(resolvedUserId);
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


// private methods

bool CGqlContextCreatorComp::ResolveUserId(
			const QByteArray& token,
			QByteArray& userId,
			QByteArrayList& scopes,
			bool& isPat,
			QString& errorMessage,
			imtgql::IGqlContextCreator::ContextCreationStatus& status) const
{
	TokenCacheLookupResult lookupResult = TryGetCachedToken(token, userId, scopes, isPat, errorMessage, status);
	if (lookupResult == TCLR_VALID){
		status = imtgql::IGqlContextCreator::CCS_OK;
		return true;
	}
	if (lookupResult == TCLR_INVALID){
		return false;
	}

	if (IsPatToken(token)){
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
			StoreCachedToken(token, QByteArray(), QByteArray(), QByteArrayList(), true, false, status, s_negativeTokenCacheTtlMs);
			return false;
		}

		StoreCachedToken(token, userId, tokenId, scopes, true, true, imtgql::IGqlContextCreator::CCS_OK, s_tokenCacheTtlMs);
		return true;
	}

	isPat = false;
	using JwtState = imtauth::IJwtSessionController::JwtState;
	JwtState state = JwtState::JS_NONE;
	{
		QMutexLocker locker(&m_tokenValidationMutex);
		if (!m_jwtSessionControllerCompPtr.IsValid()){
			errorMessage = QStringLiteral("JWT session controller is not configured.");
			status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
			return false;
		}

		state = m_jwtSessionControllerCompPtr->ValidateJwt(token);
		if (state != JwtState::JS_EXPIRED && state != JwtState::JS_INVALID){
			userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(token);
		}
	}

	if (state == JwtState::JS_EXPIRED){
		errorMessage = QStringLiteral("JWT token expired.");
		status = imtgql::IGqlContextCreator::CCS_UNAUTHORIZED;
		StoreCachedToken(token, QByteArray(), QByteArray(), QByteArrayList(), false, false, status, s_negativeTokenCacheTtlMs);
		return false;
	}
	if (state == JwtState::JS_INVALID){
		errorMessage = QStringLiteral("Invalid JWT token.");
		status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
		StoreCachedToken(token, QByteArray(), QByteArray(), QByteArrayList(), false, false, status, s_negativeTokenCacheTtlMs);
		return false;
	}

	StoreCachedToken(token, userId, QByteArray(), QByteArrayList(), false, true, imtgql::IGqlContextCreator::CCS_OK, s_tokenCacheTtlMs);
	return true;
}


CGqlContextCreatorComp::TokenCacheLookupResult CGqlContextCreatorComp::TryGetCachedToken(
			const QByteArray& token,
			QByteArray& userId,
			QByteArrayList& scopes,
			bool& isPat,
			QString& errorMessage,
			imtgql::IGqlContextCreator::ContextCreationStatus& status) const
{
	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	auto iter = m_tokenCache.find(token);
	if (iter == m_tokenCache.end()){
		return TCLR_MISS;
	}

	if (iter->expiresAt <= now){
		m_tokenCache.erase(iter);
		return TCLR_MISS;
	}

	if (!iter->isValid || iter->userId.isEmpty()){
		isPat = iter->isPat;
		status = iter->isValid ? imtgql::IGqlContextCreator::CCS_FORBIDDEN : iter->status;
		errorMessage = status == imtgql::IGqlContextCreator::CCS_UNAUTHORIZED
				? QStringLiteral("JWT token expired.")
				: QStringLiteral("Invalid authentication token.");
		return TCLR_INVALID;
	}

	userId = iter->userId;
	scopes = iter->scopes;
	isPat = iter->isPat;
	return TCLR_VALID;
}


void CGqlContextCreatorComp::StoreCachedToken(
			const QByteArray& token,
			const QByteArray& userId,
			const QByteArray& tokenId,
			const QByteArrayList& scopes,
			bool isPat,
			bool isValid,
			imtgql::IGqlContextCreator::ContextCreationStatus status,
			qint64 ttlMs) const
{
	TokenCacheEntry entry;
	entry.userId = userId;
	entry.tokenId = tokenId;
	entry.scopes = scopes;
	entry.isPat = isPat;
	entry.isValid = isValid;
	entry.status = status;
	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	entry.expiresAt = now + ttlMs;

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


imtgql::IGqlContextUniquePtr CGqlContextCreatorComp::CreateContextInstance() const
{
	QMutexLocker locker(&m_contextCreationMutex);
	return m_gqlContextFactCompPtr.CreateInstance();
}


bool CGqlContextCreatorComp::IsPatToken(const QByteArray& token) const
{
	return (token.size() > s_deprecatedPatPrefixLength && token.startsWith(s_deprecatedPatPrefix))
			|| (token.size() > s_patPrefixLength && token.startsWith(s_patPrefix));
}


void CGqlContextCreatorComp::SetStatus(
			imtgql::IGqlContextCreator::ContextCreationStatus* statusPtr,
			imtgql::IGqlContextCreator::ContextCreationStatus status) const
{
	if (statusPtr != nullptr){
		*statusPtr = status;
	}
}


} // namespace imtservergql
