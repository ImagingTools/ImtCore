#include "ImtGeoPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGeoPck
{


I_EXPORT_PACKAGE(
			"ImtGeoPck",
			"Geo component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			AddressElementInfo,
			"Address element",
			"Address element");

I_EXPORT_COMPONENT(
            AddressElementDatabaseDelegate,
            "Address element info object for SQL table",
            "SQL Address Element Delegate");


} // namespace ImtGeoPck


