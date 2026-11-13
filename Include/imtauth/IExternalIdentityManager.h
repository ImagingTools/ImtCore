// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IExternalIdentity.h>


namespace imtauth
{


/**
	Storage of the links between external provider accounts and local users.

	\ingroup Authorization
*/
class IExternalIdentityManager: virtual public istd::IPolymorphic
{
public:
	typedef QList<IExternalIdentitySharedPtr> Identities;

	/**
		Find the link of the external account, invalid pointer if the account is not linked.
	*/
	virtual IExternalIdentitySharedPtr FindIdentity(const QByteArray& providerId, const QString& subject) const = 0;

	virtual Identities GetUserIdentities(const QByteArray& userId) const = 0;

	/**
		Link the external account to the user.
		\return ID of the new link, empty if the account is already linked or the link could not be stored.
	*/
	virtual QByteArray LinkIdentity(const QByteArray& userId, const QByteArray& providerId, const QString& subject, const QString& email) const = 0;

	/**
		Store a successful sign-in with the linked account and the e-mail the provider reported for it.
	*/
	virtual bool RegisterSignIn(const QByteArray& identityId, const QString& email) const = 0;

	virtual bool RemoveIdentity(const QByteArray& identityId) const = 0;
};


} // namespace imtauth


