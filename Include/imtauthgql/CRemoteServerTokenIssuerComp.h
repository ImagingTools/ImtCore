// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauthgql/IServerTokenIssuer.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


/**
	The server tokens of a remote server over its GraphQL API, through a client whose address is set per request.
*/
class CRemoteServerTokenIssuerComp:
			public imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase>,
			virtual public IServerTokenIssuer
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteServerTokenIssuerComp);
		I_REGISTER_INTERFACE(IServerTokenIssuer);
	I_END_COMPONENT;

	// reimplemented (IServerTokenIssuer)
	virtual QByteArray SignIn(const ServerInfo& server, const QString& login, const QString& password, QString& errorMessage) const override;
	virtual bool GetPermissions(
				const ServerInfo& server,
				const QByteArray& sessionToken,
				const QByteArray& clientProductId,
				sdl::V1_0::imtauth::CServerTokenReceiver& permissions,
				QString& errorMessage) const override;
	virtual bool IssueToken(
				const ServerInfo& server,
				const QByteArray& sessionToken,
				const QByteArray& clientProductId,
				const QString& name,
				QByteArrayList& scopes,
				QByteArray& tokenId,
				QByteArray& token,
				QString& errorMessage) const override;
	virtual bool SetTokenScopes(
				const ServerInfo& server,
				const QByteArray& sessionToken,
				const QByteArray& clientProductId,
				const QByteArray& tokenId,
				QByteArrayList& scopes,
				QString& errorMessage) const override;
	virtual bool RevokeToken(const ServerInfo& server, const QByteArray& sessionToken, const QByteArray& tokenId, QString& errorMessage) const override;

private:
	template<class Payload>
	Payload Send(const ServerInfo& server, const QByteArray& sessionToken, imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	/**
		The permissions the token holds: the server completes the requested ones.
	*/
	bool GetTokenScopes(
				const ServerInfo& server,
				const QByteArray& sessionToken,
				const QByteArray& clientProductId,
				const QByteArray& tokenId,
				QByteArrayList& scopes,
				QString& errorMessage) const;
};


} // namespace imtauthgql


