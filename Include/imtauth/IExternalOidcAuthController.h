// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QString>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <imtauth/IExternalIdentity.h>


namespace imtauth
{


/**
	Interface for orchestrating external OIDC authentication flows.

	Manages the authorization code flow with external providers (Google, Apple, Facebook):
	- Generates authorization URLs for redirecting users to external providers
	- Handles callbacks with authorization codes, performs token exchange
	- Links external identities to local ImtCore users
	- Creates internal sessions after successful external authentication

	\ingroup Authorization
*/
class IExternalOidcAuthController: virtual public istd::IPolymorphic
{
public:
	/**
		Result of a successful external authentication callback.
	*/
	struct AuthResult
	{
		bool success = false;
		QString errorMessage;
		IJwtSessionController::UserSession session;
		QByteArray userId;
	};

	/**
		Information about a linked external provider.
	*/
	struct LinkedProvider
	{
		QByteArray provider;
		QString externalSubject;
		QString externalEmail;
		QDateTime linkedAt;
		QDateTime lastAuthAt;
	};

	/**
		Get the list of available external provider IDs.
		\return List of provider identifiers (e.g. "google", "apple", "facebook")
	*/
	virtual QByteArrayList GetAvailableProviders() const = 0;

	/**
		Build the authorization URL for redirecting the user to the external provider.
		\param providerId Provider identifier
		\param state CSRF state parameter
		\param nonce Nonce for ID token validation
		\return Full authorization redirect URL, or empty string on error
	*/
	virtual QString GetAuthorizationUrl(const QByteArray& providerId, const QByteArray& state, const QByteArray& nonce) const = 0;

	/**
		Handle the callback from the external provider after user authorization.
		Exchanges the authorization code for tokens, validates the ID token,
		links or creates the local user, and creates an internal session.
		\param providerId Provider identifier
		\param authCode Authorization code from the provider
		\param state CSRF state parameter for validation
		\return AuthResult containing session data on success, or error info on failure
	*/
	virtual AuthResult HandleCallback(const QByteArray& providerId, const QByteArray& authCode, const QByteArray& state) const = 0;

	/**
		Unlink an external provider from a user account.
		\param userId Local user ID
		\param providerId Provider identifier to unlink
		\return true if successfully unlinked
	*/
	virtual bool UnlinkProvider(const QByteArray& userId, const QByteArray& providerId) const = 0;

	/**
		Get the list of external providers linked to a user.
		\param userId Local user ID
		\return List of linked provider information
	*/
	virtual QList<LinkedProvider> GetLinkedProviders(const QByteArray& userId) const = 0;
};


} // namespace imtauth
