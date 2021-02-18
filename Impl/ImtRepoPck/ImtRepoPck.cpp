#include "ImtRepoPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtRepoPck
{


I_EXPORT_PACKAGE(
			"ImtRepoPck",
			"ImagingTools core framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			FileCollection,
			"File-based container of the objects",
			"Object Collection File Data Model Repository");

I_EXPORT_COMPONENT(
			FileTransformationController,
			"Repository file transformation controller",
			"Object Collection File Data Model Repository Revision Format");

I_EXPORT_COMPONENT(
			RepositoryFileTransformationCompBase,
			"Base repository file transformation",
			"Collection File Data Model Repository Revision Format Tanformation");


} // namespace ImtRepoPck


