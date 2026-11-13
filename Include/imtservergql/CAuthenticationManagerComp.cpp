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
#include <imtbase/IIdentifiable.h>
#include <imtauth/CollectionSelection.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/imtauth.h>
#include <imtauth/IUserSettings.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtgql/imtgql.h>
#include <imtservergql/CAuthorizationSnapshot.h>
#include <imtservergql/imtservergql.h>


namespace imtservergql
{


static imtgql::IGqlContextCreator::ContextCreationStatus ToContextCreationStatus(imtauth::SessionTokenCheck check)
{
	switch (check){
	case imtauth::STC_VALID:
		return imtgql::IGqlContextCreator::CCS_OK;
	case imtauth::STC_REAUTHENTICATE:
		return imtgql::IGqlContextCreator::CCS_UNAUTHORIZED;
	case imtauth::STC_INVALID:
		return imtgql::IGqlContextCreator::CCS_FORBIDDEN;
	default:
		return imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
	}
}


// What a user of a product without a token of its server is told
static QString GetNotConnectedMessage(const QByteArray& productId, const QByteArray& receiverProductId)
{
	return QStringLiteral("%1 is not connected to %2: its server has no valid server token. An administrator of %2 has to issue one for %1.")
				.arg(QString::fromUtf8(productId), QString::fromUtf8(receiverProductId));
}


// public methods

// reimplemented (imtgql::IGqlContextCreator)

imtgql::IGqlContextUniquePtr CAuthenticationManagerComp::CreateGqlContext(
			const imtgql::IGqlContext::Headers& headers,
			imtgql::IGqlContextCreator::ContextCreationError& error) const
{
	return CreateContext(headers, CK_REQUEST, error);
}


imtgql::IGqlContextUniquePtr CAuthenticationManagerComp::RenewGqlContext(
			const imtgql::IGqlContext::Headers& headers,
			bool isEstablished,
			AuthorizationState& state,
			ContextCreationError& error,
			QDateTime* sessionExpiresAtPtr) const
{
	const qint64 startGeneration = GetAuthorizationGeneration();

	imtgql::IGqlContextUniquePtr gqlContextPtr = CreateContext(headers, isEstablished ? CK_ESTABLISHED_SUBSCRIPTION : CK_NEW_SUBSCRIPTION, error, sessionExpiresAtPtr);
	imtgql::CGqlRequestContextManager::Clear();

	// Whatever was decided, it was decided on data that changed meanwhile
	if (GetAuthorizationGeneration() != startGeneration){
		state = AS_STALE;
		SetError(error, CCS_INTERNAL_ERROR, QStringLiteral("The access changed during the check."));

		return nullptr;
	}

	if (!gqlContextPtr.IsValid()){
		state = error.status == CCS_INTERNAL_ERROR ? AS_UNAVAILABLE : AS_DENIED;

		return nullptr;
	}

	state = AS_CONFIRMED;

	return gqlContextPtr;
}


qint64 CAuthenticationManagerComp::GetAuthorizationGeneration() const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);

	return m_tokenChangeGeneration;
}


