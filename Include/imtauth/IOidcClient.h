// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// Qt includes
#include <QtCore/QStringList>


namespace imtauth
{


/**
	Interface for describing an OIDC client registration.

	OIDC clients represent applications that use OpenID Connect for authentication.
	Each client has a unique identifier, optional secret, and configuration for
	allowed redirect URIs, grant types, and scopes.

	\ingroup Authorization
*/
class IOidcClient: virtual public iser::ISerializable
{
public:
	/**
		Supported OIDC grant types.
	*/
	enum OidcGrantType
	{
		OGT_AUTHORIZATION_CODE,
		OGT_CLIENT_CREDENTIALS,
		OGT_REFRESH_TOKEN
	};

	I_DECLARE_ENUM(OidcGrantType, OGT_AUTHORIZATION_CODE, OGT_CLIENT_CREDENTIALS, OGT_REFRESH_TOKEN)

	/**
		Get client identifier.
		\return Unique client ID
	*/
	virtual QByteArray GetClientId() const = 0;

	/**
		Set client identifier.
		\param clientId Client ID to set
	*/
	virtual void SetClientId(const QByteArray& clientId) = 0;

	/**
		Get client secret hash.
		\return Hashed client secret
	*/
	virtual QByteArray GetClientSecretHash() const = 0;

	/**
		Set client secret hash.
		\param secretHash Hashed client secret to set
	*/
	virtual void SetClientSecretHash(const QByteArray& secretHash) = 0;

	/**
		Get human-readable client name.
		\return Client name
	*/
	virtual QString GetClientName() const = 0;

	/**
		Set human-readable client name.
		\param name Client name to set
	*/
	virtual void SetClientName(const QString& name) = 0;

	/**
		Get list of allowed redirect URIs.
		\return List of redirect URIs
	*/
	virtual QStringList GetRedirectUris() const = 0;

	/**
		Set list of allowed redirect URIs.
		\param uris Redirect URIs to set
	*/
	virtual void SetRedirectUris(const QStringList& uris) = 0;

	/**
		Get list of allowed grant types.
		\return List of grant type identifiers
	*/
	virtual QByteArrayList GetGrantTypes() const = 0;

	/**
		Set list of allowed grant types.
		\param grantTypes Grant type identifiers to set
	*/
	virtual void SetGrantTypes(const QByteArrayList& grantTypes) = 0;

	/**
		Get list of allowed scopes.
		\return List of scope identifiers
	*/
	virtual QByteArrayList GetScopes() const = 0;

	/**
		Set list of allowed scopes.
		\param scopes Scope identifiers to set
	*/
	virtual void SetScopes(const QByteArrayList& scopes) = 0;

	/**
		Get timestamp when the client was registered.
		\return Creation timestamp in UTC
	*/
	virtual QDateTime GetCreatedAt() const = 0;

	/**
		Set timestamp when the client was registered.
		\param createdAt Creation timestamp in UTC to set
	*/
	virtual void SetCreatedAt(const QDateTime& createdAt) = 0;
};


typedef istd::TUniqueInterfacePtr<IOidcClient> IOidcClientUniquePtr;
typedef istd::TSharedInterfacePtr<IOidcClient> IOidcClientSharedPtr;


} // namespace imtauth


