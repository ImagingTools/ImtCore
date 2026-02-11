// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtGuiGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGuiGqlPck
{


I_EXPORT_PACKAGE(
			"ImtGuiGqlPck",
			"ImagingTools core framework GraphQL-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "GraphQL");

I_EXPORT_COMPONENT(
			GetStyleDataController,
			"Controller providing application style in GraphQL representation",
			"Style Data Controller" IM_AUTHOR("\"Viktor Shcherbina\""));


} // namespace ImtGuiGqlPck


