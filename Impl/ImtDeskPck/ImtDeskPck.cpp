// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtDeskPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDeskPck
{


I_EXPORT_PACKAGE(
			"ImtDeskPck",
			"Service Desk domain model component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			SupportTicket,
			"Support ticket component",
			"Support Ticket");


} // namespace ImtDeskPck
