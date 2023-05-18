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


