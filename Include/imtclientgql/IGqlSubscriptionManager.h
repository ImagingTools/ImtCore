// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>
#include <imtclientgql/IGqlSubscriptionClient.h>


namespace imtclientgql
{


/**
	Registry of GraphQL subscriptions and the clients listening to them.

	Clients are held by raw reference and not owned, so UnregisterSubscription() alone
	defines when one may be destroyed.

	Threading: both methods may be called from any thread, and callbacks may arrive on any
	thread, not necessarily the one that registered.
*/
class IGqlSubscriptionManager: virtual public istd::IChangeable
{
public:
	/**
		Registers \a subscriptionClient for \a subscriptionRequest, opening the
		subscription if it is not open already. The client must stay alive until
		UnregisterSubscription() has returned for it.

		One subscription may be shared between clients with an equivalent request, so the
		same ID can be returned to several of them; unregistering one does not affect the
		others.

		\return The subscription ID, or an empty QByteArray if registration failed --
			nothing was registered and no callback will be delivered.
	*/
	virtual QByteArray RegisterSubscription(
			const imtgql::IGqlRequest& subscriptionRequest,
			imtclientgql::IGqlSubscriptionClient& subscriptionClient) = 0;

	/**
		Unregisters \a subscriptionClient from \a subscriptionId.

		Revocation is synchronous: once this returns, no callback to the client for this
		subscription will start and none is still executing -- so the caller may destroy
		the client immediately.

		The one exception is a callback running on the calling thread, i.e. unregistering
		from inside one. That is allowed and must not block, but the callback is the
		caller's own stack frame, so it keeps running and the client cannot be destroyed
		there.

		Honouring this can mean waiting for a callback to return, so callbacks must never
		block indefinitely and no lock a callback acquires may be held across this call.

		\return Whether the client was registered. False is not an error and the guarantee
			above holds either way.
	*/
	virtual bool UnregisterSubscription(
			const QByteArray& subscriptionId,
			const imtclientgql::IGqlSubscriptionClient& subscriptionClient) = 0;
};


} // namespace imtclientgql

