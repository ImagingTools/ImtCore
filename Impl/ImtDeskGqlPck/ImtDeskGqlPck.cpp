// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtDeskGqlPck.h"


// ACF includes
#include <icomp/export.h>


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
			TicketNotificationProducer,
			"Sends the notifications of the tickets (new messages, new assignees) to the notification center",
			"Ticket Notification Producer");


} // namespace ImtDeskGqlPck
