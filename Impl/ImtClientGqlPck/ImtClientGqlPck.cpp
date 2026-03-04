// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
			ModelSubscriber,
			"Subscription controller",
			"Subscription Controller");

I_EXPORT_COMPONENT(
			GqlHierarchicalStructureDelegate,
			"GraphQl structure delegate",
			"GraphQl Hierarchical Structure Delegate");

I_EXPORT_COMPONENT(
			ConnectionStatusSubscriber,
			"Connection status subscriber",
			"Connection Status Subscriber");

I_EXPORT_COMPONENT(
			RemoteDocumentRevisionController,
			"Remote document revision controller",
			"Remote Document Revision Controller");

I_EXPORT_COMPONENT(
			RemoteObjectCollectionController,
			"Remote object collection controller",
			"Remote Object Collection Controller");

I_EXPORT_COMPONENT(
			ObjectCollectionSubscriber,
			"Object collection subscriber",
			"Object Collection Subscriber");

I_EXPORT_COMPONENT(
			RemoteCommandsController,
			"Remote commands controller",
			"Remote Commands Controller");


} // namespace ImtClientGqlPck


