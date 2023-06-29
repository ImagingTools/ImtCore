#include "ImtComPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtComPck
{


I_EXPORT_PACKAGE(
			"ImtComPck",
			"ImagingTools communication framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			InternetConnectionChecker,
			"Internet connection checker",
			"Connection Internet");

I_EXPORT_COMPONENT(
			SimpleConnectionChecker,
			"Simple internet connection checker",
			"Simple Internet Connection Checker");


} // namespace ImtComPck


