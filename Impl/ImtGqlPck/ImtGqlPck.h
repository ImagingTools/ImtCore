#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgql/CApiClientComp.h>
#include <imtgql/CHttpGraphQLServletComp.h>
#include <imtgql/CGqlDemultiplexerComp.h>
#include <imtgql/CGqlRemoteRepresentationControllerCompBase.h>
#include <imtgql/CGqlClientEngineComp.h>
#include <imtgql/CGqlRepresentationDataControllerComp.h>
#include <imtgql/CUserSettingsControllerComp.h>
#include <imtgql/CGqlRepresentationJoinerComp.h>
#include <imtgql/CApplicationInfoControllerComp.h>
#include <imtgql/CGqlObjectCollectionComp.h>
#include <imtgql/CRemoteGqlContextControllerComp.h>
#include <imtgql/CGqlRemoteRepresentationControllerComp.h>
#include <imtgql/CGqlContextControllerComp.h>
#include <imtgql/CObjectProviderComp.h>
#include <imtgql/CWebSocketServletComp.h>
#include <imtgql/CCollectionObjectProviderComp.h>


/**
	ImtGqlPck package
*/
namespace ImtGqlPck
{


typedef imtgql::CApiClientComp ApiClient;
typedef imtgql::CHttpGraphQLServletComp GraphQLServlet;
typedef imtgql::CWebSocketServletComp WebSocketServlet;
typedef imtgql::CGqlDemultiplexerComp GraphQLDemultiplexer;
typedef imtgql::CGqlRemoteRepresentationControllerCompBase GraphQLRemoteRepresentationController;
typedef imtgql::CGqlClientEngineComp GraphQLClientEngine;
typedef imtgql::CGqlRepresentationDataControllerComp GqlRepresentationDataController;
typedef imtgql::CUserSettingsControllerComp UserSettingsController;
typedef imtgql::CGqlRepresentationJoinerComp GqlRepresentationJoiner;
typedef imtgql::CApplicationInfoControllerComp ApplicationInfoController;
typedef icomp::TModelCompWrap<imtgql::CGqlObjectCollectionComp> GqlObjectCollection;
typedef imtgql::CRemoteGqlContextControllerComp RemoteGqlContextController;
typedef imtgql::CGqlContextControllerComp GqlContextController;
typedef imtgql::CGqlRemoteRepresentationControllerComp PumaGqlRemoteRepresentationController;
typedef imtgql::CObjectProviderComp ObjectProvider;
typedef imtgql::CCollectionObjectProviderComp CollectionObjectProvider;


} // namespace ImtGqlPck


