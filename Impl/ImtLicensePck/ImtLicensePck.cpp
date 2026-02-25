// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtLicensePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtLicensePck
{


I_EXPORT_PACKAGE(
			"ImtLicensePck",
			"ImagingTools license management component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			FeatureInfo,
			"Feature information",
			"License Feature Information");

I_EXPORT_COMPONENT(
			IdentifiableFeatureInfo,
			"Identifiable Feature information",
			"Identifiable License Feature Information");

I_EXPORT_COMPONENT(
			LicenseInfo,
			"Single license information",
			"License Information");

I_EXPORT_COMPONENT(
			IdentifiableLicenseInfo,
			"Identifiable Single license information",
			"Identifiable License Information");

I_EXPORT_COMPONENT(
			LicenseInfoManager,
			"Management of licenses",
			"License Manager Rights Dongle Featuring Container Collection");

I_EXPORT_COMPONENT(
			LicenseBasedRightsProvider,
			"License based rights provider",
			"License Rights Dongle Featuring Licensing Key");

I_EXPORT_COMPONENT(
			ProductLicensingInfo,
			"Definition of the licensing model of a product",
			"License Product");

I_EXPORT_COMPONENT(
			ProductCollection,
			"Definition of all product licenses",
			"License Products System");

I_EXPORT_COMPONENT(
			ProductInstanceInfo,
			"Information about an installed instance of the product",
			"License Product Instance Setup Info Installation");

I_EXPORT_COMPONENT(
			ProductInstanceCollection,
			"List of all installed product instances",
			"License Products Installations");

I_EXPORT_COMPONENT(
			ProductLicensingInfoMetaInfoCreator,
			"Product licensing info metainfo creator",
			"License Product Metainfo Creator");

I_EXPORT_COMPONENT(
			ProductInstanceMetaInfoCreator,
			"Product instance metainfo creator",
			"License Product Instance Metainfo Creator");

I_EXPORT_COMPONENT(
			LicenseController,
			"Client-side license controller",
			"License Product Instance Controller");

I_EXPORT_COMPONENT(
			FeatureInfoProvider,
			"Feature info provider",
			"Feature Info Provider");

I_EXPORT_COMPONENT(
			ProductInfo,
			"Product info",
			"Product License Feature");

I_EXPORT_COMPONENT(
			IdentifiableProductInfo,
			"Identifiable Product info",
			"Identifiable Product License Feature");

I_EXPORT_COMPONENT(
			FeatureContainer,
			"Feature container",
			"Feature Container Subfeature");

I_EXPORT_COMPONENT(
			ProductController,
			"Product controller",
			"Product Controller");

I_EXPORT_COMPONENT(
			FeatureMetaInfoCreator,
			"Feature meta info creator",
			"Feature Meta Info Creator");

I_EXPORT_COMPONENT(
			LicenseMetaInfoCreator,
			"License meta info creator",
			"License Meta Info Creator");

I_EXPORT_COMPONENT(
			ProductMetaInfoCreator,
			"Product meta info creator",
			"Product Meta Info Creator");


} // namespace ImtLicensePck


