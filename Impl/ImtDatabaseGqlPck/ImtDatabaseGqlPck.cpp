// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtDatabaseGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDatabaseGqlPck
{


I_EXPORT_PACKAGE(
			"ImtDatabaseGqlPck",
			"GraphQL component package with database implementation",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			StructureController,
			"GraphQL-structure base controller",
			"GQL GraphQL Structure Controller");


} // namespace ImtDatabaseGqlPck


