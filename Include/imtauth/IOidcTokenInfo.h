// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtauth
{


/**
	Interface for describing an OIDC issued token.

	Tracks issued access and refresh tokens for revocation and introspection.
	Each token is associated with a client and optionally a user.

	\ingroup Authorization
*/
class IOidcTokenInfo: virtual public iser::ISerializable
{
public:
	/**
		Token type enumeration.
	*/
	enum OidcTokenType
	{
		OTT_ACCESS,
		OTT_REFRESH
	};

	I_DECLARE_ENUM(OidcTokenType, OTT_ACCESS, OTT_REFRESH)

	/**
		Get token identifier.
		\return Unique token ID (jti claim)
	*/
	virtual QByteArray GetTokenId() const = 0;

	/**
		Set token identifier.
		\param tokenId Token ID to set
	*/
	virtual void SetTokenId(const QByteArray& tokenId) = 0;

	/**
		Get client identifier that this token was issued to.
		\return Client ID
	*/
	virtual QByteArray GetClientId() const = 0;

	/**
		Set client identifier that this token was issued to.
		\param clientId Client ID to set
	*/
	virtual void SetClientId(const QByteArray& clientId) = 0;

	/**
		Get user identifier associated with this token.
		\return User ID, or empty for client_credentials grants
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set user identifier associated with this token.
		\param userId User ID to set
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
		Get the scope granted with this token.
		\return Space-separated scope string
	*/
	virtual QString GetScope() const = 0;

	/**
		Set the scope granted with this token.
		\param scope Space-separated scope string to set
	*/
	virtual void SetScope(const QString& scope) = 0;

	/**
		Get the token type.
		\return Token type (access or refresh)
	*/
	virtual OidcTokenType GetTokenType() const = 0;

	/**
		Set the token type.
		\param tokenType Token type to set
	*/
	virtual void SetTokenType(OidcTokenType tokenType) = 0;

	/**
		Get timestamp when this token expires.
		\return Expiration timestamp in UTC
	*/
	virtual QDateTime GetExpirationDate() const = 0;

	/**
		Set timestamp when this token expires.
		\param expirationDate Expiration timestamp in UTC to set
	*/
	virtual void SetExpirationDate(const QDateTime& expirationDate) = 0;

	/**
		Check if this token has been revoked.
		\return True if token is revoked
	*/
	virtual bool IsRevoked() const = 0;

	/**
		Set revocation status.
		\param revoked True to revoke the token
	*/
	virtual void SetRevoked(bool revoked) = 0;

	/**
		Get timestamp when this token was created.
		\return Creation timestamp in UTC
	*/
	virtual QDateTime GetCreatedAt() const = 0;

	/**
		Set timestamp when this token was created.
		\param createdAt Creation timestamp in UTC to set
	*/
	virtual void SetCreatedAt(const QDateTime& createdAt) = 0;
};


typedef istd::TUniqueInterfacePtr<IOidcTokenInfo> IOidcTokenInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IOidcTokenInfo> IOidcTokenInfoSharedPtr;


} // namespace imtauth


