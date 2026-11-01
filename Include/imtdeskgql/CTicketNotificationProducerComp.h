// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtnotify/INotificationSender.h>


namespace imtdeskgql
{


/**
	Notifications of the support tickets for the notification center.
	- A new message of a ticket goes to its reporter and assignees, except the sender.
	- A user added to the assignees is told so; a removed one loses that notification.
	Tickets belong to no tenant, so the notifications are personal: visible in every tenant of the product.

	The previous assignees are known only for the tickets seen since the start of the server. After a restart the
	current assignees count as added, the dedup key keeps a user from being told twice while the notification is kept.
*/
class CTicketNotificationProducerComp:
			public ilog::CLoggerComponentBase,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CTicketNotificationProducerComp);
		I_ASSIGN(m_messageCollectionCompPtr, "MessageCollection", "Chat message collection to observe", true, "MessageCollection");
		I_ASSIGN_TO(m_messageCollectionModelCompPtr, m_messageCollectionCompPtr, true);
		I_ASSIGN(m_ticketCollectionCompPtr, "TicketCollection", "Ticket collection to observe", true, "TicketCollection");
		I_ASSIGN_TO(m_ticketCollectionModelCompPtr, m_ticketCollectionCompPtr, true);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection for the names of the senders", false, "UserCollection");
		I_ASSIGN(m_notificationSenderCompPtr, "NotificationSender", "Sender of the notification center", true, "NotificationSender");
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product whose users get the notifications", true, "");
	I_END_COMPONENT;

	static const inline QByteArray s_ticketMessageTypeId = QByteArrayLiteral("imtdesk.TicketMessage");
	static const inline QByteArray s_ticketAssignedTypeId = QByteArrayLiteral("imtdesk.TicketAssigned");
	static const inline QByteArray s_ticketTypeId = QByteArrayLiteral("SupportTicket");

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	void OnMessageAdded(const QByteArray& messageId);
	void OnTicketChanged(const QByteArray& ticketId);
	void OnTicketRemoved(const QByteArray& ticketId);

	/**
		The ticket of a conversation, selected by its column; empty if there is none or it cannot be read.
	*/
	QByteArray FindTicketIdByConversationId(const QByteArray& conversationId) const;
	QString FindUserName(const QByteArray& userId) const;
	QByteArray GetProductId() const;

	/**
		The user of the request that made the change, if there is one.
	*/
	imtnotify::ObjectLinkInfo GetCurrentActor() const;

private:
	I_REF(imtbase::IObjectCollection, m_messageCollectionCompPtr);
	I_REF(imod::IModel, m_messageCollectionModelCompPtr);
	I_REF(imtbase::IObjectCollection, m_ticketCollectionCompPtr);
	I_REF(imod::IModel, m_ticketCollectionModelCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtnotify::INotificationSender, m_notificationSenderCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);

	QMutex m_assigneesMutex;
	QMap<QByteArray, QByteArrayList> m_knownAssignees;
};


} // namespace imtdeskgql


