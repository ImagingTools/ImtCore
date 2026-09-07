// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace imtauth
{


/**
	Interface for describing an external OIDC provider configuration.

	External OIDC providers (Google, Apple, Facebook) are used to authenticate
	users via third-party identity services. Each provider has a unique identifier,
	discovery URL, client credentials, and requested scopes.

	\ingroup Authorization
*/
class IExternalOidcProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get provider identifier.
		\return Unique provider name (e.g. "google", "apple", "facebook")
	*/
	virtual QByteArray GetProviderId() const = 0;

	/**
		Get the OpenID Connect discovery URL.
		\return URL to .well-known/openid-configuration, or empty for non-standard providers
	*/
	virtual QString GetDiscoveryUrl() const = 0;

	/**
		Get the authorization endpoint URL.
		\return URL to the provider's authorization endpoint
	*/
	virtual QString GetAuthorizationEndpoint() const = 0;

	/**
		Get the token endpoint URL.
		\return URL to the provider's token endpoint
	*/
	virtual QString GetTokenEndpoint() const = 0;

	/**
		Get the user info endpoint URL.
		\return URL to the provider's user info endpoint
	*/
	virtual QString GetUserInfoEndpoint() const = 0;

	/**
		Get the OAuth client ID for this provider.
		\return Client ID registered with the external provider
	*/
	virtual QByteArray GetClientId() const = 0;

	/**
		Get the OAuth client secret for this provider.
		\return Client secret registered with the external provider
	*/
	virtual QByteArray GetClientSecret() const = 0;

	/**
		Get the requested scopes.
		\return Space-separated list of scopes (e.g. "openid profile email")
	*/
	virtual QString GetScopes() const = 0;

	/**
		Get the redirect URI back to ImtCore.
		\return Callback URL for the authorization code flow
	*/
	virtual QString GetRedirectUri() const = 0;
};


} // namespace imtauth
