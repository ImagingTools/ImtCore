// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// forward declarations
namespace imtdeskgql
{
class CTicketCollectionControllerComp;
class CTicketCollectionDocumentServiceComp;
class CEntityContextTicketsControllerComp;
class CTicketMessageNotifierComp;
class CTicketAssigneeNotifierComp;
} // namespace imtdeskgql


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
