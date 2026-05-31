// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <iprm/IIdParam.h>

// Generated includes
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/TicketCollectionDocumentService.h>


/**
	\file imtdeskgql.h
	\brief Main header file for the imtdeskgql module - Service Desk GraphQL API Layer
*/

/**
	\namespace imtdeskgql
	\brief Service Desk GraphQL API Layer Module

	The \b imtdeskgql module bridges the imtdesk domain model with the GraphQL
	API defined in the ImtDesk SDL schema. It provides collection and object
	controller components that translate between C++ domain objects and
	SDL-generated GraphQL representation types.

	\section imtdeskgql_overview Overview

	- \b CTicketCollectionControllerComp — handles TicketsList / TicketItem queries,
	  TicketCreate and TicketUpdate mutations
	- \b CTicketCollectionDocumentServiceComp — handles GetTicketRepresentation query,
	  UpdateTicketFromRepresentation mutation for the document editor

	\section imtdeskgql_related Related Modules
	- \b imtdesk — domain model
	- \b imtdeskdb — database persistence
	- \b imtdeskgui — QML user interface
*/
namespace imtdeskgql
{


inline imtdesk::ISupportTicket::TicketType GetTicketTypeFromSdlType(sdl::imtdesk::ImtDesk::TicketType sdlTicketType)
{
	switch(sdlTicketType){
	case sdl::imtdesk::ImtDesk::TicketType::AccessRequest:
		return imtdesk::ISupportTicket::TT_ACCESS_REQUEST;
	case sdl::imtdesk::ImtDesk::TicketType::SupportRequest:
		return imtdesk::ISupportTicket::TT_SUPPORT_REQUEST;
	case sdl::imtdesk::ImtDesk::TicketType::FeatureRequest:
		return imtdesk::ISupportTicket::TT_FEATURE_REQUEST;
	case sdl::imtdesk::ImtDesk::TicketType::BugReport:
		return imtdesk::ISupportTicket::TT_BUG_REPORT;
	}

	return imtdesk::ISupportTicket::TT_ACCESS_REQUEST;
}


inline sdl::imtdesk::ImtDesk::TicketType GetSdlTypeFromTicketType(imtdesk::ISupportTicket::TicketType ticketType)
{
	switch(ticketType){
	case imtdesk::ISupportTicket::TT_ACCESS_REQUEST:
		return sdl::imtdesk::ImtDesk::TicketType::AccessRequest;
	case imtdesk::ISupportTicket::TT_SUPPORT_REQUEST:
		return sdl::imtdesk::ImtDesk::TicketType::SupportRequest;
	case imtdesk::ISupportTicket::TT_FEATURE_REQUEST:
		return sdl::imtdesk::ImtDesk::TicketType::FeatureRequest;
	case imtdesk::ISupportTicket::TT_BUG_REPORT:
		return sdl::imtdesk::ImtDesk::TicketType::BugReport;
	}

	return sdl::imtdesk::ImtDesk::TicketType::AccessRequest;
}


inline imtdesk::ISupportTicket::TicketStatus GetStatusTypeFromSdlType(sdl::imtdesk::ImtDesk::TicketStatus sdlTicketStatus)
{
	switch(sdlTicketStatus){
	case sdl::imtdesk::ImtDesk::TicketStatus::Open:
		return imtdesk::ISupportTicket::TicketStatus::TS_OPEN;
	case sdl::imtdesk::ImtDesk::TicketStatus::Closed:
		return imtdesk::ISupportTicket::TicketStatus::TS_CLOSED;
	}

	return imtdesk::ISupportTicket::TicketStatus::TS_OPEN;
}


inline sdl::imtdesk::ImtDesk::TicketStatus GetSdlTypeFromStatusType(imtdesk::ISupportTicket::TicketStatus ticketStatus)
{
	switch (ticketStatus){
	case imtdesk::ISupportTicket::TS_OPEN:
		return sdl::imtdesk::ImtDesk::TicketStatus::Open;
	case imtdesk::ISupportTicket::TS_CLOSED:
		return sdl::imtdesk::ImtDesk::TicketStatus::Closed;
	}

	return sdl::imtdesk::ImtDesk::TicketStatus::Open;
}


inline imtdesk::ISupportTicket::StateReason GetStateReasonFromSdlType(sdl::imtdesk::ImtDesk::StateReason sdlStateReason)
{
	switch(sdlStateReason){
	case sdl::imtdesk::ImtDesk::StateReason::None:
		return imtdesk::ISupportTicket::StateReason::SR_NONE;
	case sdl::imtdesk::ImtDesk::StateReason::Completed:
		return imtdesk::ISupportTicket::StateReason::SR_COMPLETED;
	case sdl::imtdesk::ImtDesk::StateReason::NotPlanned:
		return imtdesk::ISupportTicket::StateReason::SR_NOT_PLANNED;
	case sdl::imtdesk::ImtDesk::StateReason::Reopened:
		return imtdesk::ISupportTicket::StateReason::SR_REOPENED;
	}

	return imtdesk::ISupportTicket::StateReason::SR_NONE;
}


inline sdl::imtdesk::ImtDesk::StateReason GetSdlTypeFromStateReason(imtdesk::ISupportTicket::StateReason stateReason)
{
	switch (stateReason){
	case imtdesk::ISupportTicket::SR_NONE:
		return sdl::imtdesk::ImtDesk::StateReason::None;
	case imtdesk::ISupportTicket::SR_COMPLETED:
		return sdl::imtdesk::ImtDesk::StateReason::Completed;
	case imtdesk::ISupportTicket::SR_NOT_PLANNED:
		return sdl::imtdesk::ImtDesk::StateReason::NotPlanned;
	case imtdesk::ISupportTicket::SR_REOPENED:
		return sdl::imtdesk::ImtDesk::StateReason::Reopened;
	}

	return sdl::imtdesk::ImtDesk::StateReason::None;
}


inline imtdesk::ISupportTicket::TicketPriority GetPriorityTypeFromSdlType(sdl::imtdesk::ImtDesk::TicketPriority sdlTicketPriority)
{
	switch (sdlTicketPriority){
	case sdl::imtdesk::ImtDesk::TicketPriority::Low:
		return imtdesk::ISupportTicket::TicketPriority::TP_LOW;
	case sdl::imtdesk::ImtDesk::TicketPriority::Medium:
		return imtdesk::ISupportTicket::TicketPriority::TP_MEDIUM;
	case sdl::imtdesk::ImtDesk::TicketPriority::High:
		return imtdesk::ISupportTicket::TicketPriority::TP_HIGH;
	case sdl::imtdesk::ImtDesk::TicketPriority::Critical:
		return imtdesk::ISupportTicket::TicketPriority::TP_CRITICAL;
	}

	return imtdesk::ISupportTicket::TicketPriority::TP_LOW;
}


inline sdl::imtdesk::ImtDesk::TicketPriority GetSdlTypeFromPriorityType(imtdesk::ISupportTicket::TicketPriority ticketPriority)
{
	switch (ticketPriority){
	case imtdesk::ISupportTicket::TP_LOW:
		return sdl::imtdesk::ImtDesk::TicketPriority::Low;
	case imtdesk::ISupportTicket::TP_MEDIUM:
		return sdl::imtdesk::ImtDesk::TicketPriority::Medium;
	case imtdesk::ISupportTicket::TP_HIGH:
		return sdl::imtdesk::ImtDesk::TicketPriority::High;
	case imtdesk::ISupportTicket::TP_CRITICAL:
		return sdl::imtdesk::ImtDesk::TicketPriority::Critical;
	}

	return sdl::imtdesk::ImtDesk::TicketPriority::Low;
}


/**
	Simple IIdParam implementation for passing ID-based filters to
	CreateObjectCollectionIterator (e.g. TicketId, ConversationId).
*/
class CIdParam: virtual public iprm::IIdParam
{
public:
	CIdParam(const QByteArray& id = QByteArray()) : m_id(id) {}

	// reimplemented (iprm::IIdParam)
	virtual QByteArray GetId() const override { return m_id; }
	virtual void SetId(const QByteArray& id) override { m_id = id; }

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& /*archive*/) override { return true; }

private:
	QByteArray m_id;
};


} // namespace imtdeskgql


