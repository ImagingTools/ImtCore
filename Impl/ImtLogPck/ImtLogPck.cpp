#include "ImtLogPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtLogPck
{


I_EXPORT_PACKAGE(
			"ImtLogPck",
			"ImagingTools log framework component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			LoginEventController,
			"Login event controller component",
			"Login Event Controller");

I_EXPORT_COMPONENT(
			MessageGroupInfoProvider,
			"Information provider for messages",
			"Message Information Provider");


} // namespace ImtLogPck


