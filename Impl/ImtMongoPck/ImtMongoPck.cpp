// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtMongoPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtMongoPck
{


I_EXPORT_PACKAGE(
			"ImtDatabasePck",
			"Database-related component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			MongoDatabaseCollection,
			"Mongo-based object collection",
			"Mongo Database");

I_EXPORT_COMPONENT(
			MongoDatabaseEngine,
			"Mongo-based database engine",
			"Mongo Database");


} // namespace ImtMongoPck


