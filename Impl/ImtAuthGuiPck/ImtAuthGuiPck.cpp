#include "ImtAuthGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAuthGuiPck
{


I_EXPORT_PACKAGE(
			"ImtAuthGuiPck",
			"ImagingTools license management UI-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			ContactInfoEditor,
			"Contact info editor",
			"Contact Info Editor");

I_EXPORT_COMPONENT(
			AddressEditor,
			"Address editor",
			"Address Editor");

I_EXPORT_COMPONENT(
			AccountInfoEditor,
			"Account info editor",
			"Account Info Editor");

I_EXPORT_COMPONENT(
			ContactInfoViewDelegate,
			"Contact info view delegate",
			"Contact Address Info View Delegate");

I_EXPORT_COMPONENT(
			AccountInfoViewDelegate,
			"Account info view delegate",
			"Account Info View Delegate");

I_EXPORT_COMPONENT(
			DesignManager,
			"Design Manager",
			"Style Design Manager");


} // namespace ImtAuthGuiPck


