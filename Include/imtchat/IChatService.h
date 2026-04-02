// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtchat/IConversation.h>


namespace imtchat
{


/**
	Service interface for chat conversation management.

	Provides operations for creating and retrieving conversations.

	\ingroup imtchat
*/
class IChatService: virtual public istd::IPolymorphic
{
public:
	/**
		Create a new conversation.
		\param name              Display name.
		\param conversationType  Type (0 = Direct, 1 = Group, 2 = Channel, 3 = Support).
		\param participantIds    Initial participant list.
		\return New conversation ID, or empty on failure.
	*/
	virtual QByteArray CreateConversation(
				const QString& name,
				int conversationType,
				const QByteArrayList& participantIds) = 0;

	/**
		Retrieve conversation IDs accessible to the current user.
		\param offset  Pagination offset.
		\param limit   Maximum number of conversations (-1 = all).
		\return List of conversation IDs.
	*/
	virtual QByteArrayList GetConversations(int offset = 0, int limit = -1) const = 0;
};

} // namespace imtchat