imtgql::IGqlContextUniquePtr CAuthenticationManagerComp::CreateContext(
			const imtgql::IGqlContext::Headers& headers,
			ContextKind kind,
			imtgql::IGqlContextCreator::ContextCreationError& error,
			QDateTime* sessionExpiresAtPtr) const
{
	SetError(error, imtgql::IGqlContextCreator::CCS_OK);
	if (sessionExpiresAtPtr != nullptr){
		*sessionExpiresAtPtr = QDateTime();
	}
	QByteArray sessionId;

	// A subscription is checked on the current state: no cache, every source read afresh
	const bool isRenewal = kind != CK_REQUEST;

	if (!m_gqlContextFactCompPtr.IsValid()){
		SetError(
					error,
					imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR,
					QStringLiteral("GraphQL context factory is not configured."));
		return nullptr;
	}

	const QByteArray token = headers.value(imtbase::s_authenticationTokenHeaderId);
	const QByteArray headerProductId = headers.value(imtbase::s_productIdHeaderId);

	// A server of one product checks its own permissions whatever product the client names.
	QByteArray productId = m_productInfoCompPtr.IsValid() ? m_productInfoCompPtr->GetProductId() : QByteArray();
	if (productId.isEmpty()){
		productId = headerProductId;
	}
	QByteArray resolvedUserId;
	QByteArray tenantId;
	QByteArray clientProductId;
	QByteArray userTokenProductId;
	QByteArrayList scopes;
	bool isPat = false;
	const bool isSessionAuthorized = !token.isEmpty() && !IsPatToken(token) && IsComputedBySource();
	imtauth::ISessionAuthorizationProvider::SessionAuthorization sessionAuthorization;
	const IServerTokenReceiver* receiverPtr = GetServerTokenReceiver();
	const QByteArray ownProductId = (receiverPtr != nullptr) ? receiverPtr->GetServerProductId() : QByteArray();
	if (isSessionAuthorized){
		// A server that receives no servers of other products takes only the sessions opened through it
		if (ownProductId.isEmpty() && m_productInfoCompPtr.IsValid() && imtauth::GetJwtProductId(token) != productId){
			SetError(error, CCS_FORBIDDEN, QStringLiteral("The session was not opened through this server and does not work here."));
			return nullptr;
		}

		// A request may use an answer a few seconds old; a subscription is checked on the current state
		imtauth::SessionTokenClaims claims;
		const bool hasClaims = imtauth::ParseSessionTokenClaims(token, claims);
		const qint64 resolutionGeneration = GetAuthorizationGeneration();
		bool isClaimed = false;
		const bool isCached = !isRenewal && TryAwaitOrClaimSessionAuthorization(token, sessionAuthorization, isClaimed);
		if (!isCached){
			sessionAuthorization = m_sessionAuthorizationProviderCompPtr->GetSessionAuthorization(token, productId, kind == CK_ESTABLISHED_SUBSCRIPTION);
		}

		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(sessionAuthorization.userPtr.GetPtr());
		const imtbase::IIdentifiable* identifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(sessionAuthorization.userPtr.GetPtr());
		const bool isComputed = sessionAuthorization.state == imtauth::ISessionAuthorizationProvider::SAS_COMPUTED;
		const bool isAnswerValid = isComputed && hasClaims && !sessionAuthorization.userId.isEmpty()
					&& sessionAuthorization.userId == claims.userId && sessionAuthorization.tenantId == claims.tenantId
					&& userInfoPtr != nullptr && identifiablePtr != nullptr
					&& identifiablePtr->GetObjectUuid() == sessionAuthorization.userId && userInfoPtr->IsEnabled();
		if (!isCached){
			if (isAnswerValid){
				CAuthorizationSnapshot::DetachUser(*sessionAuthorization.userPtr.GetPtr());
				if (!isRenewal){
					StoreCachedSessionAuthorization(token, sessionAuthorization, claims.expSecs, resolutionGeneration);
				}
			}
			if (isClaimed){
				ReleaseTokenResolutionClaim(token);
			}
		}

		if (!isComputed){
			if (sessionAuthorization.state == imtauth::ISessionAuthorizationProvider::SAS_REFUSED){
				if (sessionAuthorization.refusalReason == imtauth::ISessionAuthorizationProvider::RR_REAUTHENTICATE){
					InvalidateTokenCache(token);
				}
				SetError(error, sessionAuthorization.refusalReason == imtauth::ISessionAuthorizationProvider::RR_REAUTHENTICATE ? CCS_UNAUTHORIZED : CCS_FORBIDDEN, sessionAuthorization.message);
			}
			else{
				SetError(error, CCS_INTERNAL_ERROR, sessionAuthorization.message);
			}
			return nullptr;
		}

		if (!isAnswerValid){
			SetError(error, CCS_INTERNAL_ERROR, QStringLiteral("The authorization server answered with an invalid user or session."));
			return nullptr;
		}
		resolvedUserId = sessionAuthorization.userId;
		tenantId = sessionAuthorization.tenantId;
	}
	else if (!token.isEmpty()){
		QByteArray tokenProductId;
		bool isTokenAccepted = true;
		if (isRenewal){
			// The token of a server is known only where it was issued
			const imtauth::IPersonalAccessTokenManager* managerPtr = imtauth::IsServerToken(token)
						? ((receiverPtr != nullptr) ? receiverPtr->GetTokenManager() : nullptr)
						: m_patManagerCompPtr.GetPtr();
			FreshToken freshToken;
			if (!ResolveFreshToken(token, managerPtr, kind == CK_ESTABLISHED_SUBSCRIPTION, freshToken, error)){
				return nullptr;
			}

			resolvedUserId = freshToken.userId;
			tenantId = freshToken.tenantId;
			scopes = freshToken.scopes;
			isPat = freshToken.isPat;
			sessionId = freshToken.sessionId;
			tokenProductId = freshToken.productId;
			clientProductId = freshToken.clientProductId;
		}
		else{
			imtgql::IGqlContextCreator::ContextCreationStatus authStatus = imtgql::IGqlContextCreator::CCS_OK;
			QString authErrorMessage;
			if (!ResolveUserId(token, resolvedUserId, tenantId, scopes, isPat, authErrorMessage, authStatus)){
				SetError(error, authStatus, authErrorMessage);
				return nullptr;
			}

			isTokenAccepted = !isPat || GetCachedPatProducts(token, tokenProductId, clientProductId);
		}

		if (isPat){
			// A token is of the kind its prefix names
			isTokenAccepted = isTokenAccepted && imtauth::IsServerToken(token) == !clientProductId.isEmpty();
			if (isTokenAccepted && !clientProductId.isEmpty()){
				// The token of a server names the server accepting it and the product using it
				isTokenAccepted = tokenProductId == ownProductId && !ownProductId.isEmpty() && (headerProductId.isEmpty() || headerProductId == clientProductId);
				productId = clientProductId;
			}
			else if (isTokenAccepted && !tokenProductId.isEmpty()){
				// A token works only for its product, whatever product the client names
				isTokenAccepted = (!m_productInfoCompPtr.IsValid() || tokenProductId == productId) && (headerProductId.isEmpty() || tokenProductId == headerProductId);
				userTokenProductId = tokenProductId;
				productId = tokenProductId;
			}

			if (!isTokenAccepted){
				SetError(error, imtgql::IGqlContextCreator::CCS_FORBIDDEN, QStringLiteral("The personal access token is not valid for this product."));
				return nullptr;
			}
		}
	}

	// A request of another product comes through the server of that product, which proves it with its own token
	QByteArray serverProductId = clientProductId;
	QByteArrayList serverScopes = clientProductId.isEmpty() ? QByteArrayList() : scopes;
	QByteArray foreignProductId;
	if (clientProductId.isEmpty() && !ownProductId.isEmpty()){
		const QByteArray serverToken = headers.value(imtbase::s_serverTokenHeaderId);
		if (!serverToken.isEmpty()){
			if (!ResolveServerToken(serverToken, isRenewal, serverProductId, serverScopes, error)){
				return nullptr;
			}

			if (!headerProductId.isEmpty() && headerProductId != serverProductId){
				SetError(error, imtgql::IGqlContextCreator::CCS_FORBIDDEN, QStringLiteral("The token of the server is not the token of the product '%1'.").arg(QString::fromUtf8(headerProductId)));
				return nullptr;
			}

			// A personal token of a user goes only through the server of its product
			if (!userTokenProductId.isEmpty() && userTokenProductId != serverProductId){
				SetError(error, imtgql::IGqlContextCreator::CCS_FORBIDDEN, QStringLiteral("The personal access token is not for the product of the server."));
				return nullptr;
			}

			if (productId.isEmpty()){
				productId = serverProductId;
			}
		}
		else{
			// Without a token of its server only a superuser session works for another product; decided once the user is known
			const QByteArray requestProductId = userTokenProductId.isEmpty() ? headerProductId : userTokenProductId;
			if (!requestProductId.isEmpty() && requestProductId != ownProductId){
				foreignProductId = requestProductId;
			}
		}
	}

	// A session opened through the server of a product works only through a server, which bounds it by its token: sent directly
	// it would escape that token. A session opened here works only here. A forwarded session may pass several servers
	if (!ownProductId.isEmpty() && !token.isEmpty() && !isPat){
		// Sent directly, a session of this server or of this product (its server opened it at the authorization server)
		const QByteArray sessionProductId = imtauth::GetJwtProductId(token);
		const bool isOwnSession = serverProductId.isEmpty() ? (sessionProductId.isEmpty() || sessionProductId == ownProductId) : !sessionProductId.isEmpty();
		// A product that names itself without a token of its server is told that first: it is the cause
		if (!isOwnSession && !foreignProductId.isEmpty()){
			SetError(error, imtgql::IGqlContextCreator::CCS_FORBIDDEN, GetNotConnectedMessage(foreignProductId, ownProductId));
			return nullptr;
		}
		if (!isOwnSession){
			SetError(error, imtgql::IGqlContextCreator::CCS_FORBIDDEN, sessionProductId.isEmpty()
						? QStringLiteral("The session was opened at this server and works only here, not through the server of '%1'.").arg(QString::fromUtf8(serverProductId))
						: QStringLiteral("The session was opened through the server of '%1' and works only through it.").arg(QString::fromUtf8(sessionProductId)));
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
	gqlContextPtr->SetServerProductId(serverProductId);
	gqlContextPtr->SetProductInfo(m_productInfoCompPtr.IsValid() ? m_productInfoCompPtr.GetPtr() : m_permissionTreeCompPtr.GetPtr());
	if (isPat){
		gqlContextPtr->SetScopes(scopes);
	}

	// Set before the tenant and the user are read: remote collections authenticate their requests with this context.
	imtgql::CGqlRequestContextManager::SetContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr.GetPtr()));

	// Resolve the scope, the ownership and the organization access of the token, cached for TenantDataCacheLifetime alongside the rest of this token's
	// resolved claims (see TokenCacheEntry) instead of being looked up live on
	// every call - CreateGqlContext() runs on every single incoming request AND
	// every WebSocket subscription (re-)registration, and m_tenantManagerCompPtr
	// is a remote proxy (CRemoteTenantControllerComp) that hits the tenant
	// service over the network. Without this cache, a burst of requests (e.g.
	// the dozens fired right after a tenant switch: subscriptions re-registering,
	// per-open-document queries, page reload) each re-triggered their own
	// uncached GetTenant() round-trip - the exact flood seen after SelectTenant.
	// A session of a user whose source computes the user: the user and its organization come in one complete answer, for a request
	// and a subscription alike; no cache, and no read with the token of the user, which may have expired
	imtbase::IObjectCollection::DataPtr sessionUserPtr;
	TokenTenantData tenantData;
	if (isSessionAuthorized){
		if (!tenantId.isEmpty()){
			tenantData.isTenantOwner = sessionAuthorization.tenantOwnerId == resolvedUserId;
			tenantData.hasOrganizationAccess = true;
			tenantData.tenantPermissions = sessionAuthorization.tenantPermissions;
			if (tenantData.isTenantOwner || sessionAuthorization.tenantCreatorId == resolvedUserId){
				tenantData.organizationPermissions = {imtauth::s_organizationManagementPermission};
			}
		}
		sessionUserPtr = sessionAuthorization.userPtr;
		if (sessionExpiresAtPtr != nullptr){
			*sessionExpiresAtPtr = sessionAuthorization.sessionExpiresAt;
		}
	}
	else if (sessionExpiresAtPtr != nullptr && isRenewal && !isPat && !sessionId.isEmpty() && m_slaveJwtSessionControllerCompPtr.IsValid()){
		// A session that could not be read has no known end: the control check comes all the same
		const imtauth::ISessionSharedPtr sessionPtr = m_slaveJwtSessionControllerCompPtr->GetSession(sessionId);
		if (sessionPtr.IsValid()){
			*sessionExpiresAtPtr = sessionPtr->GetExpirationDate();
		}
	}

	if (!resolvedUserId.isEmpty()){
		if (isRenewal && !isSessionAuthorized){
			// Organization data that could not be read says nothing about the access of a subscription
			tenantData = ReadTenantData(resolvedUserId, tenantId);
			if (!tenantData.isRead){
				imtgql::CGqlRequestContextManager::Clear();
				SetError(error, imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR, QStringLiteral("The organization data could not be read."));
				return nullptr;
			}
		}
		else if (!isSessionAuthorized && !TryGetCachedTenantData(token, tenantData)){
			// A request goes on with what was read; only a complete read is kept
			const qint64 resolutionGeneration = GetAuthorizationGeneration();
			tenantData = ReadTenantData(resolvedUserId, tenantId);
			if (tenantData.isRead){
				StoreCachedTenantData(token, tenantData, resolutionGeneration);
			}
		}

		if (tenantData.isTenantOwner){
			gqlContextPtr->SetIsTenantOwner(true);
		}

		gqlContextPtr->SetTenantPermissions(tenantData.tenantPermissions);
		gqlContextPtr->SetOrganizationPermissions(tenantData.organizationPermissions);
	}

	if ((isSessionAuthorized || m_userCollectionCompPtr.IsValid()) && !resolvedUserId.isEmpty()){
		imtbase::IObjectCollection::DataPtr userDataPtr;
		const imtauth::IUserInfo* userInfoPtr = nullptr;
		imtgql::IGqlContextCreator::ContextCreationError readError;
		if (isSessionAuthorized){
			userDataPtr = sessionUserPtr;
		}
		else if (isRenewal || IsComputedBySource()){
			// A subscription holds what was read now, not the caches; a failed read says nothing about it.
			// A user computed by the source holds permissions of one product and one organization: no shared cache may serve it.
			if (!ReadCurrentUser(resolvedUserId, productId, userDataPtr, readError) && isRenewal){
				imtgql::CGqlRequestContextManager::Clear();
				SetError(error, readError.status, readError.message);
				return nullptr;
			}
		}
		else{
			m_userCollectionCompPtr->GetObjectData(resolvedUserId, userDataPtr);
		}
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());

		// Deny access for disabled accounts (IEC 62443-3-3 SR 1.3): tokens and sessions
		// of a disabled user become unusable immediately, without removing the account.
		// A cached collection that fails is denied too: a remote user service refuses a disabled user's requests.
		// A user read at its source tells a failed read, which is not a refusal.
		if (userInfoPtr == nullptr || !userInfoPtr->IsEnabled()){
			imtgql::CGqlRequestContextManager::Clear();

			// A token resolved from the cache may belong to a session closed meanwhile (a tenant switch, a logout):
			// the remote read is refused for that reason, and the client has to refresh rather than sign out.
			if (userInfoPtr == nullptr && !isPat && !isRenewal && m_slaveJwtSessionControllerCompPtr.IsValid()){
				bool isSessionChecked = false;
				if (IsSessionClosed(token, isSessionChecked)){
					InvalidateTokenCache(token);
					SetError(error, imtgql::IGqlContextCreator::CCS_UNAUTHORIZED, QStringLiteral("The session of the token is closed."));
					return nullptr;
				}

				// Neither the user nor the session could be read
				if (!isSessionChecked){
					SetError(error, imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR, QStringLiteral("The session could not be checked."));
					return nullptr;
				}
			}

			// The access could not be checked: that is not a refusal
			if (readError.status != imtgql::IGqlContextCreator::CCS_OK){
				SetError(error, readError.status, readError.message);
				return nullptr;
			}

			SetError(
						error,
						imtgql::IGqlContextCreator::CCS_FORBIDDEN,
						userInfoPtr == nullptr ? QStringLiteral("User account is not available.") : QStringLiteral("User account is disabled."));
			return nullptr;
		}

		if (isPat){
			gqlContextPtr->SetUserInfo(userInfoPtr);
			if (!imtgql::RestrictUserPermissions(*gqlContextPtr, scopes)){
				imtgql::CGqlRequestContextManager::Clear();
				SetError(error, imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR, QStringLiteral("Unable to restrict personal token permissions."));
				return nullptr;
			}
		}
		else{
			// The owner of the organization holds its whole permission scope, so the pages, commands and headers built from the permissions show all of it
			imtauth::IUserInfo::FeatureIds grantedPermissions = imtgql::CollectTenantOwnerPermissions(*gqlContextPtr);
			// Outside of any organization a user allowed to create one sees them too
			if (tenantData.hasOrganizationAccess || (tenantId.isEmpty() && imtauth::HasPermission(userInfoPtr->GetPermissions(productId), imtauth::s_addOrganizationPermission, false))){
				grantedPermissions += imtauth::GetOrganizationAccessPermissions();
			}

			istd::IChangeableUniquePtr grantedUserPtr = grantedPermissions.isEmpty() ? istd::IChangeableUniquePtr() : userInfoPtr->CloneMe();
			imtauth::IUserInfo* grantedUserInfoPtr = dynamic_cast<imtauth::IUserInfo*>(grantedUserPtr.GetPtr());
			if (grantedUserInfoPtr != nullptr){
				imtauth::IUserInfo::FeatureIds localPermissions = grantedUserInfoPtr->GetLocalPermissions(productId);
				for (const QByteArray& permission : grantedPermissions){
					if (!localPermissions.contains(permission)){
						localPermissions.append(permission);
					}
				}
				grantedUserInfoPtr->SetLocalPermissions(productId, localPermissions);

				gqlContextPtr->SetUserInfo(grantedUserInfoPtr);
			}
			else{
				gqlContextPtr->SetUserInfo(userInfoPtr);
			}
		}
	}

	// A request through the server of a product holds no more than the token of that server: the handlers check it as usual
	if (!serverProductId.isEmpty()){
		if (!BindToServerToken(*gqlContextPtr, productId, clientProductId, serverScopes)){
			imtgql::CGqlRequestContextManager::Clear();
			SetError(error, imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR, QStringLiteral("Unable to restrict server token permissions."));
			return nullptr;
		}
	}

	if (!foreignProductId.isEmpty()){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr == nullptr || !userInfoPtr->IsAdmin() || isPat){
			imtgql::CGqlRequestContextManager::Clear();
			SetError(error, imtgql::IGqlContextCreator::CCS_FORBIDDEN, GetNotConnectedMessage(foreignProductId, ownProductId));
			return nullptr;
		}

		gqlContextPtr->SetProductId(foreignProductId);
	}

	const QByteArray forwardedLanguageId = headers.value(imtbase::s_languageIdHeaderId);
	if (!forwardedLanguageId.isEmpty()){
		gqlContextPtr->SetLanguageId(forwardedLanguageId);
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
							if (index >= 0 && forwardedLanguageId.isEmpty()){
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

bool CAuthenticationManagerComp::ValidateSession(const QByteArray& sessionId, bool* isCheckedPtr) const
{
	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		if (isCheckedPtr != nullptr){
			*isCheckedPtr = false;
		}

		return false;
	}

	return m_slaveJwtSessionControllerCompPtr->ValidateSession(sessionId, isCheckedPtr);
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

	if (!m_slaveJwtSessionControllerCompPtr->RemoveSession(sessionId)){
		return false;
	}

	InvalidateSessionTokenCache(sessionId);

	return true;
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
	{
		// A check of the token that is running now must not confirm it
		QMutexLocker cacheLocker(&m_tokenCacheMutex);
		++m_tokenChangeGeneration;
	}

	if (m_slaveJwtSessionControllerCompPtr.IsValid()){
		m_slaveJwtSessionControllerCompPtr->InvalidateToken(token);
	}
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CAuthenticationManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// A revoked or changed token is validated again at its next request
	imtauth::IPersonalAccessTokenManager* serverTokenManagerPtr = m_serverTokenReceiverCompPtr.IsValid() ? m_serverTokenReceiverCompPtr->GetTokenManager() : nullptr;
	for (imtauth::IPersonalAccessTokenManager* managerPtr : {m_patManagerCompPtr.GetPtr(), serverTokenManagerPtr != m_patManagerCompPtr.GetPtr() ? serverTokenManagerPtr : nullptr}){
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(managerPtr);
		if (modelPtr != nullptr){
			modelPtr->AttachObserver(this);
		}
	}

	// A change of the users, roles or groups the authorization server computes with: the kept answers are not used any more
	if (m_sessionAuthorizationProviderCompPtr.IsValid()){
		for (imtbase::IObjectCollection* collectionPtr : {m_userCollectionCompPtr.GetPtr(), m_roleCollectionCompPtr.GetPtr(), m_userGroupCollectionCompPtr.GetPtr()}){
			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(collectionPtr);
			if (modelPtr != nullptr && !IsModelAttached(modelPtr)){
				modelPtr->AttachObserver(this);
			}
		}
	}
}


void CAuthenticationManagerComp::OnComponentDestroyed()
{
	EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelObserverBase)

void CAuthenticationManagerComp::OnUpdate(imod::IModel* /*modelPtr*/, const istd::IChangeable::ChangeSet& changeSet)
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	const QByteArray tokenId = changeSet.Contains(imtauth::IPersonalAccessTokenManager::CF_TOKEN_CHANGED)
				? changeSet.GetChangeInfo(imtauth::IPersonalAccessTokenManager::CN_TOKEN_ID).toByteArray() : QByteArray();
	if (tokenId.isEmpty()){
		InvalidateAuthorizationLocked();
	}
	else{
		++m_tokenChangeGeneration;
		if (!m_tokensBeingResolved.isEmpty()){
			m_tokenChangeGenerations.insert(tokenId, m_tokenChangeGeneration);
		}
	}

	for (auto iter = m_tokenCache.begin(); iter != m_tokenCache.end(); ){
		iter = iter->isPat && (tokenId.isEmpty() || iter->tokenId.isEmpty() || iter->tokenId == tokenId)
					? m_tokenCache.erase(iter) : std::next(iter);
	}
}


