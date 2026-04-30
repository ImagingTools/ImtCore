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
// Support both the current PAT prefix and the older prefix accepted by legacy callers.
// Keep the token cache short-lived: it avoids bursts of remote validation calls
// while limiting stale authorization data for both JWT and PAT tokens. Revocation
// becomes visible after the TTL unless the component is recreated earlier.
constexpr qint64 s_tokenCacheTtlMs = 5 * 60 * 1000;

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
	if (!token.isEmpty() && resolvedUserId.isEmpty()){
		imtgql::IGqlContextCreator::ContextCreationStatus authStatus = imtgql::IGqlContextCreator::CCS_OK;
		if (!ResolveUserId(token, resolvedUserId, errorMessage, authStatus)){
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
			QString& errorMessage,
			imtgql::IGqlContextCreator::ContextCreationStatus& status) const
{
	if (TryGetCachedToken(token, userId)){
		status = imtgql::IGqlContextCreator::CCS_OK;
		return true;
	}

	if (IsPatToken(token)){
		QByteArray tokenId;
		QByteArrayList scopes;
		{
			// Keep all calls into shared auth components serialized; their thread-safety is not guaranteed.
			QMutexLocker validationLocker(&m_tokenValidationMutex);
			if (!m_patManagerCompPtr.IsValid()){
				errorMessage = QStringLiteral("Personal access token manager is not configured.");
				status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
				return false;
			}

			if (!m_patManagerCompPtr->ValidateToken(token, userId, tokenId, scopes)){
				errorMessage = QStringLiteral("Invalid personal access token.");
				status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
				return false;
			}

			m_patManagerCompPtr->UpdateLastUsedAt(tokenId);
		}

		StoreCachedToken(token, userId, tokenId, true);
		return true;
	}

	{
		// Keep all calls into shared auth components serialized; their thread-safety is not guaranteed.
		QMutexLocker validationLocker(&m_tokenValidationMutex);
		if (!m_jwtSessionControllerCompPtr.IsValid()){
			errorMessage = QStringLiteral("JWT session controller is not configured.");
			status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
			return false;
		}

		using JwtState = imtauth::IJwtSessionController::JwtState;
		JwtState state = m_jwtSessionControllerCompPtr->ValidateJwt(token);
		if (state == JwtState::JS_EXPIRED){
			errorMessage = QStringLiteral("JWT token expired.");
			status = imtgql::IGqlContextCreator::CCS_UNAUTHORIZED;
			return false;
		}
		if (state == JwtState::JS_INVALID){
			errorMessage = QStringLiteral("Invalid JWT token.");
			status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
			return false;
		}

		userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(token);
	}

	StoreCachedToken(token, userId, QByteArray(), false);
	return true;
}


bool CGqlContextCreatorComp::TryGetCachedToken(const QByteArray& token, QByteArray& userId) const
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
	return true;
}


void CGqlContextCreatorComp::StoreCachedToken(
			const QByteArray& token,
			const QByteArray& userId,
			const QByteArray& tokenId,
			bool isPat) const
{
	TokenCacheEntry entry;
	entry.userId = userId;
	entry.tokenId = tokenId;
	entry.isPat = isPat;
	entry.expiresAt = QDateTime::currentMSecsSinceEpoch() + s_tokenCacheTtlMs;

	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokenCache.insert(token, entry);
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
