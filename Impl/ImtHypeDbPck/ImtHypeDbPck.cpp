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