// private methods

bool CAuthenticationManagerComp::HasOrganizationAccess(const QByteArray& userId, bool& isRead) const
{
	// A listed entry that cannot be read is a failed read too
	if (m_membershipManagerCompPtr.IsValid()){
		bool isListRead = false;
		for (const QByteArray& membershipId : m_membershipManagerCompPtr->GetMembershipsByUser(userId, &isListRead)){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (!membershipPtr.IsValid()){
				isListRead = false;
			}
			else if (membershipPtr->IsActive()){
				return true;
			}
		}
		isRead = isRead && isListRead;
	}

	if (m_invitationManagerCompPtr.IsValid()){
		const imtauth::ITenantInvitationManager::Statuses statuses{imtauth::ITenantInvitation::TIS_PENDING};
		bool isListRead = false;
		for (const QByteArray& invitationId : m_invitationManagerCompPtr->GetInvitationsByUser(userId, statuses, &isListRead)){
			imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
			if (!invitationPtr.IsValid()){
				isListRead = false;
			}
			else if (m_invitationManagerCompPtr->GetEffectiveStatus(*invitationPtr) == imtauth::ITenantInvitation::TIS_PENDING){
				return true;
			}
		}
		isRead = isRead && isListRead;
	}

	return false;
}


bool CAuthenticationManagerComp::IsSessionClosed(const QByteArray& token, bool& isChecked) const
{
	isChecked = false;

	const JwtState state = m_slaveJwtSessionControllerCompPtr->ValidateJwt(token);
	if (state == JwtState::JS_NONE){
		return false;
	}

	isChecked = true;
	if (state != JwtState::JS_EXPIRED){
		return false;
	}

	// The controller tells an expired token and a session it could not find alike; a session it could not read is not closed
	QByteArray userId;
	QByteArray tenantId;
	QByteArray sessionId;
	qint64 expSecs = 0;
	if (!ParseJwtClaims(token, userId, tenantId, sessionId, expSecs)){
		return false;
	}
	if (expSecs < QDateTime::currentSecsSinceEpoch()){
		return true;
	}

	const bool isSessionValid = m_slaveJwtSessionControllerCompPtr->ValidateSession(sessionId, &isChecked);

	return isChecked && !isSessionValid;
}


