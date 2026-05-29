// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketCollectionControllerComp.h>
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/ImtDesk.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtauth/CUserGroupFilter.h>
#include <imtdesk/ISupportTicket.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtauth/imtauth.h>


namespace imtdeskgql
{


bool CTicketCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtdesk::ImtDesk::CTicketsListGqlRequest& /*listRequest*/,
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

	QByteArrayList assigneeIds = ticketPtr->GetAssigneeIds();
	QStringList assigneeNames;
	for (const QByteArray& userId : std::as_const(assigneeIds)){
		assigneeNames << imtauth::GetUserName(*m_userCollectionCompPtr, userId);
	}
	representationObject.assignee = assigneeNames.join(';');

	representationObject.number = ticketPtr->GetNumber();
	representationObject.createdAt = ticketPtr->GetCreatedAt();

	return true;
}


void CTicketCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& /*viewParamsGql*/,
			iprm::CParamsSet* filterParamsPtr) const
{
	if (filterParamsPtr == nullptr){
		return;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr || userInfoPtr->IsAdmin()){
		return;
	}

	istd::TDelPtr<imtauth::CUserGroupFilter> groupFilter = new imtauth::CUserGroupFilter();
	groupFilter->SetUserId(gqlContextPtr->GetUserId());
	groupFilter->SetGroupIds(userInfoPtr->GetGroups());
	filterParamsPtr->SetEditableParameter("GroupFilter", groupFilter.PopPtr(), true);
}


} // namespace imtdeskgql


