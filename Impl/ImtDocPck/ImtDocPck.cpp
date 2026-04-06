// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtDocPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDocPck
{


I_EXPORT_PACKAGE(
			"ImtDocPck",
			"Document management component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			CollectionDocumentManager,
			"Collection-based document manager",
			"Collection Document");


} // namespace ImtDocPck


