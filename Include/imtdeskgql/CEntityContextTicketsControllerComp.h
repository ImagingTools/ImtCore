// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdesk/ISupportTicket.h>
#include <imtdesk/IEntityReferenceStorage.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/ImtDesk.h>


namespace imtdeskgql
{


class CEntityContextTicketsControllerComp:
		public sdl::imtdesk::ImtDesk::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtdesk::ImtDesk::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CEntityContextTicketsControllerComp);
		I_ASSIGN(m_ticketCollectionCompPtr, "TicketCollection", "Ticket collection", true, "TicketCollection");
		I_ASSIGN(m_ticketFactCompPtr, "TicketFactory", "Factory for creating ticket instances", true, "TicketFactory");
		I_ASSIGN(m_entityReferenceStorageCompPtr, "EntityReferenceStorage", "Storage service for entity references", true, "EntityReferenceStorage");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider for permissions", true, "UserGroupInfoProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtdesk::ImtDesk::CGraphQlHandlerCompBase)
	virtual sdl::imtdesk::ImtDesk::CEntityContextTicketsPayload OnEntityContextTickets(
				const sdl::imtdesk::ImtDesk::CEntityContextTicketsGqlRequest& entityContextTicketsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtdesk::ImtDesk::CCreateEntityContextTicketPayload OnCreateEntityContextTicket(
				const sdl::imtdesk::ImtDesk::CCreateEntityContextTicketGqlRequest& createEntityContextTicketRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_ticketCollectionCompPtr);
	I_FACT(imtdesk::ISupportTicket, m_ticketFactCompPtr);
	I_REF(imtdesk::IEntityReferenceStorage, m_entityReferenceStorageCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
};


} // namespace imtdeskgql

