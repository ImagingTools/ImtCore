// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentManager.h>
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
};


} // namespace imtdeskgql


