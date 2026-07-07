// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtnotify/INotificationService.h>
#include <imtnotify/INotification.h>
#include <imtbase/IObjectCollection.h>


namespace imtnotify
{


/**
	Default implementation of INotificationService backed by a standard
	imtbase::IObjectCollection.

	Notifications are stored as "Notification" objects created through the
	configured factory. The component keeps no producer-specific logic, so it
	remains reusable across every feature of the system.

	\ingroup imtnotify
*/
class CNotificationServiceComp:
		public icomp::CComponentBase,
		virtual public INotificationService
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationServiceComp)
		I_REGISTER_INTERFACE(INotificationService);
		I_ASSIGN(m_notificationCollectionCompPtr, "NotificationCollection", "Collection storing notifications", true, "NotificationCollection");
		I_ASSIGN(m_notificationFactCompPtr, "NotificationFactory", "Factory for creating notification objects", true, "NotificationFactory");
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Collection type id used when inserting notifications", false, "Notification");
	I_END_COMPONENT

	// reimplemented (imtnotify::INotificationService)
	virtual QByteArray RaiseNotification(const NotificationParams& params) override;
	virtual QByteArrayList RaiseNotificationForRecipients(
				const QByteArrayList& recipientIds,
				const NotificationParams& params) override;
	virtual QByteArrayList GetNotifications(const ListFilter& filter) const override;
	virtual int GetTotalCount(const ListFilter& filter) const override;
	virtual int GetUnreadCount(const QByteArray& recipientId) const override;
	virtual bool SetNotificationRead(const QByteArray& notificationId, bool isRead) override;
	virtual int MarkAllRead(const QByteArray& recipientId) override;
	virtual bool DeleteNotification(const QByteArray& notificationId) override;

private:
	// Fetch every notification id matching \a filter (recipient, unread,
	// category, severity), ordered per filter.sortOrder. Pagination
	// (offset / limit) is NOT applied here.
	QByteArrayList GetFilteredSortedIds(const ListFilter& filter) const;

private:
	I_REF(imtbase::IObjectCollection, m_notificationCollectionCompPtr);
	I_FACT(INotification, m_notificationFactCompPtr);
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
};


} // namespace imtnotify
