#include "ImtDocPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDocPck
{


I_EXPORT_PACKAGE(
			"ImtDocPck",
			"Document management component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			CollectionDocumentManager,
			"Collection-based document manager",
			"Collection Document");


} // namespace ImtDocPck