bool CAuthenticationManagerComp::ReadCurrentUser(
			const QByteArray& userId,
			const QByteArray& productId,
			imtbase::IObjectCollection::DataPtr& userDataPtr,
			imtgql::IGqlContextCreator::ContextCreationError& error) const
{
	const bool isComputedBySource = IsComputedBySource();
	CAuthorizationSnapshot snapshot(
				m_userCollectionCompPtr.GetPtr(),
				(!isComputedBySource && m_roleCollectionCompPtr.IsValid()) ? m_roleCollectionCompPtr.GetPtr() : nullptr,
				(!isComputedBySource && m_userGroupCollectionCompPtr.IsValid()) ? m_userGroupCollectionCompPtr.GetPtr() : nullptr);

	QString errorMessage;
	istd::IChangeableUniquePtr userPtr;
	const CAuthorizationSnapshot::ReadStatus status = snapshot.ReadUser(userId, userPtr, errorMessage);
	if (status == CAuthorizationSnapshot::RS_ABSENT){
		return true;
	}
	if (status == CAuthorizationSnapshot::RS_DENIED){
		SetError(error, imtgql::IGqlContextCreator::CCS_FORBIDDEN, errorMessage);

		return false;
	}

	bool isRead = (status == CAuthorizationSnapshot::RS_READ);
	if (isRead){
		imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(userPtr.GetPtr());
		if (isComputedBySource){
			// The source resolved the permissions: its roles and groups are not expanded again here
			CAuthorizationSnapshot::DetachUser(*userPtr);
		}
		else{
			isRead = snapshot.KeepPermissions(*userInfoPtr, productId, errorMessage);
		}
	}

	if (!isRead){
		SetError(error, imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR, errorMessage);

		return false;
	}

	userDataPtr.FromUnique(std::move(userPtr));

	return true;
}


