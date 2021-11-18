#include "ImtLicenseDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtLicenseDbPck
{


I_EXPORT_PACKAGE(
			"ImtLicenseDbPck",
			"Database-related license component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
	FeaturePackageDatabaseDelegate,
			"Feature package object delegate for SQL table",
			"SQL Feature Package License Delegate");


} // namespace ImtLicenseDbPck


