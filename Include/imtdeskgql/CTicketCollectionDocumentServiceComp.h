// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtdesk/IEntityTypeProvider.h>
#include <imtdesk/IEntityReferenceStorage.h>
#include <imtchat/IChatMessage.h>
#include <imtchat/IChatService.h>
#include <imtchat/IAttachmentStorage.h>
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserGroupInfoProvider.h>

// Generated includes
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/TicketCollectionDocumentService>


namespace imtdeskgql
{


class CTicketCollectionDocumentServiceComp: 
			public sdl::imtdesk::TicketCollectionDocumentService::CGraphQlHandlerCompBase,
			virtual public imtdoc::IDocumentServiceEventHandler
{
public:
	typedef sdl::imtdesk::TicketCollectionDocumentService::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTicketCollectionDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentServiceEventHandler)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
		I_ASSIGN(m_ticketCollectionCompPtr, "TicketCollection", "Ticket collection for refreshing DB-computed fields after save", false, "TicketCollection");
		I_ASSIGN(m_messageCollectionCompPtr, "MessageCollection", "Collection of chat messages", false, "MessageCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
		I_ASSIGN(m_chatServiceCompPtr, "ChatService", "Chat service for message and conversation management", false, "ChatService");
		I_ASSIGN(m_attachmentStorageCompPtr, "AttachmentStorage", "Storage service for binary attachments", true, "AttachmentStorage");
		I_ASSIGN(m_entityReferenceStorageCompPtr, "EntityReferenceStorage", "Storage service for entity references", true, "EntityReferenceStorage");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info for ObjectLink URL construction", true, "ApplicationInfo");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider for permissions", true, "UserGroupInfoProvider");
		I_ASSIGN_MULTI_0(m_entityTypeProvidersCompPtr, "EntityTypeProviders", "Registered entity type providers", false);
	I_END_COMPONENT

protected:
	virtual sdl::imtdesk::ImtDesk::CTicketData OnGetTicketRepresentation(
				const sdl::imtdesk::TicketCollectionDocumentService::CGetTicketRepresentationGqlRequest& getTicketRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateTicketFromRepresentation(
				const sdl::imtdesk::TicketCollectionDocumentService::CUpdateTicketFromRepresentationGqlRequest& updateTicketFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtdoc::IDocumentServiceEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_ticketCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_messageCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtchat::IChatService, m_chatServiceCompPtr);
	I_REF(imtchat::IAttachmentStorage, m_attachmentStorageCompPtr);
	I_REF(imtdesk::IEntityReferenceStorage, m_entityReferenceStorageCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_MULTIREF(imtdesk::IEntityTypeProvider, m_entityTypeProvidersCompPtr);
};


} // namespace imtdeskgql

