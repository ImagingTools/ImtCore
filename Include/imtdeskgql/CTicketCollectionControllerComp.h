// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtchat/IChatService.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserGroupInfoProvider.h>
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
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider for permissions", true, "UserGroupInfoProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtdesk::ImtDesk::CTicketCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtdesk::ImtDesk::CTicketsListGqlRequest& listRequest,
				sdl::imtdesk::ImtDesk::CTicketItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlParamObject& viewParamsGql,
				iprm::CParamsSet* filterParamsPtr) const override;

protected:
	I_FACT(imtdesk::ISupportTicket, m_ticketFactCompPtr);
	I_REF(imtchat::IChatService, m_chatServiceCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
};


} // namespace imtdeskgql
