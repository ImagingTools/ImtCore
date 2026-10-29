// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QString>

// ImtCore includes
#include <imtauth/IExternalLoginCallbackHandler.h>


namespace imtauth
{


/**
	Sign-in with external identity providers (authorization code flow) and the external
	accounts linked to the users.

	A login is started by the UI client with BeginLogin, which returns the provider URL the
	user opens in a browser and a login ID known only to the client. The provider redirects
	the browser to the server callback (HandleCallback); the client polls TakeLoginResult
	with the login ID until the login is finished.

	\ingroup Authorization
*/
class IExternalLoginController: virtual public IExternalLoginCallbackHandler
{
public:
	struct ProviderInfo
	{
		QByteArray id;
		QString name;
		QByteArray type;
	};

	typedef QList<ProviderInfo> Providers;

	struct LinkedAccount
	{
		QByteArray providerId;
		QString email;
		QDateTime linkedAt;
		QDateTime lastSignInAt;
	};

	typedef QList<LinkedAccount> LinkedAccounts;

	enum LoginStatus
	{
		/**
			Unknown or expired login ID, or the result was already taken.
		*/
		LS_UNKNOWN,
		/**
			The provider has not redirected the browser back yet.
		*/
		LS_PENDING,
		/**
			The user is authenticated, the result holds the user ID.
		*/
		LS_SIGNED_IN,
		/**
			The external account was linked to the user who started the login.
		*/
		LS_LINKED,
		LS_FAILED
	};

	struct LoginResult
	{
		LoginStatus status = LS_UNKNOWN;
		QByteArray userId;
		QByteArray productId;
		QString message;
	};

	virtual Providers GetProviders() const = 0;

	/**
		Start a login with the provider.
		\param linkUserId	empty to sign in, otherwise the user the external account is linked to.
		\param loginId		[out] secret ID of the login, used to take the result.
		\param authorizationUrl	[out] URL of the provider the user has to open.
	*/
	virtual bool BeginLogin(
				const QByteArray& providerId,
				const QByteArray& productId,
				const QByteArray& linkUserId,
				QByteArray& loginId,
				QString& authorizationUrl,
				QString& errorMessage) const = 0;

	/**
		Current state of the login. A finished login (signed in, linked or failed) is returned once.
	*/
	virtual LoginResult TakeLoginResult(const QByteArray& loginId) const = 0;

	virtual LinkedAccounts GetLinkedAccounts(const QByteArray& userId) const = 0;

	/**
		Remove the link of the user's account at the provider. Refused for the last sign-in method of a user without password.
	*/
	virtual bool UnlinkAccount(const QByteArray& userId, const QByteArray& providerId, QString& errorMessage) const = 0;
};


} // namespace imtauth


