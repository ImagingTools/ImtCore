// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketCollectionControllerComp.h>

// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>


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
	representationObject.typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	representationObject.title = ticketPtr->GetTitle();
	representationObject.assigneeId = ticketPtr->GetAssigneeId();
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
	representationPayload.title = ticketPtr->GetTitle();
	representationPayload.description = ticketPtr->GetDescription();
	representationPayload.assigneeId = ticketPtr->GetAssigneeId();
	representationPayload.reporterId = ticketPtr->GetReporterId();
	representationPayload.conversationId = ticketPtr->GetConversationId();
	representationPayload.messageId = ticketPtr->GetMessageId();
	representationPayload.createdAt = ticketPtr->GetCreatedAt();
	representationPayload.updatedAt = ticketPtr->GetUpdatedAt();
	representationPayload.resolvedAt = ticketPtr->GetResolvedAt();

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

	if (representation.assigneeId){
		ticketPtr->SetAssigneeId(*representation.assigneeId);
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

	return true;
}


} // namespace imtdeskgql
