// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtchat/IConversation.h>
#include <imtchat/IChatMessage.h>


#undef SendMessage


namespace imtchat
{


/**
	Service interface for chat operations.

	Provides operations for sending and retrieving messages, managing
	conversations, and marking messages as read.

	\ingroup imtchat
*/
class IChatService: virtual public istd::IPolymorphic
{
public:
	/**
		Send a message to a conversation.
		\param conversationId    Target conversation ID.
		\param senderId          User ID of the message sender.
		\param content           Message text content.
		\param entityReferences  Optional entity reference IDs.
		\param attachmentIds     Optional attachment IDs.
		\param replyToId         Optional ID of the message being replied to.
		\return New message ID, or empty on failure.
	*/
	virtual QByteArray SendMessage(
				const QByteArray& conversationId,
				const QByteArray& senderId,
				const QString& content,
				const QByteArrayList& entityReferences = QByteArrayList(),
				const QByteArrayList& attachmentIds = QByteArrayList(),
				const QByteArray& replyToId = QByteArray()) = 0;

	/**
		Retrieve messages for a conversation.
		\param conversationId  Conversation to query.
		\param offset          Pagination offset.
		\param limit           Maximum number of messages to return (-1 = all).
		\return List of message IDs in chronological order.
	*/
	virtual QByteArrayList GetMessages(
				const QByteArray& conversationId,
				int offset = 0,
				int limit = 50) const = 0;

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

	/**
		Mark a message as read for the current user.
		\param conversationId  The conversation containing the message.
		\param messageId       The message to mark as read.
		\return True on success.
	*/
	virtual bool MarkMessageRead(
				const QByteArray& conversationId,
				const QByteArray& messageId) = 0;

	/**
		Edit the text content of an existing message. The message's
		\c updatedAt timestamp is refreshed so clients can detect that
		the message was edited.
		\param messageId  The message to edit.
		\param senderId   The user requesting the edit; must match the
		                  original sender.
		\param newContent The new text content.
		\return True on success.
	*/
	virtual bool EditMessage(
				const QByteArray& messageId,
				const QByteArray& senderId,
				const QString& newContent) = 0;

	/**
		Permanently delete a message from its conversation.
		\param messageId  The message to delete.
		\param senderId   The user requesting the deletion; must match the
		                  original sender.
		\return True on success.
	*/
	virtual bool DeleteMessage(
				const QByteArray& messageId,
				const QByteArray& senderId) = 0;
};

} // namespace imtchat
