#include "ImtAppPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAppPck
{


I_EXPORT_PACKAGE(
			"ImtAppPck",
			"ImagingTools core framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			BackupSettings,
			"Backup settings",
			"Backup Settings");

I_EXPORT_COMPONENT(
			BackupSettingsRepresentationController,
			"Backup settings representation controller",
			"Backup Settings Representation Controller");


} // namespace ImtAppPck


