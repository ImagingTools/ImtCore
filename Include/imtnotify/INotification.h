// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtnotify/NotificationTypes.h>


namespace imtnotify
{


/**
	Notification in the inbox of a user. It keeps what happened, never a text in a language: a client shows the name of
	the subject and the label of the type, which the server of the client gives in the catalog of the types.
	The message part is the one of istd::IInformationProvider: the category is the severity, the description is the
	text a user wrote (e.g. the beginning of a message, never translated), the source is the name of the user who caused
	the event, the time stamp is the time of the latest event.
	The id of a notification is its id in the collection of the notifications.
*/
class INotification: virtual public istd::IInformationProvider
{
public:
	virtual void SetCategory(InformationCategory category) = 0;
	virtual void SetDescription(const QString& description) = 0;
	virtual void SetTimeStamp(const QDateTime& timeStamp) = 0;

	/**
		The inbox the notification belongs to.
	*/
	virtual const NotificationScope& GetScope() const = 0;
	virtual void SetScope(const NotificationScope& scope) = 0;

	/**
		Event type, e.g. "imtdesk.TicketAssigned"; the catalog of the types gives its label and icon.
	*/
	virtual const QByteArray& GetTypeId() const = 0;
	virtual void SetTypeId(const QByteArray& typeId) = 0;

	/**
		The object the notification is about; the client opens it.
	*/
	virtual const ObjectLinkInfo& GetSubject() const = 0;
	virtual void SetSubject(const ObjectLinkInfo& subject) = 0;

	/**
		Who caused the event: the actors of the latest events of a group, the newest first, at most s_maxActorCount.
		The source of the message is the name of the newest one.
	*/
	virtual const ObjectLinkInfos& GetActors() const = 0;
	virtual void SetActors(const ObjectLinkInfos& actors) = 0;

	/**
		The values the message of the type puts in; a group keeps the ones of its latest event.
	*/
	virtual const NotificationArguments& GetArguments() const = 0;
	virtual void SetArguments(const NotificationArguments& arguments) = 0;

	/**
		An unread notification with the same group key in the same inbox is updated instead of adding a new one.
	*/
	virtual const QByteArray& GetGroupKey() const = 0;
	virtual void SetGroupKey(const QByteArray& groupKey) = 0;

	/**
		Number of events merged into this notification by its group key.
	*/
	virtual int GetGroupCount() const = 0;
	virtual void SetGroupCount(int groupCount) = 0;

	virtual bool IsDelivered() const = 0;
	virtual void SetDelivered(bool isDelivered) = 0;

	virtual bool IsRead() const = 0;
	virtual void SetRead(bool isRead) = 0;
};


} // namespace imtnotify


