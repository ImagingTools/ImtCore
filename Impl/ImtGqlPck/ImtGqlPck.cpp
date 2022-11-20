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

I_EXPORT_COMPONENT(
            SingleCommandPermissionsProvider,
            "SingleCommandPermissionsProvider",
            "Single Command Permissions Provider");

I_EXPORT_COMPONENT(
			CommandPermissionsProviderJoiner,
			"CommandPermissionsProviderJoiner",
			"Command Permissions Provider Joiner Comp");

I_EXPORT_COMPONENT(
			PermissionsController,
			"PermissionsController",
			"Permissions Controller");



} // namespace ImtGqlPck


