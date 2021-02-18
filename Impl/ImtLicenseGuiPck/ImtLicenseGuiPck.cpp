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
			"License Info");

I_EXPORT_COMPONENT(
			LicenseManagerViewDelegate,
			"License manager view delegate",
			"License Manager View Delegate");


} // namespace ImtLicenseGuiPck


