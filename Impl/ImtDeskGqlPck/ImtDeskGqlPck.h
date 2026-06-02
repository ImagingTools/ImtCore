// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// SDL schema includes (for GqlHandlerCompBase definitions)
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/ImtDesk.h>
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/TicketCollectionDocumentService.h>

// ImtCore includes
#include <imtdeskgql/CTicketCollectionControllerComp.h>
#include <imtdeskgql/CTicketCollectionDocumentServiceComp.h>
#include <imtdeskgql/CEntityContextTicketsControllerComp.h>
#include <imtdeskgql/CTicketMessageNotifierComp.h>
#include <imtdeskgql/CTicketAssigneeNotifierComp.h>


/**
	ImtDeskGqlPck package
*/
namespace ImtDeskGqlPck
{


typedef imtdeskgql::CTicketCollectionControllerComp TicketCollectionController;
typedef imtdeskgql::CTicketCollectionDocumentServiceComp TicketCollectionDocumentService;
typedef imtdeskgql::CEntityContextTicketsControllerComp EntityContextTicketsController;
typedef imtdeskgql::CTicketMessageNotifierComp TicketMessageNotifier;
typedef imtdeskgql::CTicketAssigneeNotifierComp TicketAssigneeNotifier;


} // namespace ImtDeskGqlPck
