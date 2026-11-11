// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	The authorization of a user session for a product, as the authorization server computes it: the user with its final
	permissions for the product and the organization of the session, and that organization.
	A product server asks for it with its server token; the session token is the proof of the session, not an authentication.
*/
class ISessionAuthorizationProvider: virtual public istd::IPolymorphic
{
public:
	enum AuthorizationState
	{
		/// The user and its organization were read completely; an empty permission set is a result too.
		SAS_COMPUTED,
		/// The session, the user or the organization is refused: see RefusalReason.
		SAS_REFUSED,
		/// Something could not be read: nothing is known.
		SAS_UNCHECKED
	};

	enum RefusalReason
	{
		RR_NONE,
		/// The access token expired or its session is closed: the client authenticates again.
		RR_REAUTHENTICATE,
		/// The user is absent or disabled, or the token is no session token of it.
		RR_FORBIDDEN
	};

	struct SessionAuthorization
	{
		AuthorizationState state = SAS_UNCHECKED;
		RefusalReason refusalReason = RR_NONE;
		QString message;
		QByteArray userId;
		QByteArray tenantId;
		/// The user with permissions[productId] final, pointing to no provider; valid only for SAS_COMPUTED.
		imtbase::IObjectCollection::DataPtr userPtr;
		/// The organization of the session; empty without one.
		QByteArray tenantOwnerId;
		QByteArray tenantCreatorId;
		QByteArrayList tenantPermissions;
		/// When the session ends, as it is known now; invalid when the authorization server does not tell it.
		QDateTime sessionExpiresAt;
	};

	/**
		Get the authorization of the session of an access token.
		\param productId					The product of the server asking.
		\param isEstablishedSubscription	An expired access token holds while its session lives; every other use refuses it.
	*/
	virtual SessionAuthorization GetSessionAuthorization(
				const QByteArray& accessToken,
				const QByteArray& productId,
				bool isEstablishedSubscription) const = 0;
};


} // namespace imtauth
