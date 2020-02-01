#include "QHelpGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace QHelpGuiPck
{


I_EXPORT_PACKAGE(
			"QHelpGuiPck",
			"ImagingTools help component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			HtmlHelpGui2,
			"Help GUI showing html help document",
			"Help Viewer Document Html Description" IM_TAG("Help"));


} // namespace QHelpGuiPck


