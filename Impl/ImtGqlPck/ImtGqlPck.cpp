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
			"GraphQL servlet",
			"GraphQL Receiver");

I_EXPORT_COMPONENT(
			WebSocketServlet,
			"WebSocket servlet",
			"GraphQL WebSocket Receiver");

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

I_EXPORT_COMPONENT(
			GqlObjectCollection,
			"GraphQl based object collection",
			"GraphQl Gql Object Collection");

I_EXPORT_COMPONENT(
			GqlContextCacheController,
			"GraphQl context cache controller",
			"GraphQl Gql GQL Object Context Cache Controller");

I_EXPORT_COMPONENT(
			RemoteGqlContextController,
			"Remote gql context controller",
			"Remote Gql Context Controller");

I_EXPORT_COMPONENT(
			GqlContextController,
			"Gql context controller",
			"Gql Context Controller");

I_EXPORT_COMPONENT(
			PumaGqlRemoteRepresentationController,
			"Puma GraphQL remote representation controller",
			"Puma GraphQL Gql Api Client Controller User");

I_EXPORT_COMPONENT(
			ObjectProvider,
			"Object provider",
			"Object Provider");

I_EXPORT_COMPONENT(
			CollectionObjectProvider,
			"Collection object provider",
			"Collection Object Provider");


} // namespace ImtGqlPck


