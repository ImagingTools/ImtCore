// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtnotify/INotification.h>


namespace imtnotify
{


/**
	Producer-facing service interface for the notification (Activity) subsystem.

	This is the only type a feature needs to depend on in order to raise a
	notification. It hides how notifications are stored, delivered and rendered,
	so new producers (tickets, chat, tenants, licensing, ...) integrate without
	any change to imtnotify (Open-Closed) and without imtnotify knowing anything
	about them (Single Responsibility, Dependency Inversion).

	\ingroup imtnotify
*/
class INotificationService: virtual public istd::IPolymorphic
{
public:
	/**
		Parameters describing a single notification to raise. Grouped in a
		struct so producers pass named fields and the interface stays stable
		as optional fields are added.
	*/
	struct NotificationParams
	{
		QByteArray recipientId;                         //!< Target user (required).
		QString category;                               //!< Producer category, e.g. "Ticket".
		QString title;                                  //!< Short title (required).
		QString body;                                   //!< Long description.
		QString iconName;                               //!< ImtCore icon path.
		INotification::Severity severity = INotification::SV_INFO;
		QString sourceType;                             //!< Producing entity type.
		QByteArray sourceId;                            //!< Producing entity id.
		QString targetRoute;                            //!< Client deep-link route.
		QString metadata;                               //!< Free-form JSON metadata.
	};

	/**
		Raise (persist) a single notification.
		\param params  The notification description.
		\return The new notification id, or an empty array on failure.
	*/
	virtual QByteArray RaiseNotification(const NotificationParams& params) = 0;

	/**
		Raise the same notification for several recipients at once. The default
		implementation calls RaiseNotification once per recipient; overriders may
		batch. \c params.recipientId is ignored.
		\return The list of created notification ids (one per successful insert).
	*/
	virtual QByteArrayList RaiseNotificationForRecipients(
				const QByteArrayList& recipientIds,
				const NotificationParams& params) = 0;

	/**
		Ordering applied to the notification feed.
	*/
	enum SortOrder
	{
		SO_NEWEST_FIRST,
		SO_OLDEST_FIRST
	};

	/**
		Server-side filter / sort / pagination specification for a feed query.
		All filtering, sorting and pagination happen on the server so the client
		only ever receives the exact page it needs to render.
	*/
	struct ListFilter
	{
		QByteArray recipientId;                         //!< Target user; empty = all recipients (admin).
		bool unreadOnly = false;                        //!< Only unread notifications.
		QString category;                               //!< Producer category; empty = any.
		int severity = -1;                              //!< INotification::Severity value; -1 = any.
		SortOrder sortOrder = SO_NEWEST_FIRST;          //!< Ordering by creation time.
		int offset = 0;                                 //!< Pagination offset.
		int limit = -1;                                 //!< Page size; -1 = all remaining.
	};

	/**
		Retrieve the ids of notifications matching \a filter, ordered and paged
		as requested.
	*/
	virtual QByteArrayList GetNotifications(const ListFilter& filter) const = 0;

	/**
		Count all notifications matching \a filter, ignoring its offset / limit.
		Used to drive pagination ("load more" availability).
	*/
	virtual int GetTotalCount(const ListFilter& filter) const = 0;

	/**
		Count unread notifications addressed to a recipient.
		\param recipientId  Target user; empty means all recipients.
	*/
	virtual int GetUnreadCount(const QByteArray& recipientId) const = 0;

	/**
		Mark a single notification read / unread.
		\return True on success.
	*/
	virtual bool SetNotificationRead(const QByteArray& notificationId, bool isRead) = 0;

	/**
		Mark every notification of a recipient as read.
		\param recipientId  Target user; empty means all recipients.
		\return The number of notifications whose state changed.
	*/
	virtual int MarkAllRead(const QByteArray& recipientId) = 0;

	/**
		Permanently delete a notification.
		\return True on success.
	*/
	virtual bool DeleteNotification(const QByteArray& notificationId) = 0;
};

} // namespace imtnotify
