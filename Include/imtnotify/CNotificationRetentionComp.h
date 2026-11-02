// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QSet>
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtnotify/INotificationInboxController.h>


namespace imtnotify
{


/**
	Keeps the notifications only as long as they are of use: removes the ones older than the retention period from
	every inbox, periodically, and all of a user removed from the collection of the users; a failed removal of a user is
	retried with the next periodic one.
	The sessions are told the removed notifications by the inbox controller.
*/
class CNotificationRetentionComp:
			public ilog::CLoggerComponentBase,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CNotificationRetentionComp);
		I_ASSIGN(m_inboxControllerCompPtr, "InboxController", "Controller removing the notifications", true, "InboxController");
		I_ASSIGN(m_userCollectionModelCompPtr, "UserCollection", "Collection of the users: the notifications of a removed user are removed", false, "UserCollection");
		I_ASSIGN(m_retentionDaysAttrPtr, "RetentionDays", "Days a notification is kept", true, 90);
		I_ASSIGN(m_checkIntervalHoursAttrPtr, "CheckIntervalHours", "Hours between two removals of the expired notifications", true, 6);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	void RemoveExpired();
	void RemoveUserNotifications(const QByteArray& userId);

private:
	I_REF(INotificationInboxController, m_inboxControllerCompPtr);
	I_REF(imod::IModel, m_userCollectionModelCompPtr);
	I_ATTR(int, m_retentionDaysAttrPtr);
	I_ATTR(int, m_checkIntervalHoursAttrPtr);

	QTimer m_timer;

	// Removed users whose notifications could not be removed yet
	QSet<QByteArray> m_pendingUserIds;
	QMutex m_pendingUserIdsMutex;
};


} // namespace imtnotify


