#include "ImtLicenseGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtLicenseGuiPck
{


I_EXPORT_PACKAGE(
			"ImtLicenseGuiPck",
			"ImagingTools license management UI-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			LicenseInfoEditorGui,
			"License info editor",
			"License Info Editor");

I_EXPORT_COMPONENT(
			LicenseManagerViewDelegate,
			"License manager view delegate",
			"License Manager View Delegate");

I_EXPORT_COMPONENT(
			ProductLicensingInfoViewDelegate,
			"Product licensing info delegate",
			"Product Licensing Info View Collection Delegate");

I_EXPORT_COMPONENT(
			ProductInstanceInfoViewDelegate,
			"Product instance info view delegate",
			"Product Licensing Instance Info View Collection Delegate");

I_EXPORT_COMPONENT(
			FeatureCollectionViewDelegate,
			"Feature collection view delegate",
			"Feature Licensing Collection View Delegate");

I_EXPORT_COMPONENT(
			FeatureInfoEditorGui,
			"Feature info editor",
			"Feature Licensing Info Collection View Editor");

I_EXPORT_COMPONENT(
			ProductInstanceInfoEditor,
			"Product instance editor",
			"Licensing Product Instance Installation");


} // namespace ImtLicenseGuiPck


