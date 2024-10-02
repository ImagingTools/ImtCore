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

I_EXPORT_COMPONENT(
			RemoteDatabaseServerConnection,
			"Remote database server connection",
			"Remote Database Server Connection");

I_EXPORT_COMPONENT(
			DatabaseServerConnectionController,
			"Database server connection controller",
			"Database Server Connection Controller");


} // namespace ImtDatabaseGqlPck