CAuthenticationManagerComp::TokenTenantData CAuthenticationManagerComp::ReadTenantData(const QByteArray& userId, const QByteArray& tenantId) const
{
	TokenTenantData tenantData;

	if (tenantId.isEmpty()){
		tenantData.hasOrganizationAccess = HasOrganizationAccess(userId, tenantData.isRead);

		return tenantData;
	}

	QByteArray ownerId;
	QByteArray creatorId;
	QByteArrayList tenantPermissions;
	if (m_tenantManagerCompPtr.IsValid()){
		imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId, &tenantData.isRead);
		if (tenantPtr.IsValid()){
			ownerId = tenantPtr->GetOwnerId();
			creatorId = tenantPtr->GetCreatorId();
			tenantPermissions = tenantPtr->GetTenantPermissions();
		}
	}

	CompleteTenantData(userId, tenantId, ownerId, creatorId, tenantPermissions, tenantData);

	return tenantData;
}


void CAuthenticationManagerComp::CompleteTenantData(
			const QByteArray& userId,
			const QByteArray& tenantId,
			const QByteArray& ownerId,
			const QByteArray& creatorId,
			const QByteArrayList& tenantPermissions,
			TokenTenantData& tenantData) const
{
	tenantData.isTenantOwner = !ownerId.isEmpty() && ownerId == userId;
	const bool isCreator = !creatorId.isEmpty() && creatorId == userId;
	tenantData.tenantPermissions = tenantPermissions;

	// The owner and the creator manage all of the organization, a member what the membership gives
	if (tenantData.isTenantOwner || isCreator){
		tenantData.hasOrganizationAccess = true;
		tenantData.organizationPermissions = {imtauth::s_organizationManagementPermission};
	}
	else if (!m_membershipManagerCompPtr.IsValid()){
		// Without the memberships (a product server) the organization of the token is trusted, and a member holds no organization permissions
		tenantData.hasOrganizationAccess = true;
	}
	else{
		bool isMembershipRead = false;
		imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->FindMembership(userId, tenantId, &isMembershipRead);
		tenantData.isRead = tenantData.isRead && isMembershipRead;
		if (membershipPtr.IsValid() && membershipPtr->IsActive()){
			tenantData.hasOrganizationAccess = true;
			tenantData.organizationPermissions = membershipPtr->GetOrganizationPermissions();
		}
	}
}


