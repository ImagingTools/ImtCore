// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CEntityContextTicketsControllerComp.h>

// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QtGlobal>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtdeskgql/imtdeskgql.h>
#include <imtdeskgql/TicketPermissions.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtgql/IGqlContext.h>


namespace
{


bool TicketHasEntityContext(
			const imtdesk::ISupportTicket* ticketPtr,
			const imtdesk::IEntityReferenceStorage* entityReferenceStoragePtr,
			const QString& entityType,
			const QByteArray& entityId)
{
	if (ticketPtr == nullptr || entityReferenceStoragePtr == nullptr){
		return false;
	}

	const QByteArrayList entityReferenceIds = ticketPtr->GetEntityReferences();
	for (const QByteArray& refId : entityReferenceIds){
		QString refType;
		QByteArray refEntityId;
		QString displayName;
		QString entityUrl;
		if (!entityReferenceStoragePtr->GetEntityReference(refId, refType, refEntityId, displayName, entityUrl)){
			continue;
		}

		if (refType == entityType && refEntityId == entityId){
			return true;
		}
	}

	return false;
}


} // namespace


namespace imtdeskgql
{


sdl::imtdesk::ImtDesk::CEntityContextTicketsPayload CEntityContextTicketsControllerComp::OnEntityContextTickets(
			const sdl::imtdesk::ImtDesk::CEntityContextTicketsGqlRequest& entityContextTicketsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtdesk::ImtDesk::CEntityContextTicketsPayload response;

	if (!m_ticketCollectionCompPtr.IsValid() || !m_entityReferenceStorageCompPtr.IsValid()){
		errorMessage = QStringLiteral("Ticket collection is not configured");
		return response;
	}

	sdl::imtdesk::ImtDesk::EntityContextTicketsRequestArguments arguments = entityContextTicketsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		errorMessage = QStringLiteral("Invalid request input");
		return response;
	}

	QString entityType;
	if (arguments.input.Version_1_0->entityType){
		entityType = *arguments.input.Version_1_0->entityType;
	}

	QByteArray entityId;
	if (arguments.input.Version_1_0->entityId){
		entityId = *arguments.input.Version_1_0->entityId;
	}

	if (entityType.isEmpty() || entityId.isEmpty()){
		errorMessage = QStringLiteral("Entity context is empty");
		return response;
	}

	int offset = 0;
	int count = 250;
	if (arguments.input.Version_1_0->viewParams){
		if (arguments.input.Version_1_0->viewParams->offset){
			offset = qMax(0, *arguments.input.Version_1_0->viewParams->offset);
		}
		if (arguments.input.Version_1_0->viewParams->count){
			count = qMax(1, *arguments.input.Version_1_0->viewParams->count);
		}
	}

	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	istd::TDelPtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_ticketCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, nullptr));
	if (!iteratorPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to iterate tickets");
		return response;
	}

	QList<sdl::imtdesk::ImtDesk::CTicketItemData::V1_0> itemList;
	int matchedCount = 0;
	while (iteratorPtr->Next()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!iteratorPtr->GetObjectData(dataPtr)){
			continue;
		}

		const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(dataPtr.GetPtr());
		if (ticketPtr == nullptr){
			continue;
		}

		if (!HasTicketVisibility(contextPtr, ticketPtr, m_userCollectionCompPtr.GetPtr(), m_userGroupInfoProviderCompPtr.GetPtr())){
			continue;
		}

		if (!TicketHasEntityContext(ticketPtr, m_entityReferenceStorageCompPtr.GetPtr(), entityType, entityId)){
			continue;
		}

		if (matchedCount < offset){
			++matchedCount;
			continue;
		}

		if (count >= 0 && itemList.size() >= count){
			break;
		}

		sdl::imtdesk::ImtDesk::CTicketItemData::V1_0 itemData;
		itemData.id = ticketPtr->GetId();
		itemData.typeId = iteratorPtr->GetObjectTypeId();
		itemData.number = ticketPtr->GetNumber();
		itemData.title = ticketPtr->GetTitle();
		itemData.status = imtdeskgql::GetSdlTypeFromStatusType(ticketPtr->GetStatus());
		itemData.priority = imtdeskgql::GetSdlTypeFromPriorityType(ticketPtr->GetPriority());
		itemData.createdAt = ticketPtr->GetCreatedAt();

		QStringList assigneeNames;
		for (const QByteArray& assigneeId : ticketPtr->GetAssigneeIds()){
			assigneeNames << imtauth::GetUserName(*m_userCollectionCompPtr, assigneeId);
		}
		itemData.assignee = assigneeNames.join(';');

		itemList << itemData;
		++matchedCount;
	}

	response.Version_1_0.Emplace();
	if (!itemList.isEmpty()){
		response.Version_1_0->items.Emplace().FromList(itemList);
	}

	return response;
}


