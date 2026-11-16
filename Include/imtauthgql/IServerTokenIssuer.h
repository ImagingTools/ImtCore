// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ServerTokens.h>


namespace imtauthgql
{


/**
	The tokens a server of a product issues for itself to the servers of other products,
	managed remotely with a superuser session at that server.
*/
class IServerTokenIssuer: virtual public istd::IPolymorphic
{
public:
	/**
		The server issuing the tokens.
	*/
	struct ServerInfo
	{
		/**
			Address of its GraphQL endpoint.
		*/
		QUrl url;

		/**
			Its product: the tokens it issues are for it.
		*/
		QByteArray productId;
	};

	/**
		Sign the superuser in at the server.
		\return the token of the session, empty if the sign-in failed.
	*/
	virtual QByteArray SignInSuperuser(const ServerInfo& server, const QString& password, QString& errorMessage) const = 0;

	/**
		End a session opened by SignInSuperuser().
	*/
	virtual bool SignOut(const ServerInfo& server, const QByteArray& sessionToken, QString& errorMessage) const = 0;

	/**
		Make the product a client of the server; nothing changes when it is one already.
		\param isAdded	Set to true when the server has added the product now.
	*/
	virtual bool AddClient(
				const ServerInfo& server,
				const QByteArray& sessionToken,
				const QByteArray& clientProductId,
				const QString& name,
				bool& isAdded,
				QString& errorMessage) const = 0;

	/**
		Get the permissions a token of the client product can hold at the server.
	*/
	virtual bool GetPermissions(
				const ServerInfo& server,
				const QByteArray& sessionToken,
				const QByteArray& clientProductId,
				sdl::V1_0::imtauth::CServerTokenReceiver& permissions,
				QString& errorMessage) const = 0;

	/**
		Issue a token for the client product.
		\param scopes	Requested permissions; receives the ones the token holds.
	*/
	virtual bool IssueToken(
				const ServerInfo& server,
				const QByteArray& sessionToken,
				const QByteArray& clientProductId,
				const QString& name,
				QByteArrayList& scopes,
				QByteArray& tokenId,
				QByteArray& token,
				QString& errorMessage) const = 0;

	/**
		Change the permissions of a token.
		\param scopes	Requested permissions; receives the ones the token holds.
	*/
	virtual bool SetTokenScopes(
				const ServerInfo& server,
				const QByteArray& sessionToken,
				const QByteArray& tokenId,
				QByteArrayList& scopes,
				QString& errorMessage) const = 0;

	virtual bool RevokeToken(const ServerInfo& server, const QByteArray& sessionToken, const QByteArray& tokenId, QString& errorMessage) const = 0;
};


} // namespace imtauthgql


