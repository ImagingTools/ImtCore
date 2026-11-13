// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/ISerializable.h>


namespace imtauth
{


/**
	Link between an account of an external identity provider (Google, Apple, Facebook, ...)
	and a local user.

	An external account is identified by the provider ID and the subject the provider issued
	for it. The pair is unique: one external account belongs to at most one local user.

	\ingroup Authorization
*/
class IExternalIdentity: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetId() const = 0;
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Object ID of the local user.
	*/
	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
		ID of the provider component that authenticated the account (e.g. "google").
	*/
	virtual QByteArray GetProviderId() const = 0;
	virtual void SetProviderId(const QByteArray& providerId) = 0;

	/**
		Stable account identifier issued by the provider (the 'sub' claim).
	*/
	virtual QString GetSubject() const = 0;
	virtual void SetSubject(const QString& subject) = 0;

	/**
		E-mail address reported by the provider at the last sign-in.
	*/
	virtual QString GetEmail() const = 0;
	virtual void SetEmail(const QString& email) = 0;

	virtual QDateTime GetLinkedAt() const = 0;
	virtual void SetLinkedAt(const QDateTime& linkedAt) = 0;

	virtual QDateTime GetLastSignInAt() const = 0;
	virtual void SetLastSignInAt(const QDateTime& lastSignInAt) = 0;
};


typedef istd::TUniqueInterfacePtr<IExternalIdentity> IExternalIdentityUniquePtr;
typedef istd::TSharedInterfacePtr<IExternalIdentity> IExternalIdentitySharedPtr;


} // namespace imtauth


