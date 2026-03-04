// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtServerGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtServerGqlPck
{


I_EXPORT_PACKAGE(
			"ImtServerGqlPck",
			"Server GraphQL component package",
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
			"GraphQL-representation data controller",
			"GraphQL Representation Controller Model");

I_EXPORT_COMPONENT(
			UserSettingsController,
			"Client settings controller",
			"GraphQL Controller Model Client User");

I_EXPORT_COMPONENT(
			GqlRepresentationJoiner,
			"Joiner for GraphQL representations",
			"GraphQL Gql Joiner Representation");

I_EXPORT_COMPONENT(
			ApplicationInfoController,
			"Application info controller for representation",
			"Application Controller Representation");

I_EXPORT_COMPONENT(
			GqlContextController,
			"GraphQL context controller",
			"GraphQL Context Controller");

I_EXPORT_COMPONENT(
			ObjectProvider,
			"Object provider",
			"Object Provider");

I_EXPORT_COMPONENT(
			CollectionObjectProvider,
			"Collection object provider",
			"Collection Object Provider");

I_EXPORT_COMPONENT(
			SerializableObjectCollectionController,
			"Serializable object collection Gql controller for gql collection",
			"GqlController Serializable Collection");

I_EXPORT_COMPONENT(
			OperationContextController,
			"Operation context controller",
			"Operation Context Controller");

I_EXPORT_COMPONENT(
			ObjectCollectionChangeNotifier,
			"Subscriprion controller for object collection",
			"Subscriprion Subscriber Collection ObjectCollection");

I_EXPORT_COMPONENT(
			ModelChangeNotifier,
			"Model-based subscription notifier",
			"Model Subscriber Collection");

I_EXPORT_COMPONENT(
			ModelPublisher,
			"Model-based publisher",
			"Model Subscriber Collection");

I_EXPORT_COMPONENT(
			StructureSubscriberController,
			"Subscription controller for collection structure",
			"Subscription Subscriber Collection ObjectCollection Subscriber");

I_EXPORT_COMPONENT(
			BroadcastMessageConsumerController,
			"Gql controller for broadcast message",
			"GraphQl Controller Broadcast Message Consumer");

I_EXPORT_COMPONENT(
			BroadcastMessageConsumerSubscriber,
			"Subscriber controller for broadcast message",
			"GraphQl Subscriber Broadcast Message Consumer");

I_EXPORT_COMPONENT(
			SlaveSubscriberController,
			"Slave subscriber controller",
			"GraphQl Subscriber Controller");

I_EXPORT_COMPONENT(
			GqlSearchController,
			"GraphQL search controller",
			"GraphQL GQL Search Controller");

I_EXPORT_COMPONENT(
			TableViewParamController,
			"Table view param controller",
			"Table View Param Controller");

I_EXPORT_COMPONENT(
			ConnectionStatusSubscriberController,
			"Connection status subscriber controller",
			"Connection Status Subscriber Controller");

I_EXPORT_COMPONENT(
			HeadersDataProvider,
			"Headers data provider",
			"Headers Data Provider");

I_EXPORT_COMPONENT(
			DocumentRevisionController,
			"Document revision controller",
			"Document Revision Controller");

I_EXPORT_COMPONENT(
			CommandsController,
			"Commands controller",
			"Commands Controller");

I_EXPORT_COMPONENT(
			SettingsController,
			"Settings controller",
			"Settings Controller");

I_EXPORT_COMPONENT(
			ProgressManagerPublisher,
			"GraphQL Progress manager publisher",
			"GraphQL Progress Publisher");

I_EXPORT_COMPONENT(
			GqlCollectionImportController,
			"GraphQL Collection import controller",
			"GraphQL Collection Import Controller");

I_EXPORT_COMPONENT(
			CollectionImportController,
			"Collection import controller",
			"Collection Import Controller");

I_EXPORT_COMPONENT(
			CollectionImportJobParamsFiller,
			"Collection import job params filler",
			"Collection Import Job Params Filler");

I_EXPORT_COMPONENT(
			CollectionDocumentManagerController,
			"Server-side document manager based on document collection",
			"Collection Document Manager");

I_EXPORT_COMPONENT(
			CollectionDocumentManagerPublisher,
			"Collection document manager publisher",
			"Collection Document Manager Publisher");

I_EXPORT_COMPONENT(
			GqlContextCreator,
			"GraphQL context creator",
			"GraphQL GQL Context Creator");

\
} // namespace ImtServerGqlPck


