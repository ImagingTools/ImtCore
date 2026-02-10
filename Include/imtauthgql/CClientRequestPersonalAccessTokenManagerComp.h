// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtauthgql/CClientRequestManagerCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens.h>


namespace imtauthgql
{


class CClientRequestPersonalAccessTokenManagerComp:
			public imtauthgql::CClientRequestManagerCompBase,
			virtual public imtauth::IPersonalAccessTokenManager
{
public:
	typedef imtauthgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestPersonalAccessTokenManagerComp)
		I_REGISTER_INTERFACE(imtauth::IPersonalAccessTokenManager);
		I_ASSIGN(m_tokenFactoryCompPtr, "PersonalAccessTokenFactory", "Personal access token factory", true, "PersonalAccessTokenFactory");
	I_END_COMPONENT;

	// reimplemented (imtauth::IPersonalAccessTokenManager)
	virtual TokenCreationResult CreateToken(
				const QByteArray& userId,
				const QString& name,
				const QString& description,
				const QByteArrayList& scopes,
				const QDateTime& expiresAt) override;
	virtual bool ValidateToken(const QByteArray& rawToken, QByteArray& userId, QByteArray& tokenId, QByteArrayList& scopes) const override;
	virtual QByteArrayList GetTokenIds(const QByteArray& userId) const override;
	virtual imtauth::IPersonalAccessTokenSharedPtr GetToken(const QByteArray& tokenId) const override;
	virtual bool RevokeToken(const QByteArray& tokenId) override;
	virtual bool UpdateLastUsedAt(const QByteArray& tokenId) override;
	virtual bool DeleteToken(const QByteArray& tokenId) override;

private:
	I_FACT(imtauth::IPersonalAccessToken, m_tokenFactoryCompPtr);
};


} // namespace imtauthgql


