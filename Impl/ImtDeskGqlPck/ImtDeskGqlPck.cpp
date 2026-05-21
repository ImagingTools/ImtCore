// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtDeskGqlPck.h"


// ACF includes
#include <icomp/export.h>

// ImtCore includes
#include <imtdeskgql/CTicketCollectionControllerComp.h>
#include <imtdeskgql/CTicketCollectionDocumentServiceComp.h>
#include <imtdeskgql/CEntityContextTicketsControllerComp.h>
#include <imtdeskgql/CTicketMessageNotifierComp.h>
#include <imtdeskgql/CTicketAssigneeNotifierComp.h>


namespace ImtDeskGqlPck
{


I_EXPORT_PACKAGE(
			"ImtDeskGqlPck",
			"Service Desk GraphQL controller component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "GraphQL");

I_EXPORT_COMPONENT(
			TicketCollectionController,
			"Ticket collection controller",
			"Ticket Collection Controller");

I_EXPORT_COMPONENT(
			TicketCollectionDocumentService,
			"Ticket collection document manager",
			"Ticket Collection Document Manager");

I_EXPORT_COMPONENT(
			EntityContextTicketsController,
			"Entity context tickets GraphQL controller",
			"Entity Context Tickets Controller");

I_EXPORT_COMPONENT(
			TicketMessageNotifier,
			"Ticket message subscription publisher (notifies ticket reporter/assignees about new messages)",
			"Ticket Message Notifier");

I_EXPORT_COMPONENT(
			TicketAssigneeNotifier,
			"Ticket assignee change subscription publisher (notifies newly added assignees)",
			"Ticket Assignee Notifier");


} // namespace ImtDeskGqlPck
