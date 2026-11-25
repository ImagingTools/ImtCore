// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	The access a user gets to an organization it is no member of: the contracts of one of its organizations give
	permissions, and that organization gives them to groups the user is in.
	\ingroup Permission
*/
class IDelegatedAccess: virtual public istd::IPolymorphic
{
public:
	/**
		The permissions of the product the user gets in the target organization.
		\param isReadPtr [optional] False when the contracts, the memberships or the groups could not be read.
	*/
	virtual QByteArrayList GetDelegatedPermissions(
				const QByteArray& userId,
				const QByteArray& targetTenantId,
				const QByteArray& productId,
				bool* isReadPtr = nullptr) const = 0;

	/**
		The organizations the user gets any permission in.
		\param isReadPtr [optional] False when the contracts, the memberships or the groups could not be read.
	*/
	virtual QByteArrayList GetDelegatingTenantIds(const QByteArray& userId, bool* isReadPtr = nullptr) const = 0;

	/**
		The user gets any permission in the target organization.
	*/
	virtual bool HasDelegatedAccess(const QByteArray& userId, const QByteArray& targetTenantId, bool* isReadPtr = nullptr) const = 0;
};


} // namespace imtauth


