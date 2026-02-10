// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtclientgql
{


/**
	Interface for the subscription client.
*/
class IGqlSubscriptionClient: virtual public istd::IChangeable
{
public:
	enum SubscriptionStatus
	{
		SS_UNKNOWN = 0,
		SS_UNREGISTERED,
		SS_IN_REGISTRATION,
		SS_REGISTERED
	};

	/**
		Handling the incomming data for a given subscription
		\param subscriptionId	ID of the subscription the client is listening
		\param subscriptionData	Related data
	*/
	virtual void OnResponseReceived(
				const QByteArray& subscriptionId,
				const QByteArray& subscriptionData) = 0;

	/**
		Handling the incomming notification about the change of the subscription status.
		\param subscriptionId	ID of the subscription the client is listening
		\param status	New subscription status
		\param message	
	*/
	virtual void OnSubscriptionStatusChanged(
				const QByteArray& subscriptionId,
				const SubscriptionStatus& status,
				const QString& message) = 0;
};


} // namespace imtclientgql


