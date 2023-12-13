#include "ImtClientGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtClientGqlPck
{


I_EXPORT_PACKAGE(
			"ImtClientGqlPck",
			"Client GraphQL component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			ApiClient,
			"General API client",
			"GraphQL API Client");

I_EXPORT_COMPONENT(
			GraphQLClientEngine,
			"GraphQLClientEngine",
			"GraphQL Client Engine");

I_EXPORT_COMPONENT(
			GqlObjectCollectionDelegate,
			"GraphQl based delegate for object collection",
			"GraphQl Gql Object Collection Delegate");

I_EXPORT_COMPONENT(
			GqlObjectCollection,
			"GraphQl based object collection",
			"GraphQl Gql Object Collection");

I_EXPORT_COMPONENT(
			GraphQLRemoteRepresentationController,
			"GraphQL remote representation controller",
			"GraphQL Gql Api Client Controller User");

I_EXPORT_COMPONENT(
			PumaGqlRemoteRepresentationController,
			"Puma GraphQL remote representation controller",
			"Puma GraphQL Gql Api Client Controller User");

I_EXPORT_COMPONENT(
			SubscriptionManager,
			"Subscription manager for a client",
			"GraphQl Service Subscription  Manager");

I_EXPORT_COMPONENT(
		WebSocketClient,
		"The client for 'CWebSocketServer'",
		"Rest WebSocket Client");

I_EXPORT_COMPONENT(
		SubscriptionController,
		"Subscription controller",
		"Subscription Controller");

I_EXPORT_COMPONENT(
        GqlStructureDelegate,
        "GraphQl structure delegate",
        "GraphQl Structure Delegate");


} // namespace ImtClientGqlPck


