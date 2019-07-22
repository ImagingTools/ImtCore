#include "ImtGuiPck.h"


// ACF includes
#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imtgui);


namespace ImtGuiPck
{


I_EXPORT_PACKAGE("ImtGuiPck", "ImagingTools standard UI-component package", "GUI UI UX View");

I_EXPORT_COMPONENT(
			ThumbnailDecoratorGui,
			"Thumbnail selection page view component",
			"Menu Thumbnail Selection Page");

I_EXPORT_COMPONENT(
			SingleStatusProviderGui,
			"Single information status provider GUI component",
			"Single Information Status Provider");

I_EXPORT_COMPONENT(
			MultiStatusProviderGui,
			"Multiple information status provider GUI component",
			"Multiple Information Status Provider");


} // namespace ImtGuiPck


