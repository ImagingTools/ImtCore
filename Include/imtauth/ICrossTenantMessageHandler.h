// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/ICrossTenantMessage.h>


namespace imtauth
{


/**
	Domain handler of the cross-tenant messaging pipeline (RFC phase 2 §14,
	phase 3 §15).

	Each domain (orders, document sharing, reports, ...) provides its own handler.
	A handler understands the structure of a concrete message type, materializes a
	new local object inside the target tenant from the projected payload and links
	it back to the message. The broker keeps no business knowledge; it only
	dispatches messages to the handler that declares it can process them.

	\ingroup Tenant
*/
class ICrossTenantMessageHandler: virtual public istd::IPolymorphic
{
public:
	/**
		Query whether this handler can process a given message type.
		\param messageType The domain type of the message.
		\param customType Concrete type name when \a messageType is CTMT_CUSTOM.
	*/
	virtual bool CanHandle(CrossTenantMessageType messageType, const QString& customType) const = 0;

	/**
		Process a delivered message in the target tenant.

		The handler creates the local target object from the (already projected)
		payload of \a messageInfo and returns its ID through \a targetObjectId.
		Returning false marks the message as failed.

		\param messageInfo The message to process.
		\param targetObjectId Output ID of the local object created by the handler.
		\param errorMessage Output error description when processing fails.
		\return true if the message was processed successfully.
	*/
	virtual bool HandleMessage(
				const CrossTenantMessageInfo& messageInfo,
				QByteArray& targetObjectId,
				QString& errorMessage) = 0;
};


} // namespace imtauth
