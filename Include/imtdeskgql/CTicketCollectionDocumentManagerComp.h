// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtchat/IChatMessage.h>
#include <imtchat/IConversation.h>
#include <imtdoc/IDocumentManager.h>
#include <imtbase/IObjectCollection.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>

// Generated includes
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/TicketCollectionDocumentManager.h>


namespace imtdeskgql
{


class CTicketCollectionDocumentManagerComp: public sdl::imtdesk::TicketCollectionDocumentManager::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtdesk::TicketCollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTicketCollectionDocumentManagerComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentManager", "Collection document manager", false, "CollectionDocumentManager");
		I_ASSIGN(m_messageCollectionCompPtr, "MessageCollection", "Collection of chat messages", false, "MessageCollection");
		I_ASSIGN(m_messageFactCompPtr, "MessageFactory", "Factory for creating chat message instances", false, "MessageFactory");
		I_ASSIGN(m_conversationCollectionCompPtr, "ConversationCollection", "Collection of conversations", false, "ConversationCollection");
		I_ASSIGN(m_conversationFactCompPtr, "ConversationFactory", "Factory for creating conversation instances", false, "ConversationFactory");
	I_END_COMPONENT

protected:
	virtual sdl::imtdesk::ImtDesk::CTicketData OnGetTicketRepresentation(
				const sdl::imtdesk::TicketCollectionDocumentManager::CGetTicketRepresentationGqlRequest& getTicketRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnUpdateTicketFromRepresentation(
				const sdl::imtdesk::TicketCollectionDocumentManager::CUpdateTicketFromRepresentationGqlRequest& updateTicketFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentManager, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_messageCollectionCompPtr);
	I_FACT(imtchat::IChatMessage, m_messageFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_conversationCollectionCompPtr);
	I_FACT(imtchat::IConversation, m_conversationFactCompPtr);
};


} // namespace imtdeskgql

