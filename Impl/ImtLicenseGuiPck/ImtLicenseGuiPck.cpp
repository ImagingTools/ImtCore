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
			ProductLicensingInfoGui,
			"Product License Info Editor",
			"Product License Info Editor");

I_EXPORT_COMPONENT(
			LicenseManagerViewDelegate,
			"License manager view delegate",
			"License Manager View Delegate");

I_EXPORT_COMPONENT(
			ProductLicensingInfoCollectionViewDelegate,
			"Product licensing info collection delegate",
			"Product Licensing Info Collection Delegate");


} // namespace ImtLicenseGuiPck


