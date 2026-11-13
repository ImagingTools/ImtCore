// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtnotify/NotificationTypes.h>


namespace imtnotify
{


/**
	The only interface a feature needs to notify users.
	A notification is stored for every recipient, so a user who is offline sees it after the next login.
*/
class INotificationSender: virtual public istd::IPolymorphic
{
public:
	struct Audience
	{
		QByteArray productId;
		QByteArray tenantId;
		QByteArrayList userIds;
	};

	struct Message
	{
		/**
			Event type, e.g. "imtdesk.TicketAssigned", declared in the catalog of the types of its module.
		*/
		QByteArray typeId;

		istd::IInformationProvider::InformationCategory severity = istd::IInformationProvider::IC_INFO;

		/**
			Who caused the event. The actor never receives a notification about the own action.
		*/
		ObjectLinkInfo actor;

		/**
			The object the notification is about; the client opens it.
		*/
		ObjectLinkInfo subject;

		/**
			Text a user wrote, e.g. the beginning of a message; it is shown as it is, never translated.
		*/
		QString body;

		/**
			The values the message of the type puts in, by the ids the type declares (ParameterIds).
		*/
		NotificationArguments arguments;

		/**
			An unread notification with the same group key in the same inbox is updated instead of adding a new one.
		*/
		QByteArray groupKey;

		/**
			A second notification with the same key in the same inbox is ignored.
		*/
		QByteArray dedupKey;
	};

	/**
		Store the message in the inbox of every user of the audience and deliver it to the connected sessions.
		\return false if the message is invalid or could not be stored for a recipient.
	*/
	virtual bool Send(const Audience& audience, const Message& message) = 0;

	/**
		Remove the notifications of a group from the inboxes of the product and tenant of the audience, e.g. when an
		invitation is revoked: of its users, or of every user when it names none.
	*/
	virtual bool Withdraw(const Audience& audience, const QByteArray& groupKey) = 0;
};


} // namespace imtnotify


