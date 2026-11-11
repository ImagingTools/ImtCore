// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtrest/IRequest.h>


namespace imtgql
{


class IGqlSubscriberController: virtual public istd::IPolymorphic
{
public:
	/**
		Who subscribes.
	*/
	enum SubscriberKind
	{
		SK_ANONYMOUS,
		SK_USER,
		/**
			A signed-in recipient whatever its permissions: the channel delivers to a subscriber only what is addressed to its own
			identity (principal, user, organization, product), so a change of its permissions does not end its subscription.
			Declared for the command of a request, never implied by SK_USER, by an administrator or by an undeclared open channel.
		*/
		SK_RECIPIENT
	};

	/**
		Get IDs of the supported commands.
	*/
	virtual bool IsRequestSupported(const CGqlRequest& gqlRequest) const = 0;

	/**
		Tell whether this channel is declared open for such a subscriber. A channel is closed unless it says so.
		imtservergql::CGqlPublisherCompBase opens its channels to signed-in users only: its publishers filter the events by the subscriber.
	*/
	virtual bool IsOpenFor(SubscriberKind subscriberKind, const CGqlRequest& gqlRequest) const = 0;

	/**
		Register subscriprion for a GraphQL-based request.
	*/
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) = 0;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) = 0;
};


} // namespace imtgql


