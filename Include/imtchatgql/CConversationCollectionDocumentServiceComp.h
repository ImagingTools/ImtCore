// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>

// Generated includes
#include <GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ConversationCollectionDocumentService.h>


namespace imtchatgql
{


class CConversationCollectionDocumentServiceComp: public sdl::imtchat::ConversationCollectionDocumentService::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtchat::ConversationCollectionDocumentService::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CConversationCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
	I_END_COMPONENT

protected:
	virtual sdl::imtchat::ImtChat::CConversationData OnGetConversationRepresentation(
				const sdl::imtchat::ConversationCollectionDocumentService::CGetConversationRepresentationGqlRequest& getConversationRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateConversationFromRepresentation(
				const sdl::imtchat::ConversationCollectionDocumentService::CUpdateConversationFromRepresentationGqlRequest& updateConversationFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
};


} // namespace imtchatgql
