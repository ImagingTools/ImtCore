// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtDeskDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDeskDbPck
{


I_EXPORT_PACKAGE(
			"ImtDeskDbPck",
			"Service Desk database delegate component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			SupportTicketDbDelegate,
			"Support ticket database delegate",
			"Support Ticket Database Delegate");

I_EXPORT_COMPONENT(
			EntityReferenceStorage,
			"Entity reference storage",
			"Entity Reference Storage");


} // namespace ImtDeskDbPck
