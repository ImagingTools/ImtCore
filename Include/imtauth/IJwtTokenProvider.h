// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QByteArray>


namespace imtauth
{


/**
	Interface for RS256 JWT token operations.

	Provides asymmetric (RS256) JWT token generation and validation
	for OIDC compliance. This is separate from the existing
	CJwtSessionControllerComp which uses HMAC-SHA256 for internal sessions.

	\ingroup Authorization
*/
class IJwtTokenProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Generate a signed JWT token from claims.
		\param claims JSON object containing token claims (sub, iss, aud, exp, etc.)
		\return Signed JWT string, or empty on failure
	*/
	virtual QByteArray GenerateToken(const QJsonObject& claims) const = 0;

	/**
		Validate a JWT token and extract claims.
		\param jwt Signed JWT string to validate
		\param claims Output JSON object receiving the decoded claims
		\return True if the token is valid and not expired
	*/
	virtual bool ValidateToken(const QByteArray& jwt, QJsonObject& claims) const = 0;

	/**
		Get the JSON Web Key Set (JWKS) containing the public key(s).
		\return JSON-encoded JWKS document
	*/
	virtual QByteArray GetJwks() const = 0;

	/**
		Get the key identifier used in token headers.
		\return Key ID string
	*/
	virtual QByteArray GetKeyId() const = 0;
};


} // namespace imtauth


