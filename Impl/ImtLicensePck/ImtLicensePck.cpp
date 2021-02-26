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
			FeatureInfoProvider,
			"Feature info provider",
			"License Feature Container Collection Info Provider");

I_EXPORT_COMPONENT(
			LicenseInfo,
			"Single license information",
			"License Information");

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
			ProductInstanceCollection,
			"List of all installed product instances",
			"License Products Installations");

I_EXPORT_COMPONENT(
			ProductLicensingInfoMetaInfoCreator,
			"Product licensing info metainfo creator",
			"License Product Metainfo Creator");


} // namespace ImtLicensePck


