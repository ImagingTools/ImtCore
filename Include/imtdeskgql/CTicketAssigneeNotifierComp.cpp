// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketAssigneeNotifierComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtdesk/ISupportTicket.h>
#include <imtdeskgql/TicketPermissions.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtdeskgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CTicketAssigneeNotifierComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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

void CTicketAssigneeNotifierComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// Build the initial assignee cache from all existing tickets.
	if (m_ticketCollectionCompPtr.IsValid()){
		const imtbase::ICollectionInfo::Ids ids = m_ticketCollectionCompPtr->GetElementIds();
		for (const QByteArray& id : std::as_const(ids)){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_ticketCollectionCompPtr->GetObjectData(id, dataPtr)){
				const imtdesk::ISupportTicket* ticketPtr =
						dynamic_cast<const imtdesk::ISupportTicket*>(dataPtr.GetPtr());
				if (ticketPtr != nullptr){
					m_cachedAssignees.insert(id, ticketPtr->GetAssigneeIds());
				}
			}
		}
	}

	if (m_ticketCollectionModelCompPtr.IsValid()){
		m_ticketCollectionModelCompPtr->AttachObserver(this);
	}
}


void CTicketAssigneeNotifierComp::OnComponentDestroyed()
{
	if (m_ticketCollectionModelCompPtr.IsValid()){
		m_ticketCollectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CTicketAssigneeNotifierComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_requestManagerCompPtr.IsValid() || !m_ticketCollectionCompPtr.IsValid()){
		return;
	}

	QByteArray ticketId;

	if (changeSet.Contains(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED)){
		ticketId = changeSet.GetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED).toByteArray();
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
		ticketId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray();
	}

	if (ticketId.isEmpty()){
		return;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_ticketCollectionCompPtr->GetObjectData(ticketId, dataPtr)){
		return;
	}

	const imtdesk::ISupportTicket* ticketPtr =
			dynamic_cast<const imtdesk::ISupportTicket*>(dataPtr.GetPtr());
	if (ticketPtr == nullptr){
		return;
	}

	const QByteArrayList currentAssignees = ticketPtr->GetAssigneeIds();
	const QByteArrayList previousAssignees = m_cachedAssignees.value(ticketId);

	// Update cache immediately.
	m_cachedAssignees.insert(ticketId, currentAssignees);

	// Compute newly added assignees.
	QByteArrayList addedAssignees;
	for (const QByteArray& assigneeId : std::as_const(currentAssignees)){
		if (!previousAssignees.contains(assigneeId)){
			addedAssignees << assigneeId;
		}
	}

	if (addedAssignees.isEmpty()){
		return;
	}

	// Build the JSON payload.
	QJsonObject payload;
	payload.insert(QStringLiteral("ticketId"), QString::fromUtf8(ticketId));
	payload.insert(QStringLiteral("ticketNumber"), ticketPtr->GetNumber());
	payload.insert(QStringLiteral("ticketTitle"), ticketPtr->GetTitle());

	QJsonDocument doc(payload);
	const QByteArray data = doc.toJson(QJsonDocument::Compact);

	const QByteArray commandId = m_commandIdAttrPtr.IsValid()
			? *m_commandIdAttrPtr
			: QByteArrayLiteral("OnTicketAssigneeChanged");

	// Publish only to the newly added assignees — they are the ones who
	// need a notification. The user who made the change is excluded.
	PublishDataFiltered(commandId, data, [&](const imtgql::CGqlRequest& subscriberRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = subscriberRequest.GetRequestContext();
		if (contextPtr == nullptr){
			return false;
		}
		const QByteArray userId = contextPtr->GetUserId();
		if (userId.isEmpty()){
			return false;
		}
		return addedAssignees.contains(userId);
	});
}


} // namespace imtdeskgql
