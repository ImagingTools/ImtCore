// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>


namespace imtauth
{


/**
	Interface for managing personal access tokens.

	This interface provides operations for creating, validating, revoking, and
	managing personal access tokens (PATs) for API authentication. Tokens are
	securely generated and stored as SHA-256 hashes for security.

	\ingroup Authorization
*/
class IPersonalAccessTokenManager: virtual public istd::IPolymorphic
{
public:
	/**
		Result structure returned by CreateToken operation.
	*/
	struct TokenCreationResult
	{
		QByteArray tokenId;   ///< Unique identifier for the created token
		QByteArray rawToken;  ///< Plain text token value (returned only once at creation)
		bool success;         ///< True if token was created successfully
	};

	/**
		Create a new personal access token.

		Generates a cryptographically secure random token, hashes it with SHA-256,
		and stores it in the collection. The raw token value is returned only once
		and should be saved by the caller as it cannot be retrieved later.

		\param userId User ID who owns the token
		\param name Human-readable name for the token
		\param description Description of the token's purpose
		\param scopes List of permission scopes to grant to this token
		\param expiresAt Optional expiration timestamp (invalid QDateTime for no expiration)
		\return TokenCreationResult containing the token ID and raw token value
	*/
	virtual TokenCreationResult CreateToken(
		const QByteArray& userId,
		const QString& name,
		const QString& description,
		const QByteArrayList& scopes,
		const QDateTime& expiresAt) = 0;

	/**
		Validate a personal access token.

		Checks if the provided token is valid by comparing its hash against stored
		hashes. Validates that the token is not revoked and not expired. Uses
		constant-time comparison to prevent timing attacks.

		\param rawToken Plain text token value to validate
		\param[out] userId User ID associated with the token (if valid)
		\param[out] tokenId Token ID associated with the token (if valid)
		\param[out] scopes Permission scopes granted to the token (if valid)
		\return True if token is valid, false if invalid, revoked, or expired
	*/
	virtual bool ValidateToken(const QByteArray& rawToken, QByteArray& userId, QByteArray& tokenId, QByteArrayList& scopes) const = 0;

	/**
		Get list of token IDs for a specific user.

		\param userId User ID to get tokens for
		\return List of token IDs owned by the user
	*/
	virtual QByteArrayList GetTokenIds(const QByteArray& userId) const = 0;

	/**
		Get token information by token ID.
		
		Returns a copy of the token object with all metadata except the raw token
		value (which is never stored or retrievable).

		\param tokenId Token ID to retrieve
		\return Shared pointer to token object, or nullptr if not found
	*/
	virtual IPersonalAccessTokenSharedPtr GetToken(const QByteArray& tokenId) const = 0;

	/**
		Revoke a personal access token.

		Marks the token as revoked, immediately invalidating it for authentication.
		The token remains in the collection for audit purposes but cannot be used.

		\param tokenId Token ID to revoke
		\return True if token was revoked successfully, false if token not found
	*/
	virtual bool RevokeToken(const QByteArray& tokenId) = 0;

	/**
		Update the last used timestamp for a token.

		Records when the token was last successfully used for authentication.
		This information can be used for monitoring and security auditing.

		\param tokenId Token ID to update
		\return True if timestamp was updated successfully, false if token not found
	*/
	virtual bool UpdateLastUsedAt(const QByteArray& tokenId) = 0;

	/**
		Delete a personal access token.

		Permanently removes the token from the collection. This operation cannot
		be undone. Consider using RevokeToken() instead for audit trail purposes.

		\param tokenId Token ID to delete
		\return True if token was deleted successfully, false if token not found
	*/
	virtual bool DeleteToken(const QByteArray& tokenId) = 0;
};


} // namespace imtauth


