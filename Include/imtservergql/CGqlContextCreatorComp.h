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


class CGqlContextCreatorComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlContextCreator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlContextCreatorComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextCreator);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", false, "UserSettingsCollection");
		I_ASSIGN(m_gqlContextFactCompPtr, "GqlContextFactory", "GraphQL context factory", true, "GqlContextFactory");
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "JWT session controller", false, "JwtSessionController");
		I_ASSIGN(m_patManagerCompPtr, "PersonalAccessTokenManager", "Personal Access Token manager", false, "PersonalAccessTokenManager");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextCreator)
	virtual imtgql::IGqlContextUniquePtr CreateGqlContext(
				const QByteArray& token,
				const QByteArray& productId,
				const QByteArray& userId,
				const imtgql::IGqlContext::Headers& headers,
				QString& errorMessage,
				imtgql::IGqlContextCreator::ContextCreationStatus* statusPtr = nullptr) const override;

private:
	struct TokenCacheEntry
	{
		QByteArray userId;
		QByteArray tokenId;
		bool isPat = false;
		qint64 expiresAt = 0;
	};

	bool ResolveUserId(
				const QByteArray& token,
				QByteArray& userId,
				QString& errorMessage,
				imtgql::IGqlContextCreator::ContextCreationStatus& status) const;
	bool TryGetCachedToken(const QByteArray& token, QByteArray& userId) const;
	void StoreCachedToken(
				const QByteArray& token,
				const QByteArray& userId,
				const QByteArray& tokenId,
				bool isPat) const;
	imtgql::IGqlContextUniquePtr CreateContextInstance() const;
	bool IsPatToken(const QByteArray& token) const;
	void SetStatus(
				imtgql::IGqlContextCreator::ContextCreationStatus* statusPtr,
				imtgql::IGqlContextCreator::ContextCreationStatus status) const;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_FACT(imtgql::IGqlContext, m_gqlContextFactCompPtr);
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_patManagerCompPtr);

	mutable QMutex m_contextFactoryMutex;
	mutable QMutex m_authMutex;
	mutable QMutex m_tokenCacheMutex;
	mutable QHash<QByteArray, TokenCacheEntry> m_tokenCache;
};


} // namespace imtservergql
