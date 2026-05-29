// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <iprm/IIdParam.h>

// Generated includes
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/TicketCollectionDocumentService>


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


inline imtdesk::ISupportTicket::TicketType GetTicketTypeFromSdlType(sdl::V1_0::imtdesk::TicketType sdlTicketType)
{
	switch(sdlTicketType){
	case sdl::V1_0::imtdesk::TicketType::AccessRequest:
		return imtdesk::ISupportTicket::TT_ACCESS_REQUEST;
	case sdl::V1_0::imtdesk::TicketType::SupportRequest:
		return imtdesk::ISupportTicket::TT_SUPPORT_REQUEST;
	case sdl::V1_0::imtdesk::TicketType::FeatureRequest:
		return imtdesk::ISupportTicket::TT_FEATURE_REQUEST;
	case sdl::V1_0::imtdesk::TicketType::BugReport:
		return imtdesk::ISupportTicket::TT_BUG_REPORT;
	}

	return imtdesk::ISupportTicket::TT_ACCESS_REQUEST;
}


inline sdl::V1_0::imtdesk::TicketType GetSdlTypeFromTicketType(imtdesk::ISupportTicket::TicketType ticketType)
{
	switch(ticketType){
	case imtdesk::ISupportTicket::TT_ACCESS_REQUEST:
		return sdl::V1_0::imtdesk::TicketType::AccessRequest;
	case imtdesk::ISupportTicket::TT_SUPPORT_REQUEST:
		return sdl::V1_0::imtdesk::TicketType::SupportRequest;
	case imtdesk::ISupportTicket::TT_FEATURE_REQUEST:
		return sdl::V1_0::imtdesk::TicketType::FeatureRequest;
	case imtdesk::ISupportTicket::TT_BUG_REPORT:
		return sdl::V1_0::imtdesk::TicketType::BugReport;
	}

	return sdl::V1_0::imtdesk::TicketType::AccessRequest;
}


inline imtdesk::ISupportTicket::TicketStatus GetStatusTypeFromSdlType(sdl::V1_0::imtdesk::TicketStatus sdlTicketStatus)
{
	switch(sdlTicketStatus){
	case sdl::V1_0::imtdesk::TicketStatus::Open:
		return imtdesk::ISupportTicket::TicketStatus::TS_OPEN;
	case sdl::V1_0::imtdesk::TicketStatus::Closed:
		return imtdesk::ISupportTicket::TicketStatus::TS_CLOSED;
	}

	return imtdesk::ISupportTicket::TicketStatus::TS_OPEN;
}


inline sdl::V1_0::imtdesk::TicketStatus GetSdlTypeFromStatusType(imtdesk::ISupportTicket::TicketStatus ticketStatus)
{
	switch (ticketStatus){
	case imtdesk::ISupportTicket::TS_OPEN:
		return sdl::V1_0::imtdesk::TicketStatus::Open;
	case imtdesk::ISupportTicket::TS_CLOSED:
		return sdl::V1_0::imtdesk::TicketStatus::Closed;
	}

	return sdl::V1_0::imtdesk::TicketStatus::Open;
}


inline imtdesk::ISupportTicket::StateReason GetStateReasonFromSdlType(sdl::V1_0::imtdesk::StateReason sdlStateReason)
{
	switch(sdlStateReason){
	case sdl::V1_0::imtdesk::StateReason::None:
		return imtdesk::ISupportTicket::StateReason::SR_NONE;
	case sdl::V1_0::imtdesk::StateReason::Completed:
		return imtdesk::ISupportTicket::StateReason::SR_COMPLETED;
	case sdl::V1_0::imtdesk::StateReason::NotPlanned:
		return imtdesk::ISupportTicket::StateReason::SR_NOT_PLANNED;
	case sdl::V1_0::imtdesk::StateReason::Reopened:
		return imtdesk::ISupportTicket::StateReason::SR_REOPENED;
	}

	return imtdesk::ISupportTicket::StateReason::SR_NONE;
}


inline sdl::V1_0::imtdesk::StateReason GetSdlTypeFromStateReason(imtdesk::ISupportTicket::StateReason stateReason)
{
	switch (stateReason){
	case imtdesk::ISupportTicket::SR_NONE:
		return sdl::V1_0::imtdesk::StateReason::None;
	case imtdesk::ISupportTicket::SR_COMPLETED:
		return sdl::V1_0::imtdesk::StateReason::Completed;
	case imtdesk::ISupportTicket::SR_NOT_PLANNED:
		return sdl::V1_0::imtdesk::StateReason::NotPlanned;
	case imtdesk::ISupportTicket::SR_REOPENED:
		return sdl::V1_0::imtdesk::StateReason::Reopened;
	}

	return sdl::V1_0::imtdesk::StateReason::None;
}


inline imtdesk::ISupportTicket::TicketPriority GetPriorityTypeFromSdlType(sdl::V1_0::imtdesk::TicketPriority sdlTicketPriority)
{
	switch (sdlTicketPriority){
	case sdl::V1_0::imtdesk::TicketPriority::Low:
		return imtdesk::ISupportTicket::TicketPriority::TP_LOW;
	case sdl::V1_0::imtdesk::TicketPriority::Medium:
		return imtdesk::ISupportTicket::TicketPriority::TP_MEDIUM;
	case sdl::V1_0::imtdesk::TicketPriority::High:
		return imtdesk::ISupportTicket::TicketPriority::TP_HIGH;
	case sdl::V1_0::imtdesk::TicketPriority::Critical:
		return imtdesk::ISupportTicket::TicketPriority::TP_CRITICAL;
	}

	return imtdesk::ISupportTicket::TicketPriority::TP_LOW;
}


inline sdl::V1_0::imtdesk::TicketPriority GetSdlTypeFromPriorityType(imtdesk::ISupportTicket::TicketPriority ticketPriority)
{
	switch (ticketPriority){
	case imtdesk::ISupportTicket::TP_LOW:
		return sdl::V1_0::imtdesk::TicketPriority::Low;
	case imtdesk::ISupportTicket::TP_MEDIUM:
		return sdl::V1_0::imtdesk::TicketPriority::Medium;
	case imtdesk::ISupportTicket::TP_HIGH:
		return sdl::V1_0::imtdesk::TicketPriority::High;
	case imtdesk::ISupportTicket::TP_CRITICAL:
		return sdl::V1_0::imtdesk::TicketPriority::Critical;
	}

	return sdl::V1_0::imtdesk::TicketPriority::Low;
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


