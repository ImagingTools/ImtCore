// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QString>


namespace imtauth
{


/**
	Interface for linking an external provider account to an ImtCore user.

	Each external identity maps a provider's subject claim (sub) to a local
	ImtCore user ID. This enables users to authenticate via multiple external
	providers while maintaining a single internal account.

	\ingroup Authorization
*/
class IExternalIdentity: virtual public iser::ISerializable
{
public:
	/**
		Get the unique identity record ID.
		\return UUID of this external identity link
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique identity record ID.
		\param id UUID to set
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the local ImtCore user ID.
		\return UUID of the linked local user
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set the local ImtCore user ID.
		\param userId UUID of the local user
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
		Get the provider identifier.
		\return Provider name (e.g. "google", "apple", "facebook")
	*/
	virtual QByteArray GetProvider() const = 0;

	/**
		Set the provider identifier.
		\param provider Provider name
	*/
	virtual void SetProvider(const QByteArray& provider) = 0;

	/**
		Get the external subject claim.
		\return The 'sub' claim from the external provider's ID token
	*/
	virtual QString GetExternalSubject() const = 0;

	/**
		Set the external subject claim.
		\param subject The 'sub' claim value
	*/
	virtual void SetExternalSubject(const QString& subject) = 0;

	/**
		Get the external email address.
		\return Email from the external provider, or empty string
	*/
	virtual QString GetExternalEmail() const = 0;

	/**
		Set the external email address.
		\param email Email from the external provider
	*/
	virtual void SetExternalEmail(const QString& email) = 0;

	/**
		Get the timestamp when this identity was linked.
		\return Link creation timestamp in UTC
	*/
	virtual QDateTime GetLinkedAt() const = 0;

	/**
		Set the timestamp when this identity was linked.
		\param linkedAt Link creation timestamp in UTC
	*/
	virtual void SetLinkedAt(const QDateTime& linkedAt) = 0;

	/**
		Get the timestamp of the last authentication via this identity.
		\return Last authentication timestamp in UTC
	*/
	virtual QDateTime GetLastAuthAt() const = 0;

	/**
		Set the timestamp of the last authentication via this identity.
		\param lastAuthAt Last authentication timestamp in UTC
	*/
	virtual void SetLastAuthAt(const QDateTime& lastAuthAt) = 0;
};


typedef istd::TUniqueInterfacePtr<IExternalIdentity> IExternalIdentityUniquePtr;
typedef istd::TSharedInterfacePtr<IExternalIdentity> IExternalIdentitySharedPtr;


} // namespace imtauth
