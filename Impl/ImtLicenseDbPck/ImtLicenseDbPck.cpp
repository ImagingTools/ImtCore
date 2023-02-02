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

I_EXPORT_COMPONENT(
			ProductDatabaseDelegate,
			"Product object delegate for SQL table",
			"SQL Product License Delegate");

I_EXPORT_COMPONENT(
			AccountDatabaseDelegate,
			"Account info object for SQL table",
			"SQL Account Address Contact Delegate");

I_EXPORT_COMPONENT(
			ProductInstanceDatabaseDelegate,
			"Product instance object delegate for SQL table",
			"SQL Product License Instance Installation Delegate");

I_EXPORT_COMPONENT(
            CompanyDatabaseDelegate,
            "Company info object for SQL table",
            "SQL Company Address Contact Delegate");

} // namespace ImtLicenseDbPck


