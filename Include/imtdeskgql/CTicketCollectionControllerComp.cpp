// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketCollectionControllerComp.h>

// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtdesk/ITicketAction.h>
#include <imtdeskgql/imtdeskgql.h>


namespace imtdeskgql
{


bool CTicketCollectionControllerComp::CreateRepresentationFromObject(
		const imtbase::IObjectCollectionIterator& objectCollectionIterator,
		const sdl::imtdesk::ImtDesk::CTicketsListGqlRequest& listRequest,
		sdl::imtdesk::ImtDesk::CTicketItemData::V1_0& representationObject,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Ticket collection is not set");
		SendErrorMessage(0, errorMessage, "CTicketCollectionControllerComp");
		return false;
	}

	const QByteArray objectId = objectCollectionIterator.GetObjectId();

	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtdesk::ISupportTicket* ticketPtr = nullptr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(dataPtr.GetPtr());
	}

	if (ticketPtr == nullptr){
		errorMessage = QString("Unable to create representation from ticket '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CTicketCollectionControllerComp");
		return false;
	}

	Q_UNUSED(listRequest);

	representationObject.id = objectId;
	representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	representationObject.title = ticketPtr->GetTitle();

	imtdesk::ISupportTicket::TicketStatus ticketStatus = ticketPtr->GetStatus();
	switch (ticketStatus){
	case imtdesk::ISupportTicket::TS_OPEN:
		representationObject.status = sdl::imtdesk::ImtDesk::TicketStatus::Open;
		break;
	case imtdesk::ISupportTicket::TS_CLOSED:
		representationObject.status = sdl::imtdesk::ImtDesk::TicketStatus::Closed;
		break;
	default:
		break;
	}

	representationObject.stateReason = imtdeskgql::GetSdlTypeFromStateReason(ticketPtr->GetStateReason());

	imtdesk::ISupportTicket::TicketPriority ticketPriority = ticketPtr->GetPriority();
	switch (ticketPriority){
	case imtdesk::ISupportTicket::TP_LOW:
		representationObject.priority = sdl::imtdesk::ImtDesk::TicketPriority::Low;
		break;
	case imtdesk::ISupportTicket::TP_MEDIUM:
		representationObject.priority = sdl::imtdesk::ImtDesk::TicketPriority::Medium;
		break;
	case imtdesk::ISupportTicket::TP_HIGH:
		representationObject.priority = sdl::imtdesk::ImtDesk::TicketPriority::High;
		break;
	case imtdesk::ISupportTicket::TP_CRITICAL:
		representationObject.priority = sdl::imtdesk::ImtDesk::TicketPriority::Critical;
		break;
	}

	representationObject.assigneeIds.Emplace().FromList(ticketPtr->GetAssigneeIds());
	representationObject.labelIds.Emplace().FromList(ticketPtr->GetLabelIds());
	representationObject.locked = ticketPtr->IsLocked();
	representationObject.number = ticketPtr->GetNumber();
	representationObject.createdAt = ticketPtr->GetCreatedAt();

	return true;
}


