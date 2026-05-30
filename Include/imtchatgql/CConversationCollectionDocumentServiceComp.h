// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService_fwd.h>

// Generated includes
#include <GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ConversationCollectionDocumentService_fwd.h>


namespace imtchatgql
{


class CConversationCollectionDocumentServiceComp: public sdl::V1_0::imtchat::CConversationCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtchat::CConversationCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CConversationCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
	I_END_COMPONENT

protected:
	virtual sdl::V1_0::imtchat::CConversationData OnGetConversationRepresentation(
				const sdl::V1_0::imtchat::CGetConversationRepresentationGqlRequest& getConversationRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateConversationFromRepresentation(
				const sdl::V1_0::imtchat::CUpdateConversationFromRepresentationGqlRequest& updateConversationFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
};


} // namespace imtchatgql
