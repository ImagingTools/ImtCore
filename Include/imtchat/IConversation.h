// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <iser/ISerializable.h>


namespace imtchat
{


/**
	Interface representing a chat conversation.

	A conversation is a container for messages between participants. It can be
	a direct (1:1) chat, group, channel, or support conversation.

	\ingroup imtchat
*/
class IConversation: virtual public iser::ISerializable
{
public:
	/**
		Conversation type — describes the kind of conversation.
	*/
	enum ConversationType
	{
		CT_DIRECT,
		CT_GROUP,
		CT_CHANNEL,
		CT_SUPPORT
	};
	I_DECLARE_ENUM(ConversationType,
				CT_DIRECT,
				CT_GROUP,
				CT_CHANNEL,
				CT_SUPPORT);

	/**
		Get the unique conversation identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique conversation identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the display name of the conversation.
	*/
	virtual QString GetName() const = 0;

	/**
		Set the display name of the conversation.
	*/
	virtual void SetName(const QString& name) = 0;

	/**
		Get the conversation type.
	*/
	virtual ConversationType GetConversationType() const = 0;

	/**
		Set the conversation type.
	*/
	virtual void SetConversationType(ConversationType conversationType) = 0;

	/**
		Get the list of participant user IDs.
	*/
	virtual QByteArrayList GetParticipantIds() const = 0;

	/**
		Set the list of participant user IDs.
	*/
	virtual void SetParticipantIds(const QByteArrayList& participantIds) = 0;

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
		Get the JSON metadata string for this conversation.
	*/
	virtual QString GetMetadata() const = 0;

	/**
		Set the JSON metadata string.
	*/
	virtual void SetMetadata(const QString& metadata) = 0;
};

typedef istd::TUniqueInterfacePtr<IConversation> IConversationUniquePtr;

} // namespace imtchat
