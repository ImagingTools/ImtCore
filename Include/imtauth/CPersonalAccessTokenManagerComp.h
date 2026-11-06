// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IPersonalAccessTokenCache.h>
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CPersonalAccessTokenManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IPersonalAccessTokenManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPersonalAccessTokenManagerComp);
		I_REGISTER_INTERFACE(imtauth::IPersonalAccessTokenManager);
		I_ASSIGN(m_tokenCollectionCompPtr, "TokenCollection", "Personal access token collection", true, "PersonalAccessTokenCollection");
		I_ASSIGN(m_tokenFactoryCompPtr, "TokenFactory", "Personal access token factory", true, "PersonalAccessTokenFactory");
		I_ASSIGN(m_tokenCacheCompPtr, "PersonalAccessTokenCache", "Forgets a revoked or deleted token at once", false, "PersonalAccessTokenCache");
	I_END_COMPONENT;

	// reimplemented (imtauth::IPersonalAccessTokenManager)
	virtual TokenCreationResult CreateToken(
		const QByteArray& userId,
		const QByteArray& productId,
		const QString& name,
		const QString& description,
		const QByteArrayList& scopes,
		const QDateTime& expiresAt,
				const QByteArray& clientProductId = QByteArray()) override;
	virtual bool ValidateToken(
				const QByteArray& rawToken,
				QByteArray& userId,
				QByteArray& tokenId,
				QByteArrayList& scopes,
				QByteArray* productIdPtr = nullptr,
				QByteArray* clientProductIdPtr = nullptr,
				bool* isCheckFailedPtr = nullptr) const override;
	virtual QByteArrayList GetServerTokenIds(const QByteArray& clientProductId, bool* isReadPtr = nullptr) const override;
	virtual QByteArrayList GetTokenIds(const QByteArray& userId) const override;
	virtual IPersonalAccessTokenSharedPtr GetToken(const QByteArray& tokenId) const override;
	virtual bool RevokeToken(const QByteArray& tokenId) override;
	virtual bool UpdateLastUsedAt(const QByteArray& tokenId) override;
	virtual bool DeleteToken(const QByteArray& tokenId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	QByteArray GenerateRandomToken() const;
	QByteArray HashToken(const QByteArray& rawToken) const;
	QByteArray ExtractTokenId(const QByteArray& rawToken) const;

private:
	I_REF(imtbase::IObjectCollection, m_tokenCollectionCompPtr);
	I_FACT(imtauth::IPersonalAccessToken, m_tokenFactoryCompPtr);
	I_REF(imtauth::IPersonalAccessTokenCache, m_tokenCacheCompPtr);
};


} // namespace imtauth


