// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <iser/ISerializable.h>


namespace imtchat
{


/**
	Interface representing a single chat message.

	Messages belong to a conversation, are sent by a user, and can optionally
	embed entity references and attachment IDs.

	\ingroup imtchat
*/
class IChatMessage: virtual public iser::ISerializable
{
public:
	/**
		Message delivery/read status.
	*/
	enum MessageStatus
	{
		MS_SENT,
		MS_DELIVERED,
		MS_READ
	};
	I_DECLARE_ENUM(MessageStatus,
				MS_SENT,
				MS_DELIVERED,
				MS_READ);

	/**
		Get the unique message identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique message identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the conversation this message belongs to.
	*/
	virtual QByteArray GetConversationId() const = 0;

	/**
		Set the conversation identifier.
	*/
	virtual void SetConversationId(const QByteArray& conversationId) = 0;

	/**
		Get the identifier of the user who sent this message.
	*/
	virtual QByteArray GetSenderId() const = 0;

	/**
		Set the sender identifier.
	*/
	virtual void SetSenderId(const QByteArray& senderId) = 0;

	/**
		Get the message text content.
	*/
	virtual QString GetContent() const = 0;

	/**
		Set the message text content.
	*/
	virtual void SetContent(const QString& content) = 0;

	/**
		Get the delivery/read status.
	*/
	virtual MessageStatus GetStatus() const = 0;

	/**
		Set the delivery/read status.
	*/
	virtual void SetStatus(MessageStatus status) = 0;

	/**
		Get the ISO 8601 creation timestamp.
	*/
	virtual QString GetCreatedAt() const = 0;

	/**
		Set the creation timestamp.
	*/
	virtual void SetCreatedAt(const QString& createdAt) = 0;

	/**
		Get the ISO 8601 last-update timestamp.
	*/
	virtual QString GetUpdatedAt() const = 0;

	/**
		Set the last-update timestamp.
	*/
	virtual void SetUpdatedAt(const QString& updatedAt) = 0;

	/**
		Get the list of entity reference IDs linked to this message.
	*/
	virtual QByteArrayList GetEntityReferences() const = 0;

	/**
		Set the entity reference IDs.
	*/
	virtual void SetEntityReferences(const QByteArrayList& entityReferences) = 0;

	/**
		Get the list of attachment IDs associated with this message.
	*/
	virtual QByteArrayList GetAttachmentIds() const = 0;

	/**
		Set the attachment IDs.
	*/
	virtual void SetAttachmentIds(const QByteArrayList& attachmentIds) = 0;
};

typedef istd::TUniqueInterfacePtr<IChatMessage> IChatMessageUniquePtr;

} // namespace imtchat
