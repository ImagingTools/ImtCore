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
		I_ASSIGN(m_gqlContextFactCompPtr, "GqlContextFactory", "GraphQL context factory", true, "GqlContextFactory");
		I_ASSIGN(m_slaveJwtSessionControllerCompPtr, "SlaveJwtSessionController", "Slave JWT session controller for delegation", false, "JwtSessionController");
		I_ASSIGN(m_patManagerCompPtr, "PersonalAccessTokenManager", "Personal Access Token manager", false, "PersonalAccessTokenManager");
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

private:
	enum TokenCacheLookupResult
	{
		TCLR_MISS,
		TCLR_VALID,
		TCLR_INVALID
	};

	struct TokenCacheEntry
	{
		QByteArray userId;
		QByteArray tenantId;
		QByteArray tokenId;
		QByteArrayList scopes;
		bool isPat = false;
		bool isValid = true;
		imtgql::IGqlContextCreator::ContextCreationStatus status = imtgql::IGqlContextCreator::CCS_OK;
		qint64 expiresAt = 0;
	};

	bool ResolveUserId(
				const QByteArray& token,
				QByteArray& userId,
				QByteArray& tenantId,
				QByteArrayList& scopes,
				bool& isPat,
				QString& errorMessage,
				imtgql::IGqlContextCreator::ContextCreationStatus& status) const;
	TokenCacheLookupResult TryGetCachedToken(
				const QByteArray& token,
				QByteArray& userId,
				QByteArray& tenantId,
				QByteArrayList& scopes,
				bool& isPat,
				QString& errorMessage,
				imtgql::IGqlContextCreator::ContextCreationStatus& status) const;
	void StoreCachedToken(
				const QByteArray& token,
				const QByteArray& userId,
				const QByteArray& tenantId,
				const QByteArray& tokenId,
				const QByteArrayList& scopes,
				bool isPat,
				bool isValid,
				imtgql::IGqlContextCreator::ContextCreationStatus status,
				qint64 ttlMs) const;
	void InvalidateTokenCache(const QByteArray& token) const;
	imtgql::IGqlContextUniquePtr CreateContextInstance() const;
	bool IsPatToken(const QByteArray& token) const;
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

	// TODO: Remove this mutex if the ACF component factory (I_FACT) is confirmed thread-safe.
	// Currently serializes all context creation which may become a bottleneck under load.
	mutable QMutex m_contextCreationMutex;
	// TODO: Remove this mutex if IJwtSessionController and IPersonalAccessTokenManager
	// implementations are confirmed thread-safe. Currently serializes all token validation.
	mutable QMutex m_tokenValidationMutex;
	mutable QMutex m_tokenCacheMutex;
	mutable QHash<QByteArray, TokenCacheEntry> m_tokenCache;
};


} // namespace imtservergql
