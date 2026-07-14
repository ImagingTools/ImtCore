// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens_fwd.h>


namespace imtauthgql
{


class CClientRequestPersonalAccessTokenManagerComp:
			virtual public imtauth::IPersonalAccessTokenManager,
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::V1_0::imtauth::CPersonalAccessTokensGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CPersonalAccessTokensGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CClientRequestPersonalAccessTokenManagerComp);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(imtauth::IPersonalAccessTokenManager);
		I_ASSIGN(m_tokenFactoryCompPtr, "PersonalAccessTokenFactory", "Personal access token factory", true, "PersonalAccessTokenFactory");
	I_END_COMPONENT;

	// reimplemented (imtauth::IPersonalAccessTokenManager)
	virtual TokenCreationResult CreateToken(
				const QByteArray& userId,
				const QByteArray& productId,
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

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (sdl::V1_0::imtauth::CPersonalAccessTokensGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CPersonalAccessTokenList OnGetTokenList(
				const sdl::V1_0::imtauth::CGetTokenListGqlRequest& getTokenListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CPersonalAccessToken OnGetToken(
				const sdl::V1_0::imtauth::CGetTokenGqlRequest& getTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CValidateTokenPayload OnValidateToken(
				const sdl::V1_0::imtauth::CValidateTokenGqlRequest& validateTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateTokenPayload OnCreateToken(
				const sdl::V1_0::imtauth::CCreateTokenGqlRequest& createTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRevokeTokenPayload OnRevokeToken(
				const sdl::V1_0::imtauth::CRevokeTokenGqlRequest& revokeTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CDeleteTokenPayload OnDeleteToken(
				const sdl::V1_0::imtauth::CDeleteTokenGqlRequest& deleteTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_FACT(imtauth::IPersonalAccessToken, m_tokenFactoryCompPtr);
};


} // namespace imtauthgql
