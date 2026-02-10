// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtauth
{


/**
	Interface for describing a personal access token.
	
	Personal access tokens (PATs) provide a way to authenticate API requests
	without using passwords. Each token has a unique identifier, is associated
	with a user, and can have scoped permissions and an expiration date.
	
	\ingroup Authorization
*/
class IPersonalAccessToken: virtual public iser::ISerializable
{
public:
	/**
		Get token identifier.
		\return Unique token ID
	*/
	virtual QByteArray GetId() const = 0;
	
	/**
		Set token identifier.
		\param id Token ID to set
	*/
	virtual void SetId(const QByteArray& id) = 0;
	
	/**
		Get user identifier who owns this token.
		\return User ID
	*/
	virtual QByteArray GetUserId() const = 0;
	
	/**
		Set user identifier who owns this token.
		\param userId User ID to set
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;
	
	/**
		Get human-readable token name.
		\return Token name
	*/
	virtual QString GetName() const = 0;
	
	/**
		Set human-readable token name.
		\param name Token name to set
	*/
	virtual void SetName(const QString& name) = 0;
	
	/**
		Get token description explaining its purpose.
		\return Token description
	*/
	virtual QString GetDescription() const = 0;
	
	/**
		Set token description explaining its purpose.
		\param description Token description to set
	*/
	virtual void SetDescription(const QString& description) = 0;
	
	/**
		Get SHA-256 hash of the token.
		The actual token value is never stored, only its hash for security.
		\return Token hash in hexadecimal format
	*/
	virtual QByteArray GetTokenHash() const = 0;
	
	/**
		Set SHA-256 hash of the token.
		\param tokenHash Token hash to set
	*/
	virtual void SetTokenHash(const QByteArray& tokenHash) = 0;
	
	/**
		Get list of permission scopes granted to this token.
		\return List of scope identifiers
	*/
	virtual QByteArrayList GetScopes() const = 0;
	
	/**
		Set list of permission scopes granted to this token.
		\param scopes List of scope identifiers to set
	*/
	virtual void SetScopes(const QByteArrayList& scopes) = 0;
	
	/**
		Get timestamp when the token was created.
		\return Creation timestamp in UTC
	*/
	virtual QDateTime GetCreatedAt() const = 0;
	
	/**
		Set timestamp when the token was created.
		\param createdAt Creation timestamp in UTC to set
	*/
	virtual void SetCreatedAt(const QDateTime& createdAt) = 0;
	
	/**
		Get timestamp when the token was last used.
		\return Last used timestamp in UTC, or invalid QDateTime if never used
	*/
	virtual QDateTime GetLastUsedAt() const = 0;
	
	/**
		Set timestamp when the token was last used.
		\param lastUsedAt Last used timestamp in UTC to set
	*/
	virtual void SetLastUsedAt(const QDateTime& lastUsedAt) = 0;
	
	/**
		Get timestamp when the token expires.
		\return Expiration timestamp in UTC, or invalid QDateTime if no expiration
	*/
	virtual QDateTime GetExpiresAt() const = 0;
	
	/**
		Set timestamp when the token expires.
		\param expiresAt Expiration timestamp in UTC to set, or invalid QDateTime for no expiration
	*/
	virtual void SetExpiresAt(const QDateTime& expiresAt) = 0;
	
	/**
		Check if the token has been revoked.
		\return True if token is revoked, false otherwise
	*/
	virtual bool IsRevoked() const = 0;
	
	/**
		Set revocation status of the token.
		\param revoked True to revoke the token, false otherwise
	*/
	virtual void SetRevoked(bool revoked) = 0;
	
	/**
		Check if the token has expired.
		\return True if token has passed its expiration date, false otherwise
	*/
	virtual bool IsExpired() const = 0;
	
	/**
		Check if the token is valid (not revoked and not expired).
		\return True if token can be used for authentication, false otherwise
	*/
	virtual bool IsValid() const = 0;
};


typedef istd::TUniqueInterfacePtr<IPersonalAccessToken> IPersonalAccessTokenUniquePtr;
typedef istd::TSharedInterfacePtr<IPersonalAccessToken> IPersonalAccessTokenSharedPtr;


} // namespace imtauth