void CAuthenticationManagerComp::InvalidateAuthorizationLocked() const
{
	m_lastGlobalTokenChangeGeneration = ++m_tokenChangeGeneration;

	for (TokenCacheEntry& entry : m_tokenCache){
		entry.tenantDataResolvedAt = 0;
	}
	m_sessionAuthorizationCache.clear();
}


bool CAuthenticationManagerComp::TryAwaitOrClaimSessionAuthorization(
			const QByteArray& token,
			imtauth::ISessionAuthorizationProvider::SessionAuthorization& authorization,
			bool& isClaimed) const
{
	isClaimed = false;
	const qint64 ttlMs = m_tenantDataCacheTtlAttrPtr.IsValid() ? (static_cast<qint64>(*m_tenantDataCacheTtlAttrPtr) * 1000) : (10 * 1000);
	if (ttlMs <= 0){
		return false;
	}

	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	bool hasWaited = false;
	while (true){
		const auto iter = m_sessionAuthorizationCache.constFind(token);
		if (iter != m_sessionAuthorizationCache.cend()){
			// An answer never outlives its access token
			const qint64 now = QDateTime::currentMSecsSinceEpoch();
			if (now - iter->resolvedAt <= ttlMs && iter->jwtExpSecs * 1000 > now){
				authorization = iter->authorization;
				return true;
			}
			m_sessionAuthorizationCache.erase(iter);
		}

		if (!m_tokensBeingResolved.contains(token)){
			m_tokensBeingResolved.insert(token);
			isClaimed = true;
			return false;
		}

		// The answer waited for was not kept: a failing source is asked by everybody at once, not one after another
		if (hasWaited){
			return false;
		}
		hasWaited = true;
		m_tokenResolutionWaitCondition.wait(&m_tokenCacheMutex);
	}
}


void CAuthenticationManagerComp::StoreCachedSessionAuthorization(
			const QByteArray& token,
			const imtauth::ISessionAuthorizationProvider::SessionAuthorization& authorization,
			qint64 jwtExpSecs,
			qint64 resolutionGeneration) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);

	// Asked before the access or a token changed
	if (resolutionGeneration != m_tokenChangeGeneration){
		return;
	}

	const int maxTokenCacheSize = m_maxTokenCacheSizeAttrPtr.IsValid() ? *m_maxTokenCacheSizeAttrPtr : 10000;
	if (m_sessionAuthorizationCache.size() >= maxTokenCacheSize){
		m_sessionAuthorizationCache.clear();
	}

	SessionAuthorizationCacheEntry entry;
	entry.authorization = authorization;
	entry.resolvedAt = QDateTime::currentMSecsSinceEpoch();
	entry.jwtExpSecs = jwtExpSecs;
	m_sessionAuthorizationCache.insert(token, entry);
}


bool CAuthenticationManagerComp::ResolveUserId(
			const QByteArray& token,
			QByteArray& userId,
			QByteArray& tenantId,
			QByteArrayList& scopes,
			bool& isPat,
			QString& errorMessage,
			imtgql::IGqlContextCreator::ContextCreationStatus& status) const
{
	if (!imtauth::IsServerToken(token)){
		return ResolveToken(token, m_patManagerCompPtr.GetPtr(), userId, tenantId, scopes, isPat, errorMessage, status);
	}

	// The token of a server is known only where it was issued
	const IServerTokenReceiver* receiverPtr = GetServerTokenReceiver();
	if (receiverPtr == nullptr){
		errorMessage = QStringLiteral("This server accepts no tokens of servers.");
		status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;

		return false;
	}

	return ResolveToken(token, receiverPtr->GetTokenManager(), userId, tenantId, scopes, isPat, errorMessage, status);
}


