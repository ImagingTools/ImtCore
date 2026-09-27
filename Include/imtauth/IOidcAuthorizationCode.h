// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtauth
{


/**
	Interface for describing an OIDC authorization code.

	Authorization codes are short-lived tokens issued during the authorization
	code flow. They are exchanged for access tokens at the token endpoint.
	Supports PKCE via code_challenge for public clients.

	\ingroup Authorization
*/
class IOidcAuthorizationCode: virtual public iser::ISerializable
{
public:
	/**
		Get authorization code value.
		\return Authorization code string
	*/
	virtual QByteArray GetCode() const = 0;

	/**
		Set authorization code value.
		\param code Authorization code to set
	*/
	virtual void SetCode(const QByteArray& code) = 0;

	/**
		Get client identifier that requested this code.
		\return Client ID
	*/
	virtual QByteArray GetClientId() const = 0;

	/**
		Set client identifier that requested this code.
		\param clientId Client ID to set
	*/
	virtual void SetClientId(const QByteArray& clientId) = 0;

	/**
		Get user identifier who authorized this code.
		\return User ID
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set user identifier who authorized this code.
		\param userId User ID to set
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
		Get the scope granted with this authorization code.
		\return Space-separated scope string
	*/
	virtual QString GetScope() const = 0;

	/**
		Set the scope granted with this authorization code.
		\param scope Space-separated scope string to set
	*/
	virtual void SetScope(const QString& scope) = 0;

	/**
		Get the redirect URI associated with this code.
		\return Redirect URI
	*/
	virtual QString GetRedirectUri() const = 0;

	/**
		Set the redirect URI associated with this code.
		\param redirectUri Redirect URI to set
	*/
	virtual void SetRedirectUri(const QString& redirectUri) = 0;

	/**
		Get timestamp when this code expires.
		\return Expiration timestamp in UTC
	*/
	virtual QDateTime GetExpirationDate() const = 0;

	/**
		Set timestamp when this code expires.
		\param expirationDate Expiration timestamp in UTC to set
	*/
	virtual void SetExpirationDate(const QDateTime& expirationDate) = 0;

	/**
		Get the PKCE code challenge.
		\return Code challenge string, empty if PKCE not used
	*/
	virtual QByteArray GetCodeChallenge() const = 0;

	/**
		Set the PKCE code challenge.
		\param codeChallenge Code challenge to set
	*/
	virtual void SetCodeChallenge(const QByteArray& codeChallenge) = 0;

	/**
		Get the PKCE code challenge method.
		\return Code challenge method (e.g. "S256" or "plain")
	*/
	virtual QByteArray GetCodeChallengeMethod() const = 0;

	/**
		Set the PKCE code challenge method.
		\param method Code challenge method to set
	*/
	virtual void SetCodeChallengeMethod(const QByteArray& method) = 0;

	/**
		Check if this code has been used.
		\return True if code has been consumed
	*/
	virtual bool IsUsed() const = 0;

	/**
		Set whether this code has been used.
		\param used True to mark code as consumed
	*/
	virtual void SetUsed(bool used) = 0;
};


typedef istd::TUniqueInterfacePtr<IOidcAuthorizationCode> IOidcAuthorizationCodeUniquePtr;
typedef istd::TSharedInterfacePtr<IOidcAuthorizationCode> IOidcAuthorizationCodeSharedPtr;


} // namespace imtauth


