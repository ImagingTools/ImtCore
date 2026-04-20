// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketMessageNotifierComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtchat/IChatMessage.h>
#include <imtdesk/ISupportTicket.h>
#include <imtdeskgql/TicketPermissions.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtdeskgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CTicketMessageNotifierComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (m_commandIdAttrPtr.IsValid()){
		QByteArray commandId = *m_commandIdAttrPtr;
		if (!commandId.isEmpty() && gqlRequest.GetCommandId() == commandId){
			return true;
		}
	}
	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CTicketMessageNotifierComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_messageCollectionModelCompPtr.IsValid()){
		m_messageCollectionModelCompPtr->AttachObserver(this);
	}
}


void CTicketMessageNotifierComp::OnComponentDestroyed()
{
	if (m_messageCollectionModelCompPtr.IsValid()){
		m_messageCollectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CTicketMessageNotifierComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_requestManagerCompPtr.IsValid() || !m_messageCollectionCompPtr.IsValid()){
		return;
	}

	// We only care about newly inserted messages.
	if (!changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
		return;
	}

	const QByteArray messageId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray();
	if (messageId.isEmpty()){
		return;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_messageCollectionCompPtr->GetObjectData(messageId, dataPtr)){
		return;
	}

	const imtchat::IChatMessage* messagePtr = dynamic_cast<const imtchat::IChatMessage*>(dataPtr.GetPtr());
	if (messagePtr == nullptr){
		return;
	}

	const QByteArray conversationId = messagePtr->GetConversationId();
	if (conversationId.isEmpty()){
		return;
	}

	const QByteArray ticketId = FindTicketIdByConversationId(conversationId);
	if (ticketId.isEmpty()){
		// Message belongs to a conversation that is not associated with a
		// ticket — nothing to notify about for the ticket-scoped subscription.
		return;
	}

	// Resolve the ticket once for the recipient predicate; cache its
	// reporter/assignees here so we don't re-fetch for every subscriber.
	imtbase::IObjectCollection::DataPtr ticketDataPtr;
	if (!m_ticketCollectionCompPtr.IsValid() || !m_ticketCollectionCompPtr->GetObjectData(ticketId, ticketDataPtr)){
		return;
	}

	const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(ticketDataPtr.GetPtr());
	if (ticketPtr == nullptr){
		return;
	}

	const QByteArray reporterId = ticketPtr->GetReporterId();
	const QByteArrayList assigneeIds = ticketPtr->GetAssigneeIds();
	const QByteArray senderId = messagePtr->GetSenderId();

	// Resolve sender display name (best-effort; empty if unavailable).
	QString senderUserName;
	if (m_userCollectionCompPtr.IsValid() && !senderId.isEmpty()){
		imtbase::IObjectCollection::DataPtr userDataPtr;
		if (m_userCollectionCompPtr->GetObjectData(senderId, userDataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
			if (userInfoPtr != nullptr){
				senderUserName = userInfoPtr->GetName();
			}
		}
	}

	// Build the JSON payload.
	QJsonObject payload;
	payload.insert(QStringLiteral("ticketId"), QString::fromUtf8(ticketId));
	payload.insert(QStringLiteral("ticketNumber"), ticketPtr->GetNumber());
	payload.insert(QStringLiteral("ticketTitle"), ticketPtr->GetTitle());
	payload.insert(QStringLiteral("messageId"), QString::fromUtf8(messageId));
	payload.insert(QStringLiteral("conversationId"), QString::fromUtf8(conversationId));
	payload.insert(QStringLiteral("senderId"), QString::fromUtf8(senderId));
	payload.insert(QStringLiteral("senderUserName"), senderUserName);
	payload.insert(QStringLiteral("content"), messagePtr->GetContent());
	payload.insert(QStringLiteral("createdAt"), messagePtr->GetCreatedAt());

	QJsonDocument doc(payload);
	const QByteArray data = doc.toJson(QJsonDocument::Compact);

	const QByteArray commandId = m_commandIdAttrPtr.IsValid() ? *m_commandIdAttrPtr : QByteArray("OnTicketMessageReceived");

	// Publish only to subscribers whose user is reporter / assignee / admin,
	// excluding the sender itself (no need to notify yourself).
	PublishDataFiltered(commandId, data, [&](const imtgql::CGqlRequest& subscriberRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = subscriberRequest.GetRequestContext();
		if (contextPtr == nullptr){
			return false;
		}
		const QByteArray userId = GetCurrentUserId(contextPtr);
		if (!userId.isEmpty() && userId == senderId){
			return false;
		}
		if (IsCurrentUserAdmin(contextPtr)){
			return true;
		}
		if (userId.isEmpty()){
			return false;
		}
		if (userId == reporterId){
			return true;
		}
		return assigneeIds.contains(userId);
	});
}


// private methods

QByteArray CTicketMessageNotifierComp::FindTicketIdByConversationId(const QByteArray& conversationId) const
{
	if (conversationId.isEmpty() || !m_ticketCollectionCompPtr.IsValid()){
		return QByteArray();
	}

	// NOTE: Linear scan over the ticket collection. This is invoked once per
	// inserted chat message, which is typically infrequent. If ticket counts
	// grow large, this should be replaced by a conversationId -> ticketId
	// index maintained by the ticket repository (see the architecture
	// discussion item in the PR description).
	const imtbase::ICollectionInfo::Ids ids = m_ticketCollectionCompPtr->GetElementIds();
	for (const QByteArray& id : std::as_const(ids)){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_ticketCollectionCompPtr->GetObjectData(id, dataPtr)){
			continue;
		}
		const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(dataPtr.GetPtr());
		if (ticketPtr != nullptr && ticketPtr->GetConversationId() == conversationId){
			return id;
		}
	}

	return QByteArray();
}


} // namespace imtdeskgql