bool CAuthenticationManagerComp::ResolveToken(
			const QByteArray& token,
			imtauth::IPersonalAccessTokenManager* patManagerPtr,
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
		if (patManagerPtr == nullptr){
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

		QByteArray tokenProductId;
		QByteArray tokenClientProductId;
		bool isCheckFailed = false;
		qint64 resolutionGeneration;
		{
			QMutexLocker locker(&m_tokenCacheMutex);
			resolutionGeneration = m_tokenChangeGeneration;
		}
		if (!patManagerPtr->ValidateToken(token, userId, tokenId, scopes, &tokenProductId, &tokenClientProductId, &isCheckFailed)){
			errorMessage = isCheckFailed ? QStringLiteral("The personal access token could not be checked.") : QStringLiteral("Invalid personal access token.");
			status = isCheckFailed ? imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR : imtgql::IGqlContextCreator::CCS_FORBIDDEN;
			SendWarningMessage(
						0,
						QStringLiteral("ResolveUserId: invalid PAT %1")
								.arg(MaskToken(token)),
						QStringLiteral("CAuthenticationManagerComp"));
		}
		else{
			isTokenValid = true;
			patManagerPtr->UpdateLastUsedAt(tokenId);
		}

		if (!isTokenValid){
			return false;
		}

		StoreCachedToken(token, userId, tenantId, tokenId, scopes, true, 0, tokenProductId, tokenClientProductId, resolutionGeneration);
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


bool CAuthenticationManagerComp::GetCachedPatProducts(const QByteArray& token, QByteArray& productId, QByteArray& clientProductId) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	auto iter = m_tokenCache.constFind(token);
	if (iter == m_tokenCache.constEnd()){
		return false;
	}

	productId = iter->patProductId;
	clientProductId = iter->patClientProductId;
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
	if (m_tokensBeingResolved.isEmpty()){
		m_tokenChangeGenerations.clear();
	}
	m_tokenResolutionWaitCondition.wakeAll();
}


void CAuthenticationManagerComp::StoreCachedToken(
			const QByteArray& token,
			const QByteArray& userId,
			const QByteArray& tenantId,
			const QByteArray& tokenId,
			const QByteArrayList& scopes,
			bool isPat,
			qint64 jwtExpSecs,
			const QByteArray& patProductId,
			const QByteArray& patClientProductId,
			qint64 resolutionGeneration) const
{
	TokenCacheEntry entry;
	entry.patProductId = patProductId;
	entry.patClientProductId = patClientProductId;
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

	QByteArray sessionId;
	if (!isPat){
		QByteArray claimUserId;
		QByteArray claimTenantId;
		qint64 claimExpSecs = 0;
		ParseJwtClaims(token, claimUserId, claimTenantId, sessionId, claimExpSecs);
	}

	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	if (!sessionId.isEmpty()){
		// A resolution that started before RemoveSession() must not re-insert
		// a token of the removed session after its cache entries were swept.
		auto revokedIter = m_revokedSessionIds.constFind(sessionId);
		if (revokedIter != m_revokedSessionIds.constEnd() && *revokedIter > now){
			return;
		}
	}

	// The same for a validation that started before a token was revoked or changed
	if (isPat && (resolutionGeneration < m_lastGlobalTokenChangeGeneration
				|| resolutionGeneration < m_tokenChangeGenerations.value(tokenId))){
		return;
	}

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


bool CAuthenticationManagerComp::ResolveFreshToken(
			const QByteArray& token,
			const imtauth::IPersonalAccessTokenManager* managerPtr,
			bool isExpiryAccepted,
			FreshToken& freshToken,
			imtgql::IGqlContextCreator::ContextCreationError& error) const
{
	freshToken = FreshToken();

	if (IsPatToken(token)){
		if (managerPtr == nullptr){
			SetError(error, CCS_INTERNAL_ERROR, QStringLiteral("The token store is not configured."));
			return false;
		}

		QByteArray tokenId;
		bool isCheckFailed = false;
		if (!managerPtr->ValidateToken(token, freshToken.userId, tokenId, freshToken.scopes, &freshToken.productId, &freshToken.clientProductId, &isCheckFailed)){
			if (!isCheckFailed){
				InvalidateTokenCache(token);
			}
			SetError(error, isCheckFailed ? CCS_INTERNAL_ERROR : CCS_FORBIDDEN, isCheckFailed
						? QStringLiteral("The token could not be checked.") : QStringLiteral("The token is revoked, expired or unknown."));
			return false;
		}

		freshToken.isPat = true;

		// A remote store has no change notification here: the requests do not go on with an older, broader token
		QMutexLocker cacheLocker(&m_tokenCacheMutex);
		if (m_tokensBeingResolved.contains(token)){
			// The resolution running now may have read it before: it does not fill the cache. Not a change of the access
			m_tokenChangeGenerations.insert(tokenId, m_tokenChangeGeneration + 1);
		}
		auto cached = m_tokenCache.find(token);
		if (cached != m_tokenCache.end() && (cached->userId != freshToken.userId || cached->tokenId != tokenId || cached->scopes != freshToken.scopes
					|| cached->patProductId != freshToken.productId || cached->patClientProductId != freshToken.clientProductId)){
			m_tokenCache.erase(cached);
		}

		return true;
	}

	if (!m_slaveJwtSessionControllerCompPtr.IsValid()){
		SetError(error, CCS_INTERNAL_ERROR, QStringLiteral("JWT session controller is not configured."));
		return false;
	}

	// An expired access token holds while its session lives, for an established subscription only: a new one refreshes it first
	imtauth::SessionTokenClaims claims;
	QString message;
	const imtauth::SessionTokenCheck check = imtauth::CheckSessionToken(*m_slaveJwtSessionControllerCompPtr, token, isExpiryAccepted, claims, message);
	if (check != imtauth::STC_VALID){
		SetError(error, ToContextCreationStatus(check), message);
		return false;
	}

	freshToken.userId = claims.userId;
	freshToken.tenantId = claims.tenantId;
	freshToken.sessionId = claims.sessionId;

	return true;
}


bool CAuthenticationManagerComp::ResolveServerToken(
			const QByteArray& serverToken,
			bool isRenewal,
			QByteArray& clientProductId,
			QByteArrayList& scopes,
			imtgql::IGqlContextCreator::ContextCreationError& error) const
{
	if (!imtauth::IsServerToken(serverToken)){
		SetError(error, CCS_FORBIDDEN, QStringLiteral("The token of the server is not valid."));

		return false;
	}

	QByteArray tokenProductId;
	if (isRenewal){
		FreshToken freshToken;
		if (!ResolveFreshToken(serverToken, m_serverTokenReceiverCompPtr->GetTokenManager(), false, freshToken, error)){
			return false;
		}

		scopes = freshToken.scopes;
		tokenProductId = freshToken.productId;
		clientProductId = freshToken.clientProductId;
	}
	else{
		QByteArray userId;
		QByteArray tenantId;
		bool isPat = false;
		QString errorMessage;
		ContextCreationStatus status = CCS_OK;
		if (!ResolveToken(serverToken, m_serverTokenReceiverCompPtr->GetTokenManager(), userId, tenantId, scopes, isPat, errorMessage, status)){
			SetError(error, status == CCS_OK ? CCS_FORBIDDEN : status, errorMessage.isEmpty() ? QStringLiteral("The token of the server is not valid.") : errorMessage);

			return false;
		}

		if (!GetCachedPatProducts(serverToken, tokenProductId, clientProductId)){
			clientProductId.clear();
		}
	}

	if (clientProductId.isEmpty() || tokenProductId != m_serverTokenReceiverCompPtr->GetServerProductId()){
		SetError(error, CCS_FORBIDDEN, QStringLiteral("The token of the server is not issued for this server."));

		return false;
	}

	return true;
}


void CAuthenticationManagerComp::InvalidateTokenCache(const QByteArray& token) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	if (IsPatToken(token) && m_tokensBeingResolved.contains(token)){
		// An invalid remote token may not reveal its ID; reject outstanding PAT cache fills conservatively.
		m_lastGlobalTokenChangeGeneration = ++m_tokenChangeGeneration;
	}
	m_tokenCache.remove(token);
	m_expiredTokenCache.remove(token);
	m_sessionAuthorizationCache.remove(token);
}


void CAuthenticationManagerComp::InvalidateSessionTokenCache(const QByteArray& sessionId) const
{
	if (sessionId.isEmpty()){
		return;
	}

	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	const qint64 tokenCacheTtlMs = m_tokenCacheTtlAttrPtr.IsValid() ? (static_cast<qint64>(*m_tokenCacheTtlAttrPtr) * 1000) : (30 * 1000);

	QMutexLocker cacheLocker(&m_tokenCacheMutex);

	// A check of the session that is running now must not confirm it
	++m_tokenChangeGeneration;

	for (auto iter = m_revokedSessionIds.begin(); iter != m_revokedSessionIds.end(); ){
		if (*iter <= now){
			iter = m_revokedSessionIds.erase(iter);
		}
		else{
			++iter;
		}
	}

	const int maxTokenCacheSize = m_maxTokenCacheSizeAttrPtr.IsValid() ? *m_maxTokenCacheSizeAttrPtr : 10000;
	while (!m_revokedSessionIds.isEmpty() && m_revokedSessionIds.size() >= maxTokenCacheSize){
		m_revokedSessionIds.erase(m_revokedSessionIds.begin());
	}
	m_revokedSessionIds.insert(sessionId, now + tokenCacheTtlMs);

	for (auto iter = m_tokenCache.begin(); iter != m_tokenCache.end();){
		QByteArray userId;
		QByteArray tenantId;
		QByteArray tokenSessionId;
		qint64 expSecs = 0;
		if (ParseJwtClaims(iter.key(), userId, tenantId, tokenSessionId, expSecs) && tokenSessionId == sessionId){
			iter = m_tokenCache.erase(iter);
		}
		else{
			++iter;
		}
	}

	for (auto iter = m_sessionAuthorizationCache.begin(); iter != m_sessionAuthorizationCache.end();){
		imtauth::SessionTokenClaims claims;
		iter = (imtauth::ParseSessionTokenClaims(iter.key(), claims) && claims.sessionId == sessionId)
					? m_sessionAuthorizationCache.erase(iter) : std::next(iter);
	}
}


bool CAuthenticationManagerComp::TryGetCachedTenantData(const QByteArray& token, TokenTenantData& tenantData) const
{
	const qint64 ttlMs = m_tenantDataCacheTtlAttrPtr.IsValid() ? (static_cast<qint64>(*m_tenantDataCacheTtlAttrPtr) * 1000) : (10 * 1000);

	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	auto iter = m_tokenCache.find(token);
	if (iter == m_tokenCache.end() || iter->tenantDataResolvedAt == 0 || QDateTime::currentMSecsSinceEpoch() - iter->tenantDataResolvedAt > ttlMs){
		return false;
	}

	tenantData = iter->tenantData;
	return true;
}


void CAuthenticationManagerComp::StoreCachedTenantData(const QByteArray& token, const TokenTenantData& tenantData, qint64 resolutionGeneration) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);

	// Read before the access changed
	if (resolutionGeneration < m_lastGlobalTokenChangeGeneration){
		return;
	}

	auto iter = m_tokenCache.find(token);
	if (iter == m_tokenCache.end()){
		// ResolveUserId() always caches the token before this is reached; a
		// missing entry here means it expired/was invalidated in the tiny
		// window since then. Nothing to attach the flag to - next call will
		// simply resolve (and cache) it again from scratch.
		return;
	}

	iter->tenantData = tenantData;
	iter->tenantDataResolvedAt = QDateTime::currentMSecsSinceEpoch();
}


