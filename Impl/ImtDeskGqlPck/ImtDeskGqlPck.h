// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdeskgql/CTicketCollectionControllerComp.h>
#include <imtdeskgql/CTicketCollectionDocumentManagerComp.h>
#include <imtdeskgql/CEntityContextTicketsControllerComp.h>
#include <imtdeskgql/CTicketMessageNotifierComp.h>
#include <imtdeskgql/CTicketAssigneeNotifierComp.h>


/**
	ImtDeskGqlPck package
*/
namespace ImtDeskGqlPck
{


typedef imtdeskgql::CTicketCollectionControllerComp TicketCollectionController;
typedef imtdeskgql::CTicketCollectionDocumentManagerComp TicketCollectionDocumentManager;
typedef imtdeskgql::CEntityContextTicketsControllerComp EntityContextTicketsController;
typedef imtdeskgql::CTicketMessageNotifierComp TicketMessageNotifier;
typedef imtdeskgql::CTicketAssigneeNotifierComp TicketAssigneeNotifier;


} // namespace ImtDeskGqlPck
