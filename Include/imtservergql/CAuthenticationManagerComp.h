// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

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
#include <imtauth/ITenantManager.h>


namespace imtservergql
{


class CAuthenticationManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlContextCreator,
			virtual public imtauth::IJwtSessionController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAuthenticationManagerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextCreator);
		I_REGISTER_INTERFACE(imtauth::IJwtSessionController);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", false, "UserSettingsCollection");
		I_ASSIGN(m_gqlContextFactCompPtr, "GqlContextFactory", "GraphQL context factory", false, "GqlContextFactory");
		I_ASSIGN(m_slaveJwtSessionControllerCompPtr, "SlaveJwtSessionController", "Slave JWT session controller for delegation", false, "JwtSessionController");
		I_ASSIGN(m_patManagerCompPtr, "PersonalAccessTokenManager", "Personal Access Token manager", false, "PersonalAccessTokenManager");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for owner resolution", false, "TenantManager");
		I_ASSIGN(m_patPrefixAttrPtr, "PatPrefix", "Personal Access Token prefix", false, "imt_pat_");
		I_ASSIGN(m_maxTokenCacheSizeAttrPtr, "MaxTokenCacheSize", "Maximum number of cached tokens", false, 10000);
		I_ASSIGN(m_tokenCacheTtlAttrPtr, "TokenCacheLifetime", "Token cache lifetime (in secs)", false, 5 * 60);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextCreator)
	virtual imtgql::IGqlContextUniquePtr CreateGqlContext(
				const imtgql::IGqlContext::Headers& headers,
				imtgql::IGqlContextCreator::ContextCreationError& error) const override;

	// reimplemented (imtauth::IJwtSessionController)
	virtual bool ValidateSession(const QByteArray& sessionId) const override;
	virtual JwtState ValidateJwt(const QByteArray& jwt) const override;
	virtual bool RefreshToken(const QByteArray& refreshToken, UserSession& outputData) const override;
	virtual bool CreateNewSession(const QByteArray& userId, const QByteArray& tenantId, UserSession& outputData) const override;
	virtual imtauth::ISessionSharedPtr GetSession(const QByteArray& sessionId) const override;
	virtual bool RemoveSession(const QByteArray& sessionId) const override;
	virtual QByteArray GetUserFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetTenantFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetSessionFromJwt(const QByteArray& jwt) const override;
	virtual void InvalidateToken(const QByteArray& token) const override;

private:
	struct TokenCacheEntry
	{
		QByteArray userId;
		QByteArray tenantId;
		QByteArray tokenId;
		QByteArrayList scopes;
		bool isPat = false;
		qint64 expiresAt = 0;

		// Tenant-ownership flag, resolved lazily (see TryGetCachedTenantOwnership /
		// StoreCachedTenantOwnership) and cached alongside the rest of this token's
		// claims for the same TTL. 'resolved' distinguishes "not an owner" from
		// "never looked up yet" so a fresh entry doesn't get misread as a cached
		// negative result.
		bool isTenantOwner = false;
		bool tenantOwnershipResolved = false;
	};

	bool ResolveUserId(
				const QByteArray& token,
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
				bool& isPat) const;

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
				qint64 jwtExpSecs = 0) const;
	void InvalidateTokenCache(const QByteArray& token) const;

	/**
		Reads a previously-resolved tenant-ownership flag for 'token', if any.
		Returns false (nothing written to isTenantOwner) if the token isn't
		cached yet or its ownership was never resolved - the caller should then
		look it up (e.g. via the tenant manager) and store the result via
		StoreCachedTenantOwnership().
	*/
	bool TryGetCachedTenantOwnership(const QByteArray& token, bool& isTenantOwner) const;

	/**
		Records a resolved tenant-ownership flag for 'token' on its existing
		cache entry (a no-op if the token has no cache entry - callers only
		reach this after ResolveUserId() has already cached the token).
	*/
	void StoreCachedTenantOwnership(const QByteArray& token, bool isTenantOwner) const;
	imtgql::IGqlContextUniquePtr CreateContextInstance() const;
	bool IsPatToken(const QByteArray& token) const;

	/**
		Extract the 'exp' claim (seconds since epoch) from a JWT without going
		through the slave session controller. Returns 0 if the token is not a
		well-formed JWT or has no 'exp' claim.
	*/
	qint64 GetJwtExpirationSecs(const QByteArray& jwt) const;

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
	I_ATTR(QByteArray, m_patPrefixAttrPtr);
	I_ATTR(int, m_maxTokenCacheSizeAttrPtr);
	I_ATTR(int, m_tokenCacheTtlAttrPtr);

	mutable QMutex m_tokenCacheMutex;
	mutable QHash<QByteArray, TokenCacheEntry> m_tokenCache;

	// Tokens currently being resolved by some thread via ResolveUserId(), so
	// concurrent callers for the same token can wait for that result instead
	// of each independently hitting the slave JWT/PAT controller. Guarded by
	// m_tokenCacheMutex; see TryAwaitOrClaimTokenResolution().
	mutable QSet<QByteArray> m_tokensBeingResolved;
	mutable QWaitCondition m_tokenResolutionWaitCondition;
};


} // namespace imtservergql
