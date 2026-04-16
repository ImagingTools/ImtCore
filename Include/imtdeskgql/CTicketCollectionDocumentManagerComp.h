// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtdesk/IEntityTypeProvider.h>
#include <imtchat/IChatMessage.h>
#include <imtchat/IChatService.h>
#include <imtchat/IAttachmentStorage.h>
#include <imtdoc/IDocumentManager.h>
#include <imtdoc/IDocumentManagerEventHandler.h>
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>

// ACF includes
#include <ibase/IApplicationInfo.h>

// Generated includes
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/TicketCollectionDocumentManager.h>


namespace imtdeskgql
{


class CTicketCollectionDocumentManagerComp: 
			public sdl::imtdesk::TicketCollectionDocumentManager::CGraphQlHandlerCompBase,
			virtual public imtdoc::IDocumentManagerEventHandler
{
public:
	typedef sdl::imtdesk::TicketCollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTicketCollectionDocumentManagerComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentManagerEventHandler)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentManager", "Collection document manager", false, "CollectionDocumentManager");
		I_ASSIGN(m_ticketCollectionCompPtr, "TicketCollection", "Ticket collection for refreshing DB-computed fields after save", false, "TicketCollection");
		I_ASSIGN(m_messageCollectionCompPtr, "MessageCollection", "Collection of chat messages", false, "MessageCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
		I_ASSIGN(m_chatServiceCompPtr, "ChatService", "Chat service for message and conversation management", false, "ChatService");
		I_ASSIGN(m_attachmentStorageCompPtr, "AttachmentStorage", "Storage service for binary attachments", true, "AttachmentStorage");
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for direct entity reference writes", true, "DatabaseEngine");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info for ObjectLink URL construction", true, "ApplicationInfo");
		I_ASSIGN_MULTI_0(m_entityTypeProvidersCompPtr, "EntityTypeProviders", "Registered entity type providers", false);
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

	// reimplemented (imtdoc::IDocumentManagerEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override;

private:
	I_REF(imtdoc::IDocumentManager, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_ticketCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_messageCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtchat::IChatService, m_chatServiceCompPtr);
	I_REF(imtchat::IAttachmentStorage, m_attachmentStorageCompPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_MULTIREF(imtdesk::IEntityTypeProvider, m_entityTypeProvidersCompPtr);
};


} // namespace imtdeskgql