sdl::imtdesk::ImtDesk::CCreateEntityContextTicketPayload CEntityContextTicketsControllerComp::OnCreateEntityContextTicket(
			const sdl::imtdesk::ImtDesk::CCreateEntityContextTicketGqlRequest& createEntityContextTicketRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtdesk::ImtDesk::CCreateEntityContextTicketPayload response;

	if (!m_ticketCollectionCompPtr.IsValid() || !m_ticketFactCompPtr.IsValid() || !m_entityReferenceStorageCompPtr.IsValid()){
		errorMessage = QStringLiteral("Ticket creation dependencies are not configured");
		return response;
	}

	sdl::imtdesk::ImtDesk::CreateEntityContextTicketRequestArguments arguments = createEntityContextTicketRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		errorMessage = QStringLiteral("Invalid request input");
		return response;
	}

	QString entityType;
	if (arguments.input.Version_1_0->entityType){
		entityType = *arguments.input.Version_1_0->entityType;
	}

	QByteArray entityId;
	if (arguments.input.Version_1_0->entityId){
		entityId = *arguments.input.Version_1_0->entityId;
	}

	QString title;
	if (arguments.input.Version_1_0->title){
		title = *arguments.input.Version_1_0->title;
	}

	QString description;
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}

	QString entityDisplayName = QString::fromUtf8(entityId);
	if (arguments.input.Version_1_0->entityDisplayName && !arguments.input.Version_1_0->entityDisplayName->isEmpty()){
		entityDisplayName = *arguments.input.Version_1_0->entityDisplayName;
	}

	if (entityType.isEmpty() || entityId.isEmpty() || title.trimmed().isEmpty()){
		errorMessage = QStringLiteral("Entity context and title are required");
		return response;
	}

	istd::TUniqueInterfacePtr<imtdesk::ISupportTicket> ticketPtr = m_ticketFactCompPtr.CreateInstance();
	if (!ticketPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create ticket instance");
		return response;
	}

	const QByteArray ticketId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	ticketPtr->SetId(ticketId);
	ticketPtr->SetTitle(title.trimmed());
	ticketPtr->SetDescription(description.trimmed());
	ticketPtr->SetTicketType(imtdesk::ISupportTicket::TT_SUPPORT_REQUEST);
	ticketPtr->SetStatus(imtdesk::ISupportTicket::TS_OPEN);
	ticketPtr->SetStateReason(imtdesk::ISupportTicket::SR_NONE);
	ticketPtr->SetPriority(imtdesk::ISupportTicket::TP_MEDIUM);
	ticketPtr->SetAssigneeIds(QByteArrayList());

	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		ticketPtr->SetReporterId(contextPtr->GetUserId());
	}

	const QString entityUrl = QStringLiteral("%1/%2").arg(entityType, QString::fromUtf8(entityId));
	const QByteArray entityReferenceId = m_entityReferenceStorageCompPtr->CreateEntityReference(
				entityType, entityId, entityDisplayName, entityUrl);
	if (!entityReferenceId.isEmpty()){
		ticketPtr->SetEntityReferences(QByteArrayList() << entityReferenceId);
	}

	const QByteArray createdId = m_ticketCollectionCompPtr->InsertNewObject(
				"Ticket", "", "", ticketPtr.GetPtr(), ticketId);
	if (createdId.isEmpty()){
		errorMessage = QStringLiteral("Unable to create ticket");
		return response;
	}

	response.Version_1_0.Emplace();
	response.Version_1_0->id = createdId;
	return response;
}


} // namespace imtdeskgql
