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
			FeatureDatabaseDelegate,
			"Feature object delegate for SQL table",
			"SQL Feature Package License Delegate");

I_EXPORT_COMPONENT(
			ProductDatabaseDelegate,
			"Product object delegate for SQL table",
			"SQL Product License Delegate");

I_EXPORT_COMPONENT(
			LicenseDatabaseDelegate,
			"Product license object delegate for SQL table",
			"Product License Delegate");


} // namespace ImtLicenseDbPck


