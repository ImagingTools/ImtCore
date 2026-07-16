// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>

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
};


} // namespace imtservergql
