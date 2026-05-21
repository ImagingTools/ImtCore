// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes (full includes required for TModelCompWrap template instantiation)
#include <imtclientgql/CGqlObjectCollectionComp.h>
#include <imtclientgql/CConnectionStatusSubscriberComp.h>


// forward declarations
namespace imtclientgql
{
class CApiClientComp;
class CGqlClientEngineComp;
class CGqlObjectCollectionDelegateComp;
class CGqlRemoteRepresentationControllerCompBase;
class CGqlRemoteRepresentationControllerComp;
class CSubscriptionManagerComp;
class CWebSocketClientComp;
class CModelSubscriberComp;
class CGqlHierarchicalStructureDelegateComp;
class CRemoteDocumentRevisionControllerComp;
class CRemoteObjectCollectionControllerComp;
class CObjectCollectionSubscriberComp;
class CRemoteCollectionDocumentServiceControllerComp;
class CRemoteCommandsControllerComp;
class CRemoteFilterableSelectControllerComp;
} // namespace imtclientgql


/**
	ImtClientGqlPck package
*/
namespace ImtClientGqlPck
{


typedef imtclientgql::CApiClientComp ApiClient;
typedef imtclientgql::CGqlClientEngineComp GraphQLClientEngine;
typedef imtclientgql::CGqlObjectCollectionDelegateComp GqlObjectCollectionDelegate;
typedef icomp::TModelCompWrap<imtclientgql::CGqlObjectCollectionComp> GqlObjectCollection;
typedef imtclientgql::CGqlRemoteRepresentationControllerCompBase GraphQLRemoteRepresentationController;
typedef imtclientgql::CGqlRemoteRepresentationControllerComp PumaGqlRemoteRepresentationController;
typedef imtclientgql::CSubscriptionManagerComp SubscriptionManager;
typedef imtclientgql::CWebSocketClientComp WebSocketClient;
typedef imtclientgql::CModelSubscriberComp ModelSubscriber;
typedef imtclientgql::CGqlHierarchicalStructureDelegateComp GqlHierarchicalStructureDelegate;
typedef icomp::TModelCompWrap<imtclientgql::CConnectionStatusSubscriberComp> ConnectionStatusSubscriber;
typedef imtclientgql::CRemoteDocumentRevisionControllerComp RemoteDocumentRevisionController;
typedef imtclientgql::CRemoteObjectCollectionControllerComp RemoteObjectCollectionController;
typedef imtclientgql::CRemoteCollectionDocumentServiceControllerComp RemoteCollectionDocumentServiceController;
typedef imtclientgql::CObjectCollectionSubscriberComp ObjectCollectionSubscriber;
typedef imtclientgql::CRemoteCommandsControllerComp RemoteCommandsController;
typedef imtclientgql::CRemoteFilterableSelectControllerComp RemoteFilterableSelectController;


} // namespace ImtClientGqlPck

