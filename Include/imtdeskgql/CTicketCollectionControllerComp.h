// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtchat/IChatService.h>
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/ImtDesk.h>


namespace imtdeskgql
{


class CTicketCollectionControllerComp:
		public sdl::imtdesk::ImtDesk::CTicketCollectionControllerCompBase
{
public:
	typedef sdl::imtdesk::ImtDesk::CTicketCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTicketCollectionControllerComp);
		I_ASSIGN(m_ticketFactCompPtr, "TicketFactory", "Factory for creating ticket instances", true, "TicketFactory");
		I_ASSIGN(m_chatServiceCompPtr, "ChatService", "Chat service for message and conversation management", false, "ChatService");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtdesk::ImtDesk::CTicketCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtdesk::ImtDesk::CTicketsListGqlRequest& listRequest,
				sdl::imtdesk::ImtDesk::CTicketItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtdesk::ImtDesk::CTicketData::V1_0& dataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtdesk::ImtDesk::CTicketItemGqlRequest& itemRequest,
				sdl::imtdesk::ImtDesk::CTicketData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtdesk::ImtDesk::CTicketUpdateGqlRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::imtdesk::ImtDesk::CTicketData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(imtdesk::ISupportTicket, m_ticketFactCompPtr);
	I_REF(imtchat::IChatService, m_chatServiceCompPtr);
};


} // namespace imtdeskgql
