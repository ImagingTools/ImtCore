// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/TMakeModelObserverCompWrap.h>
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtservergql/CHttpGraphQLServletComp.h>
#include <imtservergql/CGqlDemultiplexerComp.h>
#include <imtservergql/CGqlRepresentationDataControllerComp.h>
#include <imtservergql/CUserSettingsControllerComp.h>
#include <imtservergql/CGqlRepresentationJoinerComp.h>
#include <imtservergql/CApplicationInfoControllerComp.h>
#include <imtservergql/CSerializableObjectCollectionControllerComp.h>
#include <imtservergql/CGqlContextControllerComp.h>
#include <imtservergql/CObjectProviderComp.h>
#include <imtservergql/CWebSocketServletComp.h>
#include <imtservergql/CCollectionObjectProviderComp.h>
#include <imtservergql/COperationContextControllerComp.h>
#include <imtservergql/CObjectCollectionChangeNotifierComp.h>
#include <imtservergql/CModelChangeNotifierComp.h>
#include <imtservergql/CModelPublisherComp.h>
#include <imtservergql/CStructureSubscriberControllerComp.h>
#include <imtservergql/CBroadcastMessageConsumerControllerComp.h>
#include <imtservergql/CBroadcastMessageConsumerSubscriberComp.h>
#include <imtservergql/CSlaveSubscriberControllerComp.h>
#include <imtservergql/CGqlSearchControllerComp.h>
#include <imtservergql/CTableViewParamControllerComp.h>
#include <imtservergql/CConnectionStatusSubscriberControllerComp.h>
#include <imtservergql/CHeadersDataProviderComp.h>
#include <imtservergql/CDocumentRevisionControllerComp.h>
#include <imtservergql/CCommandsControllerComp.h>
#include <imtservergql/CSettingsControllerComp.h>
#include <imtservergql/CProgressManagerPublisherComp.h>
#include <imtservergql/CGqlCollectionImportControllerComp.h>
#include <imtservergql/CCollectionImportControllerComp.h>
#include <imtservergql/CCollectionImportJobParamsFillerComp.h>
#include <imtservergql/CCollectionDocumentManagerControllerComp.h>
#include <imtservergql/CCollectionDocumentManagerPublisherComp.h>
#include <imtservergql/CGqlContextCreatorComp.h>


/**
	ImtServerGqlPck package
*/
namespace ImtServerGqlPck
{


typedef imtservergql::CHttpGraphQLServletComp GraphQLServlet;
typedef imtservergql::CWebSocketServletComp WebSocketServlet;
typedef imtservergql::CGqlDemultiplexerComp GraphQLDemultiplexer;
typedef imtservergql::CGqlRepresentationDataControllerComp GqlRepresentationDataController;
typedef imtservergql::CUserSettingsControllerComp UserSettingsController;
typedef imtservergql::CGqlRepresentationJoinerComp GqlRepresentationJoiner;
typedef imtservergql::CApplicationInfoControllerComp ApplicationInfoController;
typedef imtservergql::CGqlContextControllerComp GqlContextController;
typedef imtservergql::CObjectProviderComp ObjectProvider;
typedef imtservergql::CCollectionObjectProviderComp CollectionObjectProvider;
typedef imtservergql::CSerializableObjectCollectionControllerComp SerializableObjectCollectionController;
typedef imtservergql::COperationContextControllerComp OperationContextController;
typedef imtservergql::CObjectCollectionChangeNotifierComp ObjectCollectionChangeNotifier;
typedef imtservergql::CModelChangeNotifierComp ModelChangeNotifier;
typedef imtservergql::CModelPublisherComp ModelPublisher;
typedef imtservergql::CStructureSubscriberControllerComp StructureSubscriberController;
typedef imtservergql::CBroadcastMessageConsumerControllerComp BroadcastMessageConsumerController;
typedef imtservergql::CBroadcastMessageConsumerSubscriberComp BroadcastMessageConsumerSubscriber;
typedef imtservergql::CSlaveSubscriberControllerComp SlaveSubscriberController;
typedef imtservergql::CGqlSearchControllerComp GqlSearchController;
typedef imtservergql::CTableViewParamControllerComp TableViewParamController;
typedef imtservergql::CConnectionStatusSubscriberControllerComp ConnectionStatusSubscriberController;
typedef imtservergql::CHeadersDataProviderComp HeadersDataProvider;
typedef imtservergql::CDocumentRevisionControllerComp DocumentRevisionController;
typedef imtservergql::CCommandsControllerComp CommandsController;
typedef imtservergql::CSettingsControllerComp SettingsController;
typedef imtservergql::CProgressManagerPublisherComp ProgressManagerPublisher;
typedef imtservergql::CGqlCollectionImportControllerComp GqlCollectionImportController;
typedef imtservergql::CCollectionImportControllerComp CollectionImportController;
typedef imtservergql::CCollectionImportJobParamsFillerComp CollectionImportJobParamsFiller;
typedef imtservergql::CCollectionDocumentManagerControllerComp CollectionDocumentManagerController;
typedef imtservergql::CCollectionDocumentManagerPublisherComp CollectionDocumentManagerPublisher;
typedef imtservergql::CGqlContextCreatorComp GqlContextCreator;


} // namespace ImtServerGqlPck


