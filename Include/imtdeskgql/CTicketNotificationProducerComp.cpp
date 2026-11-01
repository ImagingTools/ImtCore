// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketNotificationProducerComp.h>


// Qt includes
#include <QtCore/QMutexLocker>

// ImtCore includes
#include <imtauth/CollectionSelection.h>
#include <imtauth/IUserInfo.h>
#include <imtchat/IChatMessage.h>
#include <imtdesk/ISupportTicket.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtgql/IGqlContext.h>


namespace
{


const int s_messageModelId = 0;
const int s_ticketModelId = 1;
const int s_maxPreviewLength = 200;


imtnotify::ObjectLinkInfo GetTicketLink(const QByteArray& ticketId, const imtdesk::ISupportTicket& ticket)
{
	imtnotify::ObjectLinkInfo retVal;
	retVal.id = ticketId;
	retVal.typeId = imtdeskgql::CTicketNotificationProducerComp::s_ticketTypeId;
	retVal.name = QStringLiteral("#%1 %2").arg(QString::number(ticket.GetNumber()), ticket.GetTitle());

	return retVal;
}


QByteArray GetAssigneeKey(const QByteArray& ticketId, const QByteArray& userId)
{
	return QByteArrayLiteral("ticket:") + ticketId + QByteArrayLiteral(":assignee:") + userId;
}


QByteArray GetMessagesKey(const QByteArray& ticketId)
{
	return QByteArrayLiteral("ticket:") + ticketId + QByteArrayLiteral(":messages");
}


} // namespace


namespace imtdeskgql
{


// reimplemented (icomp::CComponentBase)

void CTicketNotificationProducerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_messageCollectionModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_messageCollectionModelCompPtr.GetPtr(), s_messageModelId);
	}

	if (m_ticketCollectionModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_ticketCollectionModelCompPtr.GetPtr(), s_ticketModelId);
	}
}


void CTicketNotificationProducerComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CTicketNotificationProducerComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_notificationSenderCompPtr.IsValid()){
		return;
	}

	if (modelId == s_messageModelId){
		if (changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
			OnMessageAdded(changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray());
		}
	}
	else if (modelId == s_ticketModelId){
		if (changeSet.Contains(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED)){
			OnTicketChanged(changeSet.GetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED).toByteArray());
		}
		else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
			OnTicketChanged(changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray());
		}
		else if (changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
			const imtbase::ICollectionInfo::ElementsRemoveInfo removeInfo =
						changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED).value<imtbase::ICollectionInfo::ElementsRemoveInfo>();
			for (const QByteArray& ticketId : removeInfo.elementIds){
				OnTicketRemoved(ticketId);
			}
		}
	}
}


// private methods

void CTicketNotificationProducerComp::OnMessageAdded(const QByteArray& messageId)
{
	if (messageId.isEmpty() || !m_messageCollectionCompPtr.IsValid() || !m_ticketCollectionCompPtr.IsValid()){
		return;
	}

	imtbase::IObjectCollection::DataPtr messageDataPtr;
	if (!m_messageCollectionCompPtr->GetObjectData(messageId, messageDataPtr)){
		SendWarningMessage(0, QStringLiteral("Message '%1' could not be read, nobody is notified").arg(QString::fromUtf8(messageId)), "CTicketNotificationProducerComp");

		return;
	}

	const imtchat::IChatMessage* messagePtr = dynamic_cast<const imtchat::IChatMessage*>(messageDataPtr.GetPtr());
	if (messagePtr == nullptr){
		return;
	}

	// A conversation without a ticket is no ticket message
	const QByteArray ticketId = FindTicketIdByConversationId(messagePtr->GetConversationId());
	if (ticketId.isEmpty()){
		return;
	}

	imtbase::IObjectCollection::DataPtr ticketDataPtr;
	if (!m_ticketCollectionCompPtr->GetObjectData(ticketId, ticketDataPtr)){
		return;
	}

	const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(ticketDataPtr.GetPtr());
	if (ticketPtr == nullptr){
		return;
	}

	imtnotify::INotificationSender::Audience audience;
	audience.productId = GetProductId();
	audience.userIds = ticketPtr->GetAssigneeIds();
	audience.userIds.prepend(ticketPtr->GetReporterId());

	const QByteArray senderId = messagePtr->GetSenderId();

	QString preview = messagePtr->GetContent().simplified();
	if (preview.length() > s_maxPreviewLength){
		preview = preview.left(s_maxPreviewLength) + QStringLiteral("…");
	}

	imtnotify::INotificationSender::Message message;
	message.typeId = s_ticketMessageTypeId;
	message.actor.id = senderId;
	message.actor.typeId = QByteArrayLiteral("User");
	message.actor.name = FindUserName(senderId);
	message.subject = GetTicketLink(ticketId, *ticketPtr);
	message.body = preview;
	message.groupKey = GetMessagesKey(ticketId);
	message.dedupKey = QByteArrayLiteral("message:") + messageId;

	m_notificationSenderCompPtr->Send(audience, message);
}