istd::IChangeableUniquePtr CTicketCollectionControllerComp::CreateObjectFromRepresentation(
		const sdl::imtdesk::ImtDesk::CTicketData::V1_0& dataRepresentation,
		QByteArray& newObjectId,
		QString& errorMessage) const
{
	if (!m_ticketFactCompPtr.IsValid()){
		errorMessage = QStringLiteral("Ticket factory is not set");
		SendErrorMessage(0, errorMessage, "CTicketCollectionControllerComp");
		return nullptr;
	}

	imtdesk::ISupportTicketUniquePtr ticketPtr = m_ticketFactCompPtr.CreateInstance();
	if (!ticketPtr.IsValid()){
		errorMessage = QStringLiteral("Failed to create ticket instance");
		SendErrorMessage(0, errorMessage, "CTicketCollectionControllerComp");
		return nullptr;
	}

	if (!FillObjectFromRepresentation(dataRepresentation, *ticketPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return ticketPtr;
}


bool CTicketCollectionControllerComp::CreateRepresentationFromObject(
		const istd::IChangeable& data,
		const sdl::imtdesk::ImtDesk::CTicketItemGqlRequest& itemRequest,
		sdl::imtdesk::ImtDesk::CTicketData::V1_0& representationPayload,
		QString& errorMessage) const
{
	const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(&data);
	if (ticketPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid ISupportTicket");
		SendErrorMessage(0, errorMessage, "CTicketCollectionControllerComp");
		return false;
	}

	Q_UNUSED(itemRequest);

	representationPayload.id = ticketPtr->GetId();
	representationPayload.number = ticketPtr->GetNumber();
	representationPayload.title = ticketPtr->GetTitle();
	representationPayload.description = ticketPtr->GetDescription();
	representationPayload.assigneeIds.Emplace().FromList(ticketPtr->GetAssigneeIds());
	representationPayload.reporterId = ticketPtr->GetReporterId();
	representationPayload.conversationId = ticketPtr->GetConversationId();
	representationPayload.messageId = ticketPtr->GetMessageId();
	representationPayload.labelIds.Emplace().FromList(ticketPtr->GetLabelIds());
	representationPayload.locked = ticketPtr->IsLocked();
	representationPayload.lockReason = ticketPtr->GetLockReason();
	representationPayload.createdAt = ticketPtr->GetCreatedAt();
	representationPayload.updatedAt = ticketPtr->GetUpdatedAt();
	representationPayload.closedAt = ticketPtr->GetClosedAt();
	representationPayload.resolvedAt = ticketPtr->GetResolvedAt();

	// Populate activity items from ticket actions (ITicketAction collection)
	if (m_ticketActionsCollectionCompPtr.IsValid()){
		const QByteArray ticketId = ticketPtr->GetId();
		imtbase::IObjectCollection::IteratorPtr iteratorPtr = m_ticketActionsCollectionCompPtr->CreateIterator();
		if (iteratorPtr.IsValid()){
			while (iteratorPtr->IsValid()){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (iteratorPtr->GetObjectData(dataPtr)){
					const imtdesk::ITicketAction* actionPtr = dynamic_cast<const imtdesk::ITicketAction*>(dataPtr.GetPtr());
					if (actionPtr != nullptr && actionPtr->GetTicketId() == ticketId){
						sdl::imtdesk::ImtDesk::CTicketActivityItem sdlItem;
						sdlItem.Version_1_0.Emplace();
						sdlItem.Version_1_0->itemType = sdl::imtdesk::ImtDesk::ActivityItemType::Action;
						sdlItem.Version_1_0->userId = actionPtr->GetUserId();
						sdlItem.Version_1_0->userName = actionPtr->GetUserName();
						sdlItem.Version_1_0->timestamp = actionPtr->GetTimestamp();
						sdlItem.Version_1_0->actionType = actionPtr->GetActionTypeName();
						sdlItem.Version_1_0->actionDescription = actionPtr->GetActionTypeDescription();
						if (!representationPayload.activityItems.has_value()){
							representationPayload.activityItems.Emplace();
						}
						representationPayload.activityItems->Append(sdlItem);
					}
				}
				iteratorPtr->MoveNext();
			}
		}
	}

	return true;
}


bool CTicketCollectionControllerComp::UpdateObjectFromRepresentationRequest(
		const imtgql::CGqlRequest& /*rawGqlRequest*/,
		const sdl::imtdesk::ImtDesk::CTicketUpdateGqlRequest& updateRequest,
		istd::IChangeable& object,
		QString& errorMessage) const
{
	imtdesk::ISupportTicket* ticketPtr = dynamic_cast<imtdesk::ISupportTicket*>(&object);
	if (ticketPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid ISupportTicket");
		SendErrorMessage(0, errorMessage, "CTicketCollectionControllerComp");
		return false;
	}

	auto arguments = updateRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		I_CRITICAL();
		return false;
	}

	if (!arguments.input.Version_1_0->id.has_value()){
		I_CRITICAL();
		return false;
	}

	if (!arguments.input.Version_1_0->item.has_value()){
		I_CRITICAL();
		return false;
	}

	QByteArray objectId = *arguments.input.Version_1_0->id;
	ticketPtr->ResetData();
	ticketPtr->SetId(objectId);

	sdl::imtdesk::ImtDesk::CTicketData::V1_0 itemData = *arguments.input.Version_1_0->item;
	return FillObjectFromRepresentation(itemData, *ticketPtr, objectId, errorMessage);
}


bool CTicketCollectionControllerComp::FillObjectFromRepresentation(
		const sdl::imtdesk::ImtDesk::CTicketData::V1_0& representation,
		istd::IChangeable& object,
		QByteArray& objectId,
		QString& errorMessage) const
{
	imtdesk::ISupportTicket* ticketPtr = dynamic_cast<imtdesk::ISupportTicket*>(&object);
	if (ticketPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid ISupportTicket");
		SendErrorMessage(0, errorMessage, "CTicketCollectionControllerComp");
		return false;
	}

	if (representation.id){
		objectId = *representation.id;
	}

	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	ticketPtr->SetId(objectId);

	if (representation.title){
		ticketPtr->SetTitle(*representation.title);
	}

	if (representation.description){
		ticketPtr->SetDescription(*representation.description);
	}

	if (representation.assigneeIds){
		ticketPtr->SetAssigneeIds(representation.assigneeIds->ToList());
	}

	if (representation.reporterId){
		ticketPtr->SetReporterId(*representation.reporterId);
	}

	if (representation.conversationId){
		ticketPtr->SetConversationId(*representation.conversationId);
	}

	if (representation.messageId){
		ticketPtr->SetMessageId(*representation.messageId);
	}

	if (representation.createdAt){
		ticketPtr->SetCreatedAt(*representation.createdAt);
	}

	if (representation.updatedAt){
		ticketPtr->SetUpdatedAt(*representation.updatedAt);
	}

	if (representation.resolvedAt){
		ticketPtr->SetResolvedAt(*representation.resolvedAt);
	}

	if (representation.closedAt){
		ticketPtr->SetClosedAt(*representation.closedAt);
	}

	if (representation.labelIds){
		ticketPtr->SetLabelIds(representation.labelIds->ToList());
	}

	if (representation.locked){
		ticketPtr->SetLocked(*representation.locked);
	}

	if (representation.lockReason){
		ticketPtr->SetLockReason(*representation.lockReason);
	}

	if (representation.number){
		ticketPtr->SetNumber(*representation.number);
	}

	if (representation.stateReason){
		ticketPtr->SetStateReason(imtdeskgql::GetStateReasonFromSdlType(*representation.stateReason));
	}

	if (representation.activityItems){
		// Activity items of type Action are stored in the TicketActions collection
		// independently from the ticket. The SDL transport layer carries them
		// together for convenience, but they are persisted separately.
	}

	return true;
}


} // namespace imtdeskgql
