#include "ImtGuiGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGuiGqlPck
{


I_EXPORT_PACKAGE(
			"ImtGuiGqlPck",
			"ImagingTools core framework Gql-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			PagesDataController,
			"Pages data controller",
			"Pages Data Controller");

I_EXPORT_COMPONENT(
			CommandsDataController,
			"Commands data controller",
			"Commands Data Controller");


I_EXPORT_COMPONENT(
			GetStyleDataController,
			"Get Style data controller",
			"Style Data Controller");


} // namespace ImtGuiGqlPck


