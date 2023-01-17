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
			GraphQLRemoteRepresentationController,
			"GraphQL remote representation controller",
			"GraphQL Gql Api Client Controller User");

I_EXPORT_COMPONENT(
			GraphQLClientEngine,
			"GraphQLClientEngine",
			"GraphQL Client Engine");

I_EXPORT_COMPONENT(
			GqlRepresentationDataController,
			"Gql representation data controller",
			"GraphQL Representation Controller Model");

I_EXPORT_COMPONENT(
			UserSettingsController,
			"Client settings controller",
			"GraphQL Gql Controller Model Client User");

I_EXPORT_COMPONENT(
			GqlRepresentationJoiner,
			"Joiner for GraphQL representations",
			"GraphQL Gql Joiner Representation");

I_EXPORT_COMPONENT(
			ApplicationInfoController,
			"Application info controller for representation",
			"Application Controller Representation");


} // namespace ImtGqlPck


