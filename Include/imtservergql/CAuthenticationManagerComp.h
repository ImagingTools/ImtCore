// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/CMultiModelObserverBase.h>

// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QSet>
#include <QtCore/QWaitCondition>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/IGqlContextCreator.h>
#include <imtauth/IJwtSessionController.h>
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtauth/ITenantInvitationManager.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtlic/IProductInfo.h>
#include <imtservergql/CAuthenticationManagerCompBase.h>


namespace imtservergql
{


class CAuthenticationManagerComp:
			public CAuthenticationManagerCompBase,
			virtual public imtgql::IGqlContextCreator,
			virtual public imtauth::IJwtSessionController,
			protected imod::CMultiModelObserverBase
{
public:
	typedef CAuthenticationManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAuthenticationManagerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextCreator);
		I_REGISTER_INTERFACE(imtauth::IJwtSessionController);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", false, "UserSettingsCollection");
		I_ASSIGN(m_gqlContextFactCompPtr, "GqlContextFactory", "GraphQL context factory", false, "GqlContextFactory");
		I_ASSIGN(m_slaveJwtSessionControllerCompPtr, "SlaveJwtSessionController", "Slave JWT session controller for delegation", false, "JwtSessionController");
		I_ASSIGN(m_patManagerCompPtr, "PersonalAccessTokenManager", "Personal Access Token manager", false, "PersonalAccessTokenManager");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for owner resolution", false, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Memberships that give a user outside of any organization access to the organizations", false, "MembershipManager");
		I_ASSIGN(m_invitationManagerCompPtr, "InvitationManager", "Pending invitations that give a user outside of any organization access to the organizations", false, "InvitationManager");
		I_ASSIGN(m_productInfoCompPtr, "FeatureContainer", "Product the permissions of a request are checked against", false, "FeatureContainer");
		I_ASSIGN(m_permissionTreeCompPtr, "PermissionTree", "Permissions whose requirements are resolved here when the server serves several products: it does not name the product of a request", false, "PermissionTree");
		I_ASSIGN(m_patPrefixAttrPtr, "PatPrefix", "Personal Access Token prefix", false, "imt_pat_");
		I_ASSIGN(m_maxTokenCacheSizeAttrPtr, "MaxTokenCacheSize", "Maximum number of cached tokens", false, 10000);
		I_ASSIGN(m_tokenCacheTtlAttrPtr, "TokenCacheLifetime", "Token cache lifetime (in secs)", false, 5 * 60);
		I_ASSIGN(m_tenantDataCacheTtlAttrPtr, "TenantDataCacheLifetime", "How long the scope, the ownership and the organization access of a token are cached (in secs)", false, 10);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextCreator)
	virtual imtgql::IGqlContextUniquePtr CreateGqlContext(
				const imtgql::IGqlContext::Headers& headers,
				imtgql::IGqlContextCreator::ContextCreationError& error) const override;
	virtual imtgql::IGqlContextUniquePtr RenewGqlContext(
				const imtgql::IGqlContext::Headers& headers,
				bool isEstablished,
				AuthorizationState& state,
				ContextCreationError& error,
				QDateTime* sessionExpiresAtPtr = nullptr) const override;
	virtual qint64 GetAuthorizationGeneration() const override;

	// reimplemented (imtauth::IJwtSessionController)
	virtual bool ValidateSession(const QByteArray& sessionId, bool* isCheckedPtr = nullptr) const override;
	virtual JwtState ValidateJwt(const QByteArray& jwt) const override;
	virtual bool RefreshToken(const QByteArray& refreshToken, UserSession& outputData) const override;
	virtual bool CreateNewSession(const QByteArray& userId, const QByteArray& tenantId, UserSession& outputData) const override;
	virtual imtauth::ISessionSharedPtr GetSession(const QByteArray& sessionId) const override;
	virtual bool RemoveSession(const QByteArray& sessionId) const override;
	virtual QByteArray GetUserFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetTenantFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetSessionFromJwt(const QByteArray& jwt) const override;
	virtual void InvalidateToken(const QByteArray& token) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelObserverBase)
	virtual void OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	// A token read afresh, without the cache
	struct FreshToken
	{
		QByteArray userId;
		QByteArray tenantId;
		QByteArrayList scopes;
		bool isPat = false;
		QByteArray productId;
		QByteArray clientProductId;
		/// Of a session token.
		QByteArray sessionId;
	};

	// Organization data of a token
	struct TokenTenantData
	{
		bool isTenantOwner = false;
		QByteArrayList tenantPermissions;
		// What the user holds inside the organization: its owner and its creator manage all of it, a member what the membership gives
		QByteArrayList organizationPermissions;
		bool hasOrganizationAccess = false;
		// False when a part could not be read: the data above may miss what the user holds
		bool isRead = true;
	};

	struct TokenCacheEntry
	{
		QByteArray userId;
		QByteArray tenantId;
		QByteArray tokenId;
		QByteArrayList scopes;
		bool isPat = false;
		QByteArray patProductId;
		QByteArray patClientProductId;
		qint64 expiresAt = 0;

		// Organization data of the token, resolved lazily (see TryGetCachedTenantData /
		// StoreCachedTenantData) and kept only for TenantDataCacheLifetime, so a changed
		// scope, owner or membership reaches open sessions soon. 'resolvedAt' 0 means
		// "never looked up yet", not a cached negative result.
		TokenTenantData tenantData;
		qint64 tenantDataResolvedAt = 0;
	};

	enum ContextKind
	{
		/// A request: the caches are used
		CK_REQUEST,
		/// A subscription about to be registered: read afresh, an expired access token is refused
		CK_NEW_SUBSCRIPTION,
		/// An established subscription: read afresh, an expired access token is accepted while its session lives
		CK_ESTABLISHED_SUBSCRIPTION
	};

	/**
		\param sessionExpiresAtPtr	[optional] For a subscription: when the session of the token ends, as it is known now.
	*/
	imtgql::IGqlContextUniquePtr CreateContext(
				const imtgql::IGqlContext::Headers& headers,
				ContextKind kind,
				imtgql::IGqlContextCreator::ContextCreationError& error,
				QDateTime* sessionExpiresAtPtr = nullptr) const;

	/**
		Check a token at its store or session controller, without the cache.
		\param managerPtr		The store of a personal access token or of a server token; not used for a session token.
		\param isExpiryAccepted	An expired access token holds while its session lives.
	*/
	bool ResolveFreshToken(
				const QByteArray& token,
				const imtauth::IPersonalAccessTokenManager* managerPtr,
				bool isExpiryAccepted,
				FreshToken& freshToken,
				imtgql::IGqlContextCreator::ContextCreationError& error) const;

	/**
		Read the user at its source. Its permissions are resolved on its roles and groups read at their sources too,
		unless the source computes the user: then only the permissions it was handed out with count.
		An absent user leaves userDataPtr invalid.
	*/
	bool ReadCurrentUser(
				const QByteArray& userId,
				const QByteArray& productId,
				imtbase::IObjectCollection::DataPtr& userDataPtr,
				imtgql::IGqlContextCreator::ContextCreationError& error) const;

	/**
		Whether the session of a token is known to be closed: its access token expired, or its session is gone.
		\param isChecked	False when the session could not be read.
	*/
	bool IsSessionClosed(const QByteArray& token, bool& isChecked) const;

	bool ResolveUserId(
				const QByteArray& token,
				QByteArray& userId,
				QByteArray& tenantId,
				QByteArrayList& scopes,
				bool& isPat,
				QString& errorMessage,
				imtgql::IGqlContextCreator::ContextCreationStatus& status) const;

	/**
		ResolveUserId() with the manager that checks a personal access token, so that the tokens of servers can be kept apart from the tokens of users.
	*/
	bool ResolveToken(
				const QByteArray& token,
				imtauth::IPersonalAccessTokenManager* patManagerPtr,
				QByteArray& userId,
				QByteArray& tenantId,
				QByteArrayList& scopes,
				bool& isPat,
				QString& errorMessage,
				imtgql::IGqlContextCreator::ContextCreationStatus& status) const;
	bool TryGetCachedToken(
				const QByteArray& token,
				QByteArray& userId,
				QByteArray& tenantId,
				QByteArrayList& scopes,
				bool& isPat) const;

	/**
		Resolve the token of the server of a product: valid, issued for this server.
		\param[out] clientProductId	The product whose server holds the token.
		\param[out] scopes			What the server may do here.
	*/
	bool ResolveServerToken(
				const QByteArray& serverToken,
				bool isRenewal,
				QByteArray& clientProductId,
				QByteArrayList& scopes,
				imtgql::IGqlContextCreator::ContextCreationError& error) const;

	bool GetCachedPatProducts(const QByteArray& token, QByteArray& productId, QByteArray& clientProductId) const;

	/**
		Cache lookup identical to TryGetCachedToken(), except it assumes
		m_tokenCacheMutex is already held by the caller. Used internally by
		TryAwaitOrClaimTokenResolution() so cache-check and claim-check happen
		under one continuous critical section.
	*/
	bool TryReadCachedTokenLocked(
				const QByteArray& token,
				QByteArray& userId,
				QByteArray& tenantId,
				QByteArrayList& scopes,
				bool& isPat) const;

	/**
		Cache lookup used by ResolveUserId(). Unlike TryGetCachedToken(), a miss
		does not simply return false: if another thread is already resolving
		this exact token (e.g. a burst of GraphQL requests arriving with the
		same freshly-issued JWT before any of them populated the cache), this
		call blocks until that thread finishes and reuses its result. Otherwise
		it claims the token for the calling thread and returns false, in which
		case the caller must resolve it and eventually call
		ReleaseTokenResolutionClaim() (regardless of success/failure) to release
		the claim and wake any waiters.
	*/
	bool TryAwaitOrClaimTokenResolution(
				const QByteArray& token,
				QByteArray& userId,
				QByteArray& tenantId,
				QByteArrayList& scopes,
				bool& isPat,
				JwtState& state) const;

	/**
		Releases a claim taken by TryAwaitOrClaimTokenResolution() and wakes any
		threads waiting on it. Must be called exactly once for every claim,
		whether the resolution succeeded or failed.
	*/
	void ReleaseTokenResolutionClaim(const QByteArray& token) const;

	void StoreCachedToken(
				const QByteArray& token,
				const QByteArray& userId,
				const QByteArray& tenantId,
				const QByteArray& tokenId,
				const QByteArrayList& scopes,
				bool isPat,
				qint64 jwtExpSecs = 0,
				const QByteArray& patProductId = QByteArray(),
				const QByteArray& patClientProductId = QByteArray(),
				qint64 resolutionGeneration = 0) const;
	void StoreExpiredToken(const QByteArray& token) const;
	void InvalidateTokenCache(const QByteArray& token) const;
	void InvalidateSessionTokenCache(const QByteArray& sessionId) const;

	/**
		Reads the organization data cached for 'token' while it is fresh. Returns false
		(nothing written) if the token isn't cached, its data was never resolved or it is
		older than TenantDataCacheLifetime - the caller then resolves it and stores it via
		StoreCachedTenantData().
	*/
	bool TryGetCachedTenantData(const QByteArray& token, TokenTenantData& tenantData) const;

	/**
		Records the resolved organization data for 'token' on its existing cache entry (a no-op
		if the token has no cache entry - callers only reach this after ResolveUserId() has
		already cached the token, or if the access changed since \c resolutionGeneration).
	*/
	void StoreCachedTenantData(const QByteArray& token, const TokenTenantData& tenantData, qint64 resolutionGeneration) const;

	/// Organization data of the user of a token, read without the cache.
	TokenTenantData ReadTenantData(const QByteArray& userId, const QByteArray& tenantId) const;

	/// What the user holds in an organization read with its owner, its creator and its permissions.
	void CompleteTenantData(
				const QByteArray& userId,
				const QByteArray& tenantId,
				const QByteArray& ownerId,
				const QByteArray& creatorId,
				const QByteArrayList& tenantPermissions,
				TokenTenantData& tenantData) const;

	/**
		The session authorization of a request cached for TenantDataCacheLifetime. Without one, the caller asks the
		authorization server; with c isClaimed set it then calls ReleaseTokenResolutionClaim(), so that the requests
		of a burst wait for one answer. A waiter that got none asks itself.
	*/
	bool TryAwaitOrClaimSessionAuthorization(
				const QByteArray& token,
				imtauth::ISessionAuthorizationProvider::SessionAuthorization& authorization,
				bool& isClaimed) const;
	void StoreCachedSessionAuthorization(
				const QByteArray& token,
				const imtauth::ISessionAuthorizationProvider::SessionAuthorization& authorization,
				qint64 jwtExpSecs,
				qint64 resolutionGeneration) const;

	/// A change of the access: checks started before it are stale, and the organization data of every token is read again.
	void InvalidateAuthorizationLocked() const;
	imtgql::IGqlContextUniquePtr CreateContextInstance() const;
	bool IsPatToken(const QByteArray& token) const;

	/**
		Tell whether a user outside of any organization sees the organizations: a member of one or a user with a pending invitation.
		\param isRead Cleared when the memberships or the invitations could not be read: a refusal then says nothing.
	*/
	bool HasOrganizationAccess(const QByteArray& userId, bool& isRead) const;

	/**
		Extract the 'exp' claim (seconds since epoch) from a JWT without going
		through the slave session controller. Returns 0 if the token is not a
		well-formed JWT or has no 'exp' claim.
	*/
	qint64 GetJwtExpirationSecs(const QByteArray& jwt) const;

	/**
		Bound a request through the server of a product by the token of that server.
		The user of a forwarded request keeps only the permissions the token holds too, and is no superuser.
		A server acting on its own is a user holding the permissions of its token.
	*/
	bool BindToServerToken(imtgql::IGqlContext& gqlContext, const QByteArray& productId, const QByteArray& clientProductId, const QByteArrayList& serverScopes) const;

	/**
		Decode standard claims from a JWT payload without remote calls or
		signature verification. Returns false if the token is not a well-formed
		three-part JWT. Out parameters that are not present in the payload are
		left empty / zero.
	*/
	bool ParseJwtClaims(
				const QByteArray& jwt,
				QByteArray& userId,
				QByteArray& tenantId,
				QByteArray& sessionId,
				qint64& expSecs) const;

	static QString MaskToken(const QByteArray& token);

	void SetError(
				imtgql::IGqlContextCreator::ContextCreationError& error,
				imtgql::IGqlContextCreator::ContextCreationStatus status,
				const QString& message = QString()) const;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_FACT(imtgql::IGqlContext, m_gqlContextFactCompPtr);
	I_REF(imtauth::IJwtSessionController, m_slaveJwtSessionControllerCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_patManagerCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::ITenantInvitationManager, m_invitationManagerCompPtr);
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
	I_REF(imtlic::IProductInfo, m_permissionTreeCompPtr);
	I_ATTR(QByteArray, m_patPrefixAttrPtr);
	I_ATTR(int, m_maxTokenCacheSizeAttrPtr);
	I_ATTR(int, m_tokenCacheTtlAttrPtr);
	I_ATTR(int, m_tenantDataCacheTtlAttrPtr);

	mutable QMutex m_tokenCacheMutex;
	mutable QHash<QByteArray, TokenCacheEntry> m_tokenCache;
	mutable QHash<QByteArray, qint64> m_expiredTokenCache;

	struct SessionAuthorizationCacheEntry
	{
		imtauth::ISessionAuthorizationProvider::SessionAuthorization authorization;
		qint64 resolvedAt = 0;
		qint64 jwtExpSecs = 0;
	};

	// The answers of the authorization server for the requests of a session; a subscription is never served from here
	mutable QHash<QByteArray, SessionAuthorizationCacheEntry> m_sessionAuthorizationCache;

	// Session-IDs removed via RemoveSession(), mapped to the time until which
	// StoreCachedToken() refuses to cache JWTs of that session. Guarded by
	// m_tokenCacheMutex so an in-flight validation/refresh cannot restore a
	// revoked session's token after InvalidateSessionTokenCache() swept it.
	mutable QHash<QByteArray, qint64> m_revokedSessionIds;

	// The same for personal access tokens: when the token managers last changed; a validation started before is not cached
	mutable qint64 m_tokenChangeGeneration = 0;
	mutable qint64 m_lastGlobalTokenChangeGeneration = 0;
	mutable QHash<QByteArray, qint64> m_tokenChangeGenerations;

	// Tokens currently being resolved by some thread via ResolveUserId(), so
	// concurrent callers for the same token can wait for that result instead
	// of each independently hitting the slave JWT/PAT controller. Guarded by
	// m_tokenCacheMutex; see TryAwaitOrClaimTokenResolution().
	mutable QSet<QByteArray> m_tokensBeingResolved;
	mutable QWaitCondition m_tokenResolutionWaitCondition;
};


} // namespace imtservergql
