// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>

// Generated includes
#include <GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ConversationCollectionDocumentManager.h>


namespace imtchatgql
{


class CConversationCollectionDocumentManagerComp: public sdl::imtchat::ConversationCollectionDocumentManager::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtchat::ConversationCollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CConversationCollectionDocumentManagerComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentManager", "Collection document manager", false, "CollectionDocumentManager");
	I_END_COMPONENT

protected:
	virtual sdl::imtchat::ImtChat::CConversationData OnGetConversationRepresentation(
				const sdl::imtchat::ConversationCollectionDocumentManager::CGetConversationRepresentationGqlRequest& getConversationRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnUpdateConversationFromRepresentation(
				const sdl::imtchat::ConversationCollectionDocumentManager::CUpdateConversationFromRepresentationGqlRequest& updateConversationFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
};


} // namespace imtchatgql
