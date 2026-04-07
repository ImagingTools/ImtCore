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


} // namespace ImtDeskGqlPck
