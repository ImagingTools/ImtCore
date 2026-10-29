// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QStringList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	Configuration of an external identity provider used for "Sign in with ..." (OAuth 2.0
	authorization code flow, OpenID Connect where the provider supports it).

	The server is a confidential client: it keeps the client secret and exchanges the
	authorization code itself, so the code and the provider tokens never reach the UI client.

	\ingroup Authorization
*/
class IExternalOidcProvider: virtual public istd::IPolymorphic
{
public:
	typedef QList<QPair<QString, QString>> QueryItems;

	/**
		Unique provider ID used in the API and stored with linked accounts (e.g. "google").
	*/
	virtual QByteArray GetProviderId() const = 0;

	/**
		Name shown on the sign-in button.
	*/
	virtual QString GetProviderName() const = 0;

	/**
		Brand of the provider: "Google", "Apple", "Facebook" or "Generic". The UI styles the button by it.
	*/
	virtual QByteArray GetProviderType() const = 0;

	virtual QString GetAuthorizationEndpoint() const = 0;
	virtual QString GetTokenEndpoint() const = 0;

	/**
		Endpoint returning the account claims for the access token.
		Empty if the claims are taken from the ID token only.
	*/
	virtual QString GetUserInfoEndpoint() const = 0;

	/**
		Accepted values of the 'iss' claim of the ID token. Empty disables the check.
	*/
	virtual QStringList GetIssuers() const = 0;

	virtual QByteArray GetClientId() const = 0;
	virtual QByteArray GetClientSecret() const = 0;

	/**
		Space-separated scopes requested from the provider.
	*/
	virtual QString GetScopes() const = 0;

	/**
		Additional query items of the authorization request (e.g. response_mode=form_post).
	*/
	virtual QueryItems GetAuthorizationParams() const = 0;

	/**
		Name of the claim holding the account identifier ('sub' for OpenID Connect providers).
	*/
	virtual QString GetSubjectClaim() const = 0;

	/**
		True if the provider issues an ID token, so a nonce is sent and verified.
	*/
	virtual bool IsOpenIdConnect() const = 0;

	virtual bool IsPkceSupported() const = 0;

	/**
		True if every e-mail the provider returns is verified, even without an 'email_verified' claim.
	*/
	virtual bool IsEmailVerifiedByProvider() const = 0;
};


} // namespace imtauth


