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
			PersonInfoEditor,
			"Person info editor",
			"Person Info Editor");

I_EXPORT_COMPONENT(
			AddressEditor,
			"Address editor",
			"Address Editor");

I_EXPORT_COMPONENT(
			AccountInfoEditor,
			"Account info editor",
			"Account Info Editor");

I_EXPORT_COMPONENT(
			PersonInfoViewDelegate,
			"Person info view delegate",
			"Person Address Info View Delegate");


} // namespace ImtAuthGuiPck


