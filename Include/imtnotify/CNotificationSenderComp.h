// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtnotify/INotificationInboxController.h>
#include <imtnotify/INotificationSender.h>


namespace imtnotify
{


/**
	Stores a notification for every recipient through the inbox controller, which tells the sessions.
	A message with a dedup key already sent to an inbox is not stored again there, also after its notification was
	grouped or read; an unread notification of the same group is updated. A message with too long keys is refused,
	the names and the body are cut.
*/
class CNotificationSenderComp:
			public ilog::CLoggerComponentBase,
			virtual public INotificationSender
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationSenderComp);
		I_REGISTER_INTERFACE(INotificationSender);
		I_ASSIGN(m_inboxControllerCompPtr, "InboxController", "Controller storing the notifications", true, "InboxController");
	I_END_COMPONENT;

	// reimplemented (imtnotify::INotificationSender)
	virtual bool Send(const Audience& audience, const Message& message) override;
	virtual bool Withdraw(const QByteArray& productId, const QByteArray& tenantId, const QByteArray& groupKey) override;

private:
	bool SendToInbox(const NotificationScope& scope, const Message& message);
	bool Store(const INotification& notification);

private:
	I_REF(INotificationInboxController, m_inboxControllerCompPtr);
};


} // namespace imtnotify


