// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
			AccountController,
			"AccountController",
			"Account Controller");

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

I_EXPORT_COMPONENT(
			FeatureDependenciesProvider,
			"Feature dependencies provider",
			"Feature Dependencies Provider");

I_EXPORT_COMPONENT(
			PermissionsProvider,
			"Permissions provider",
			"Feature Dependencies Provider Permissions");

I_EXPORT_COMPONENT(
			ProductInfoProvider,
			"Product info provider",
			"Product Info Provider");

I_EXPORT_COMPONENT(
			FeatureMetaInfoDelegate,
			"Feature meta info delegate",
			"Feature Meta Info Delegate");

I_EXPORT_COMPONENT(
			ProductMetaInfoDelegate,
			"Product meta info delegate",
			"Product Meta Info Delegate");

I_EXPORT_COMPONENT(
			LicenseMetaInfoDelegate,
			"License meta info delegate",
			"License Meta Info Delegate");

I_EXPORT_COMPONENT(
			ProductInfoFileGenerator,
			"Product info file generator",
			"Product Info File Generator");


} // namespace ImtLicenseGqlPck


