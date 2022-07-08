#include "ImtGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGqlPck
{


I_EXPORT_PACKAGE(
			"ImtGqlPck",
			"GraphQL component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			ApiClient,
			"General API client",
			"GraphQL API Client");

I_EXPORT_COMPONENT(
			GraphQLServlet,
			"GraphQLServlet",
			"GraphQL Receiver");

I_EXPORT_COMPONENT(
			GraphQLDemultiplexer,
			"GraphQLDemultiplexer",
			"GraphQL Demultiplexer");


} // namespace ImtGqlPck


