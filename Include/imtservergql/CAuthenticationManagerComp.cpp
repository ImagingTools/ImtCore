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
#include <imtauth/imtauth.h>
#include <imtauth/IUserSettings.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtgql/imtgql.h>


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
	if (!token.isEmpty()){
		imtgql::IGqlContextCreator::ContextCreationStatus authStatus = imtgql::IGqlContextCreator::CCS_OK;
		QString authErrorMessage;
		if (!ResolveUserId(token, resolvedUserId, tenantId, scopes, isPat, authErrorMessage, authStatus)){
			SetError(error, authStatus, authErrorMessage);
			return nullptr;
		}

		if (isPat){
			QByteArray tokenProductId;
			bool isTokenAccepted = GetCachedPatProducts(token, tokenProductId, clientProductId);
			// A token is of the kind its prefix names
			isTokenAccepted = isTokenAccepted && imtauth::IsServerToken(token) == !clientProductId.isEmpty();
			if (isTokenAccepted && !clientProductId.isEmpty()){
				// The token of a server names the server accepting it and the product using it
				isTokenAccepted = tokenProductId == m_serverTokenReceiverCompPtr->GetServerProductId() && (headerProductId.isEmpty() || headerProductId == clientProductId);
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
	const QByteArray ownProductId = m_serverTokenReceiverCompPtr.IsValid() ? m_serverTokenReceiverCompPtr->GetServerProductId() : QByteArray();
	if (clientProductId.isEmpty() && !ownProductId.isEmpty()){
		const QByteArray serverToken = headers.value(imtbase::s_serverTokenHeaderId);
		if (!serverToken.isEmpty()){
			if (!ResolveServerToken(serverToken, serverProductId, serverScopes, error)){
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

	// A session works only through the server it was opened through: sent directly or through another server it would
	// escape the token of that server, and a session opened here would gain the rights of a server
	if (!ownProductId.isEmpty() && !token.isEmpty() && !isPat){
		// Sent directly, a session of this server or of this product (its server opened it at the authorization server)
		const QByteArray sessionProductId = imtauth::GetJwtProductId(token);
		const bool isOwnSession = serverProductId.isEmpty() ? (sessionProductId.isEmpty() || sessionProductId == ownProductId) : sessionProductId == serverProductId;
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
	gqlContextPtr->SetClientProductId(clientProductId);
	gqlContextPtr->SetServerProductId(serverProductId);
	gqlContextPtr->SetServerScopes(serverScopes);
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
	TokenTenantData tenantData;
	if (!resolvedUserId.isEmpty()){
		if (!TryGetCachedTenantData(token, tenantData)){
			bool isCreator = false;
			if (m_tenantManagerCompPtr.IsValid() && !tenantId.isEmpty()){
				imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
				if (tenantPtr.IsValid()){
					tenantData.isTenantOwner = tenantPtr->GetOwnerId() == resolvedUserId;
					isCreator = tenantPtr->GetCreatorId() == resolvedUserId;
					tenantData.tenantPermissions = tenantPtr->GetTenantPermissions();
				}
			}
			tenantData.hasOrganizationAccess = HasOrganizationAccess(resolvedUserId, tenantId, tenantData.isTenantOwner || isCreator);
			tenantData.organizationPermissions = GetOrganizationPermissions(resolvedUserId, tenantId, tenantData.isTenantOwner || isCreator);
			StoreCachedTenantData(token, tenantData);
		}

		if (tenantData.isTenantOwner){
			gqlContextPtr->SetIsTenantOwner(true);
		}

		gqlContextPtr->SetTenantPermissions(tenantData.tenantPermissions);
		gqlContextPtr->SetOrganizationPermissions(tenantData.organizationPermissions);
	}

	if (m_userCollectionCompPtr.IsValid() && !resolvedUserId.isEmpty()){
		imtbase::IObjectCollection::DataPtr userDataPtr;
		const imtauth::IUserInfo* userInfoPtr = nullptr;
		if (m_userCollectionCompPtr->GetObjectData(resolvedUserId, userDataPtr)){
			userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
		}

		// Deny access for disabled accounts (IEC 62443-3-3 SR 1.3): tokens and sessions
		// of a disabled user become unusable immediately, without removing the account.
		// A failed read is denied too: a remote user service refuses a disabled user's requests.
		if (userInfoPtr == nullptr || !userInfoPtr->IsEnabled()){
			imtgql::CGqlRequestContextManager::Clear();

			// A token resolved from the cache may belong to a session closed meanwhile (a tenant switch, a logout):
			// the remote read is refused for that reason, and the client has to refresh rather than sign out.
			if (userInfoPtr == nullptr && !isPat && m_slaveJwtSessionControllerCompPtr.IsValid()
						&& m_slaveJwtSessionControllerCompPtr->ValidateJwt(token) == imtauth::IJwtSessionController::JS_EXPIRED){
				InvalidateTokenCache(token);
				SetError(error, imtgql::IGqlContextCreator::CCS_UNAUTHORIZED, QStringLiteral("The session of the token is closed."));
				return nullptr;
			}

			SetError(
						error,
						imtgql::IGqlContextCreator::CCS_FORBIDDEN,
						userInfoPtr == nullptr ? QStringLiteral("User account is not available.") : QStringLiteral("User account is disabled."));
			return nullptr;
		}

		if (isPat){
			// For PAT: restrict the user's permissions to the intersection with the
			// token's scopes. GetPermissions() now returns role- and local-permission
			// derived permissions combined, so writing the intersection into local
			// permissions (after stripping roles/groups) is enough to make
			// GetPermissions() - the method every permission check in this codebase
			// actually reads - report exactly that intersection. An empty scope list
			// yields zero permissions (fail closed), not the full unrestricted user.
			imtauth::IUserInfo::FeatureIds fullPermissions = userInfoPtr->GetPermissions(productId);
			imtauth::IUserInfo::FeatureIds effectivePermissions;
			for (const QByteArray& perm : fullPermissions){
				if (imtauth::HasPermission(scopes, perm, false)){
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

	if (!foreignProductId.isEmpty()){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr == nullptr || !userInfoPtr->IsAdmin() || isPat){
			imtgql::CGqlRequestContextManager::Clear();
			SetError(error, imtgql::IGqlContextCreator::CCS_FORBIDDEN, QStringLiteral("The product '%1' is not connected: its server sends no valid token.").arg(QString::fromUtf8(foreignProductId)));
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
}


void CAuthenticationManagerComp::OnComponentDestroyed()
{
	EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelObserverBase)

void CAuthenticationManagerComp::OnUpdate(imod::IModel* /*modelPtr*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_lastTokenChangeAt = QDateTime::currentMSecsSinceEpoch();

	for (auto iter = m_tokenCache.begin(); iter != m_tokenCache.end(); ){
		iter = iter->isPat ? m_tokenCache.erase(iter) : std::next(iter);
	}
}


// private methods

bool CAuthenticationManagerComp::HasOrganizationAccess(const QByteArray& userId, const QByteArray& tenantId, bool isOwnerOrCreator) const
{
	if (!tenantId.isEmpty()){
		// Without the memberships (a product server) the organization of the token is trusted
		return isOwnerOrCreator || !m_membershipManagerCompPtr.IsValid() || m_membershipManagerCompPtr->IsMember(userId, tenantId);
	}

	if (m_membershipManagerCompPtr.IsValid()){
		for (const QByteArray& membershipId : m_membershipManagerCompPtr->GetMembershipsByUser(userId)){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr.IsValid() && membershipPtr->IsActive()){
				return true;
			}
		}
	}

	if (m_invitationManagerCompPtr.IsValid()){
		const imtauth::ITenantInvitationManager::Statuses statuses{imtauth::ITenantInvitation::TIS_PENDING};
		for (const QByteArray& invitationId : m_invitationManagerCompPtr->GetInvitationsByUser(userId, statuses)){
			imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
			if (invitationPtr.IsValid() && m_invitationManagerCompPtr->GetEffectiveStatus(*invitationPtr) == imtauth::ITenantInvitation::TIS_PENDING){
				return true;
			}
		}
	}

	return false;
}


QByteArrayList CAuthenticationManagerComp::GetOrganizationPermissions(const QByteArray& userId, const QByteArray& tenantId, bool isOwnerOrCreator) const
{
	if (tenantId.isEmpty()){
		return QByteArrayList();
	}

	if (isOwnerOrCreator){
		return {imtauth::s_organizationManagementPermission};
	}

	if (!m_membershipManagerCompPtr.IsValid()){
		return QByteArrayList();
	}

	imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->FindMembership(userId, tenantId);

	return membershipPtr.IsValid() && membershipPtr->IsActive() ? membershipPtr->GetOrganizationPermissions() : QByteArrayList();
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
	if (!m_serverTokenReceiverCompPtr.IsValid()){
		errorMessage = QStringLiteral("This server accepts no tokens of servers.");
		status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;

		return false;
	}

	return ResolveToken(token, m_serverTokenReceiverCompPtr->GetTokenManager(), userId, tenantId, scopes, isPat, errorMessage, status);
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
		const qint64 resolutionStartedAt = QDateTime::currentMSecsSinceEpoch();
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

		StoreCachedToken(token, userId, tenantId, tokenId, scopes, true, 0, tokenProductId, tokenClientProductId, resolutionStartedAt);
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
			qint64 resolutionStartedAt) const
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
	if (isPat && resolutionStartedAt <= m_lastTokenChangeAt){
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


imtgql::IGqlContextCreator::ContextCreationStatus CAuthenticationManagerComp::CheckToken(const QByteArray& token, QByteArrayList& scopes, QString& errorMessage) const
{
	QByteArray userId;
	QByteArray tenantId;
	bool isPat = false;
	ContextCreationStatus status = CCS_OK;

	return ResolveUserId(token, userId, tenantId, scopes, isPat, errorMessage, status) ? CCS_OK : status;
}


bool CAuthenticationManagerComp::CheckRequestAccess(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	const QByteArray serverProductId = contextPtr != nullptr ? contextPtr->GetServerProductId() : QByteArray();
	if (serverProductId.isEmpty()){
		return true;
	}

	QByteArrayList requiredPermissions;
	if (!m_serverTokenReceiverCompPtr.IsValid() || !m_serverTokenReceiverCompPtr->FindRequiredPermissions(gqlRequest, requiredPermissions)){
		errorMessage = QStringLiteral("The request '%1' is closed to the servers of products").arg(QString::fromUtf8(gqlRequest.GetCommandId()));

		return false;
	}

	for (const QByteArray& requiredPermission : requiredPermissions){
		if (!imtauth::HasPermission(contextPtr->GetServerScopes(), requiredPermission)){
			errorMessage = QStringLiteral("The server of '%1' may not send '%2': its token lacks '%3'")
						.arg(QString::fromUtf8(serverProductId), QString::fromUtf8(gqlRequest.GetCommandId()), QString::fromUtf8(requiredPermission));

			return false;
		}
	}

	return true;
}


bool CAuthenticationManagerComp::ResolveServerToken(
			const QByteArray& serverToken,
			QByteArray& clientProductId,
			QByteArrayList& scopes,
			imtgql::IGqlContextCreator::ContextCreationError& error) const
{
	QByteArray userId;
	QByteArray tenantId;
	bool isPat = false;
	QString errorMessage;
	ContextCreationStatus status = CCS_OK;
	if (!imtauth::IsServerToken(serverToken) || !ResolveToken(serverToken, m_serverTokenReceiverCompPtr->GetTokenManager(), userId, tenantId, scopes, isPat, errorMessage, status)){
		SetError(error, status == CCS_OK ? CCS_FORBIDDEN : status, errorMessage.isEmpty() ? QStringLiteral("The token of the server is not valid.") : errorMessage);

		return false;
	}

	QByteArray tokenProductId;
	if (!GetCachedPatProducts(serverToken, tokenProductId, clientProductId) || clientProductId.isEmpty() || tokenProductId != m_serverTokenReceiverCompPtr->GetServerProductId()){
		SetError(error, CCS_FORBIDDEN, QStringLiteral("The token of the server is not issued for this server."));

		return false;
	}

	return true;
}


bool CAuthenticationManagerComp::IsRevocableToken(const QByteArray& token) const
{
	return IsPatToken(token);
}


void CAuthenticationManagerComp::InvalidateTokenCache(const QByteArray& token) const
{
	QMutexLocker cacheLocker(&m_tokenCacheMutex);
	m_tokenCache.remove(token);
	m_expiredTokenCache.remove(token);
}


void CAuthenticationManagerComp::InvalidateSessionTokenCache(const QByteArray& sessionId) const
{
	if (sessionId.isEmpty()){
		return;
	}

	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	const qint64 tokenCacheTtlMs = m_tokenCacheTtlAttrPtr.IsValid() ? (static_cast<qint64>(*m_tokenCacheTtlAttrPtr) * 1000) : (30 * 1000);

	QMutexLocker cacheLocker(&m_tokenCacheMutex);
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


void CAuthenticationManagerComp::StoreCachedTenantData(const QByteArray& token, const TokenTenantData& tenantData) const
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
