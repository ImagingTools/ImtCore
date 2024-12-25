#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtclientgql/CApiClientComp.h>
#include <imtclientgql/CGqlClientEngineComp.h>
#include <imtclientgql/CGqlObjectCollectionComp.h>
#include <imtclientgql/CGqlObjectCollectionDelegateComp.h>
#include <imtclientgql/CGqlRemoteRepresentationControllerCompBase.h>
#include <imtclientgql/CGqlRemoteRepresentationControllerComp.h>
#include <imtclientgql/CSubscriptionManagerComp.h>
#include <imtclientgql/CWebSocketClientComp.h>
#include <imtclientgql/CModelSubscriberComp.h>
#include <imtclientgql/CGqlHierarchicalStructureDelegateComp.h>
#include <imtclientgql/CGqlObjectCollectionDelegateComp.h>
#include <imtclientgql/CConnectionStatusSubscriberComp.h>
#include <imtclientgql/CRemoteDocumentRevisionControllerComp.h>
#include <imtclientgql/CRemoteObjectVisualStatusControllerComp.h>


/**
	ImtGqlPck package
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
typedef imtclientgql::CRemoteObjectVisualStatusControllerComp RemoteObjectVisualStatusController;


} // namespace ImtClientGqlPck


