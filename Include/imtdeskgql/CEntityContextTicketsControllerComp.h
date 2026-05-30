// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdesk/ISupportTicket.h>
#include <imtdesk/IEntityReferenceStorage.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtchat/IChatService.h>
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/ImtDesk_fwd.h>


namespace imtdeskgql
{


class CEntityContextTicketsControllerComp:
		public sdl::V1_0::imtdesk::CImtDeskGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtdesk::CImtDeskGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CEntityContextTicketsControllerComp);
		I_ASSIGN(m_ticketCollectionCompPtr, "TicketCollection", "Ticket collection", true, "TicketCollection");
		I_ASSIGN(m_ticketFactCompPtr, "TicketFactory", "Factory for creating ticket instances", true, "TicketFactory");
		I_ASSIGN(m_entityReferenceStorageCompPtr, "EntityReferenceStorage", "Storage service for entity references", true, "EntityReferenceStorage");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider for permissions", true, "UserGroupInfoProvider");
		I_ASSIGN(m_chatServiceCompPtr, "ChatService", "Chat service for conversation creation", false, "ChatService");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtdesk::CImtDeskGqlHandlerCompBase)
	virtual sdl::V1_0::imtdesk::CEntityContextTicketsPayload OnEntityContextTickets(
				const sdl::V1_0::imtdesk::CEntityContextTicketsGqlRequest& entityContextTicketsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtdesk::CCreateEntityContextTicketPayload OnCreateEntityContextTicket(
				const sdl::V1_0::imtdesk::CCreateEntityContextTicketGqlRequest& createEntityContextTicketRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_ticketCollectionCompPtr);
	I_FACT(imtdesk::ISupportTicket, m_ticketFactCompPtr);
	I_REF(imtdesk::IEntityReferenceStorage, m_entityReferenceStorageCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_REF(imtchat::IChatService, m_chatServiceCompPtr);
};


} // namespace imtdeskgql

