// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtStylePck.h"


// ACF includes
#include <icomp/export.h>

I_REGISTER_QT_RESOURCE(imtstylelight);

namespace ImtStylePck
{


I_EXPORT_PACKAGE(
			"ImtStylePck",
			"Design style package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			DesignTokenBasedResourceProvider,
			"Design token based resource provider",
			"Design Token Resource Color Palette Font Icons");

I_EXPORT_COMPONENT(
			DesignManager,
			"Design Manager",
			"Style Design Manager");


} // namespace ImtStylePck