imtgql::IGqlContextUniquePtr CAuthenticationManagerComp::CreateContextInstance() const
{
	return m_gqlContextFactCompPtr.CreateInstance();
}


bool CAuthenticationManagerComp::IsPatToken(const QByteArray& token) const
{
	if (imtauth::IsServerToken(token)){
		return true;
	}

	const QByteArray& patPrefix = m_patPrefixAttrPtr.IsValid() ? *m_patPrefixAttrPtr : QByteArrayLiteral("imt_pat_");
	return !patPrefix.isEmpty() && token.size() > patPrefix.size() && token.startsWith(patPrefix);
}


const IServerTokenReceiver* CAuthenticationManagerComp::GetServerTokenReceiver() const
{
	return (m_serverTokenReceiverCompPtr.IsValid() && m_serverTokenReceiverCompPtr->IsEnabled()) ? m_serverTokenReceiverCompPtr.GetPtr() : nullptr;
}


bool CAuthenticationManagerComp::IsComputedBySource() const
{
	return m_sessionAuthorizationProviderCompPtr.IsValid() && m_sessionAuthorizationProviderCompPtr->IsAvailable();
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


bool CAuthenticationManagerComp::BindToServerToken(
			imtgql::IGqlContext& gqlContext,
			const QByteArray& productId,
			const QByteArray& clientProductId,
			const QByteArrayList& serverScopes) const
{
	const imtauth::IUserInfo* userInfoPtr = gqlContext.GetUserInfo();
	if (userInfoPtr != nullptr){
		// What the user may give in its product is what this server holds for it, within its personal token; the token of the server bounds what it may do
		gqlContext.SetGrantingAuthority(userInfoPtr->IsAdmin(), imtservergql::GetUserPermissions(gqlContext));

		return imtgql::RestrictUserPermissions(gqlContext, serverScopes);
	}
	else if (!clientProductId.isEmpty()){
		imtauth::CIdentifiableUserInfo boundUserInfo;
		boundUserInfo.SetName(QString::fromUtf8(clientProductId));
		boundUserInfo.SetEnabled(true);
		boundUserInfo.SetLocalPermissions(productId, serverScopes);
		gqlContext.SetUserInfo(&boundUserInfo);
		gqlContext.SetPermissionBoundary(serverScopes);
		gqlContext.SetPrincipalKind(imtgql::IGqlContext::PK_SERVICE);
	}

	return true;
}


bool CAuthenticationManagerComp::ParseJwtClaims(
			const QByteArray& jwt,
			QByteArray& userId,
			QByteArray& tenantId,
			QByteArray& sessionId,
			qint64& expSecs) const
{
	imtauth::SessionTokenClaims claims;
	const bool retVal = imtauth::ParseSessionTokenClaims(jwt, claims);
	userId = claims.userId;
	tenantId = claims.tenantId;
	sessionId = claims.sessionId;
	expSecs = claims.expSecs;

	return retVal;
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
