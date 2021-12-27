#include "ImtLicenseGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtLicenseGqlPck
{


I_EXPORT_PACKAGE(
			"ImtLicenseGqlPck",
			"ImagingTools license management Gql-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			FeaturePackageCollectionController,
			"FeaturePackageCollectionController",
			"Feature Package Collection Controller");

I_EXPORT_COMPONENT(
			ProductCollectionController,
			"ProductCollectionController",
			"Product Collection Controller");

I_EXPORT_COMPONENT(
			AccountCollectionController,
			"AccountCollectionController",
			"Account Collection Controller");

I_EXPORT_COMPONENT(
			InstallationCollectionController,
			"InstallationCollectionController",
			"Installation Collection Controller");

} // namespace ImtLicenseGuiPck