void CTicketNotificationProducerComp::OnTicketChanged(const QByteArray& ticketId)
{
	if (ticketId.isEmpty() || !m_ticketCollectionCompPtr.IsValid()){
		return;
	}

	imtbase::IObjectCollection::DataPtr ticketDataPtr;
	if (!m_ticketCollectionCompPtr->GetObjectData(ticketId, ticketDataPtr)){
		return;
	}

	const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(ticketDataPtr.GetPtr());
	if (ticketPtr == nullptr){
		return;
	}

	const QByteArrayList currentAssignees = ticketPtr->GetAssigneeIds();

	QByteArrayList addedAssignees;
	QByteArrayList removedAssignees;
	{
		QMutexLocker locker(&m_assigneesMutex);

		const bool isKnown = m_knownAssignees.contains(ticketId);
		const QByteArrayList previousAssignees = m_knownAssignees.value(ticketId);
		m_knownAssignees.insert(ticketId, currentAssignees);

		for (const QByteArray& assigneeId : currentAssignees){
			if (!isKnown || !previousAssignees.contains(assigneeId)){
				addedAssignees.append(assigneeId);
			}
		}

		for (const QByteArray& assigneeId : previousAssignees){
			if (!currentAssignees.contains(assigneeId)){
				removedAssignees.append(assigneeId);
			}
		}
	}

	const QByteArray productId = GetProductId();

	for (const QByteArray& assigneeId : std::as_const(removedAssignees)){
		m_notificationSenderCompPtr->Withdraw(productId, QByteArray(), GetAssigneeKey(ticketId, assigneeId));
	}

	if (addedAssignees.isEmpty()){
		return;
	}

	imtnotify::INotificationSender::Message message;
	message.typeId = s_ticketAssignedTypeId;
	message.actor = GetCurrentActor();
	message.subject = GetTicketLink(ticketId, *ticketPtr);

	// One notification per assignee: each one is withdrawn on its own when the assignee is removed
	for (const QByteArray& assigneeId : std::as_const(addedAssignees)){
		imtnotify::INotificationSender::Audience audience;
		audience.productId = productId;
		audience.userIds = {assigneeId};

		message.groupKey = GetAssigneeKey(ticketId, assigneeId);
		message.dedupKey = message.groupKey;

		m_notificationSenderCompPtr->Send(audience, message);
	}
}


void CTicketNotificationProducerComp::OnTicketRemoved(const QByteArray& ticketId)
{
	if (ticketId.isEmpty()){
		return;
	}

	QByteArrayList assigneeIds;
	{
		QMutexLocker locker(&m_assigneesMutex);

		assigneeIds = m_knownAssignees.take(ticketId);
	}

	// The notifications of a removed ticket lead nowhere
	const QByteArray productId = GetProductId();
	for (const QByteArray& assigneeId : std::as_const(assigneeIds)){
		m_notificationSenderCompPtr->Withdraw(productId, QByteArray(), GetAssigneeKey(ticketId, assigneeId));
	}

	m_notificationSenderCompPtr->Withdraw(productId, QByteArray(), GetMessagesKey(ticketId));
}


QByteArray CTicketNotificationProducerComp::FindTicketIdByConversationId(const QByteArray& conversationId) const
{
	if (conversationId.isEmpty()){
		return QByteArray();
	}

	bool isRead = true;
	const QByteArrayList candidateIds = imtauth::CollectionSelection::SelectIds(
				*m_ticketCollectionCompPtr,
				imtauth::CollectionSelection::ColumnEquals(QByteArrayLiteral("ConversationId"), conversationId),
				QByteArrayList(),
				&isRead);
	if (!isRead){
		SendWarningMessage(0, QStringLiteral("The tickets could not be selected by the conversation '%1'").arg(QString::fromUtf8(conversationId)), "CTicketNotificationProducerComp");

		return QByteArray();
	}

	// A collection that does not know the filter returns every ticket
	for (const QByteArray& candidateId : candidateIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_ticketCollectionCompPtr->GetObjectData(candidateId, dataPtr)){
			continue;
		}

		const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(dataPtr.GetPtr());
		if (ticketPtr != nullptr && ticketPtr->GetConversationId() == conversationId){
			return candidateId;
		}
	}

	return QByteArray();
}


QString CTicketNotificationProducerComp::FindUserName(const QByteArray& userId) const
{
	if (userId.isEmpty() || !m_userCollectionCompPtr.IsValid()){
		return QString();
	}

	imtbase::IObjectCollection::DataPtr userDataPtr;
	if (!m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)){
		return QString();
	}

	const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());

	return userInfoPtr != nullptr ? userInfoPtr->GetName() : QString();
}


QByteArray CTicketNotificationProducerComp::GetProductId() const
{
	return m_productIdAttrPtr.IsValid() ? *m_productIdAttrPtr : QByteArray();
}


imtnotify::ObjectLinkInfo CTicketNotificationProducerComp::GetCurrentActor() const
{
	imtnotify::ObjectLinkInfo retVal;

	const imtgql::IGqlContext* contextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (contextPtr != nullptr && !contextPtr->GetUserId().isEmpty()){
		retVal.id = contextPtr->GetUserId();
		retVal.typeId = QByteArrayLiteral("User");

		const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
		retVal.name = userInfoPtr != nullptr ? userInfoPtr->GetName() : QString();
	}

	return retVal;
}


} // namespace imtdeskgql


