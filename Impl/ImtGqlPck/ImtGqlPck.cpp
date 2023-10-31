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
			RemoteGqlContextController,
			"Remote gql context controller",
			"Remote Gql Context Controller");

I_EXPORT_COMPONENT(
			GqlContextController,
			"Gql context controller",
			"Gql Context Controller");

I_EXPORT_COMPONENT(
			ObjectProvider,
			"Object provider",
			"Object Provider");

I_EXPORT_COMPONENT(
			CollectionObjectProvider,
			"Collection object provider",
			"Collection Object Provider");

I_EXPORT_COMPONENT(
			GqlController,
			"Simple GQL controller",
			"GqlController");

I_EXPORT_COMPONENT(
			SerializableObjectCollectionController,
			"Serializable object collection Gql controller for gql collection",
			"GqlController Serializable Collection");

I_EXPORT_COMPONENT(
			OperationContextController,
			"Operation context controller",
			"Operation Context Controller");

I_EXPORT_COMPONENT(
			DelegatedRequestHandler,
			"Delegate for slave request handler",
			"Delegate Request Handler");

I_EXPORT_COMPONENT(
			ObjectCollectionSubscriberController,
			"Subscriprion controller for object collection",
			"Subscriprion Subscriber Collection ObjectCollection");


} // namespace ImtGqlPck


