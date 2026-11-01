// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtnotify/INotification.h>


namespace imtnotify
{


/**
	Every change of the notifications; the object collection of the notifications only reads them.
	An inbox is the one of a user in a tenant and product, with the personal notifications of the user;
	any id of another inbox is ignored. A method returns false if the notifications could not be read or written.
	Every change is told as a NotificationChange, in the inbox of the changed notification (a personal one in its own),
	so the controller is the only source of the changes the sessions get.
*/
class INotificationInboxController: virtual public istd::IChangeable
{
public:
	/**
		Store the event of a notification in its inbox: add it to the unread notification of its group, or add a new one.
		With a dedup key, an event whose key the inbox already has is not stored; the key and the notification are stored
		in one transaction, so a stop between them neither loses the event nor stores it twice. A key is kept for the
		retention period, also after its notification was grouped, read or removed.
		\param isStored	false if the inbox already had the key.
	*/
	virtual bool Store(const INotification& notification, const QByteArray& dedupKey, bool& isStored) = 0;

	virtual bool MarkRead(const NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& changedIds) = 0;

	/**
		Mark every unread notification created until the given time as read. An invalid time marks all of them.
	*/
	virtual bool MarkAllRead(const NotificationScope& scope, const QDateTime& createdUntil, QByteArrayList& changedIds) = 0;

	virtual bool MarkDelivered(const NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& changedIds) = 0;

	/**
		Mark every undelivered notification created until the given time as delivered, e.g. after a summary at the login.
	*/
	virtual bool MarkAllDelivered(const NotificationScope& scope, const QDateTime& createdUntil, QByteArrayList& changedIds) = 0;

	virtual bool Remove(const NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& removedIds) = 0;

	/**
		Remove the notifications of a group from every inbox of the tenant and product. Its key can be sent again:
		the dedup keys equal to the group key are forgotten too.
	*/
	virtual bool RemoveGroup(const QByteArray& productId, const QByteArray& tenantId, const QByteArray& groupKey) = 0;

	/**
		Remove the notifications of a user in a tenant, in every product; an empty tenant removes all of them.
	*/
	virtual bool RemoveUserNotifications(const QByteArray& tenantId, const QByteArray& userId) = 0;

	/**
		Remove at most maxCount notifications and dedup keys created before the given time, in every inbox.
		\param isComplete	false if more of them are left.
	*/
	virtual bool RemoveExpired(const QDateTime& createdBefore, int maxCount, bool& isComplete) = 0;
};


} // namespace imtnotify


