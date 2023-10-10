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
		ProductCollectionController,
		"ProductCollectionController",
		"Product Collection Controller");


I_EXPORT_COMPONENT(
		ProductController,
		"ProductController",
		"Product Controller");


I_EXPORT_COMPONENT(
		LicenseController,
		"LicenseController",
		"Product License Controller");


I_EXPORT_COMPONENT(
		LicenseCollectionController,
		"LicenseCollectionController",
		"Product License Collection Controller");


I_EXPORT_COMPONENT(
		AccountCollectionController,
		"AccountCollectionController",
		"Account Collection Controller");


I_EXPORT_COMPONENT(
		AccountController,
		"AccountController",
		"Account Controller");


I_EXPORT_COMPONENT(
		FeaturesExportProvider,
		"FeaturesExportProvider",
		"Features Export Provider");


I_EXPORT_COMPONENT(
		ProductPermissionController,
		"ProductPermissionController",
		"Product Permission Controller");

I_EXPORT_COMPONENT(
		FeatureController,
		"FeatureController",
		"Feature Controller");

I_EXPORT_COMPONENT(
		FeatureCollectionController,
		"FeatureCollectionController",
		"Feature Collection Controller");


} // namespace ImtLicenseGqlPck


