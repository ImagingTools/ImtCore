// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtrest/IResponse.h>


namespace imtrest
{


/**
	Interface for handling of messages received on an active subscription.
*/
class ISubscriptionMessageHandler: virtual public istd::IPolymorphic
{
public:
	/**
		Process a message received for the given subscription.
		\param subscriptionId ID of the subscription the message was received on.
		\param methodType type of the method/message being processed.
		\param message raw message data to be processed.
		\return response to be sent back to the client, if any.
	*/
	virtual imtrest::ConstResponsePtr ProcessMessage(const QByteArray& subscriptionId, const QByteArray& methodType, const QByteArray& message) const = 0;
};


} // namespace imtrest
