#include "ImtDatabaseGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDatabaseGuiPck
{


I_EXPORT_PACKAGE(
			"ImtDatabaseGuiPck",
			"Database-related UI-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			DatabaseLoginSettingsEditor,
			"Editor for the database access settings",
			"Database Settings Access Login User");


} // namespace ImtDatabaseGuiPck


