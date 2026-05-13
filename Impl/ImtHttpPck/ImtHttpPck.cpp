// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtHttpPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtHttpPck
{


I_EXPORT_PACKAGE(
			"ImtHttpPck",
			"Http component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			HttpFileTransfer,
			"Component for transfering files over http",
			"File Http Transfer");


} // namespace ImtHttpPck


