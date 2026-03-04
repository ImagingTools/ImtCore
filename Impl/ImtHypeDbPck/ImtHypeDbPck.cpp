// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtHypeDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtHypeDbPck
{


I_EXPORT_PACKAGE(
			"ImtHypeDbPck",
			"Database-related job processing component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			JobTicketDatabaseDelegate,
			"Job ticket database delegate",
			"Job Ticket Database Delegate");


} // namespace ImtHypeDbPck


