#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgql/CHttpGraphQLServletComp.h>
#include <imtgql/CGqlDemultiplexerComp.h>
#include <imtgql/CGqlRepresentationDataControllerComp.h>
#include <imtgql/CUserSettingsControllerComp.h>
#include <imtgql/CGqlRepresentationJoinerComp.h>
#include <imtgql/CApplicationInfoControllerComp.h>
#include <imtgql/CSerializableObjectCollectionControllerComp.h>
#include <imtgql/CRemoteGqlContextControllerComp.h>
#include <imtgql/CGqlContextControllerComp.h>
#include <imtgql/CObjectProviderComp.h>
#include <imtgql/CWebSocketServletComp.h>
#include <imtgql/CCollectionObjectProviderComp.h>
#include <imtgql/CGqlControllerComp.h>
#include <imtgql/COperationContextControllerComp.h>


/**
	ImtGqlPck package
*/
namespace ImtGqlPck
{


typedef imtgql::CHttpGraphQLServletComp GraphQLServlet;
typedef imtgql::CWebSocketServletComp WebSocketServlet;
typedef imtgql::CGqlDemultiplexerComp GraphQLDemultiplexer;
typedef imtgql::CGqlRepresentationDataControllerComp GqlRepresentationDataController;
typedef imtgql::CUserSettingsControllerComp UserSettingsController;
typedef imtgql::CGqlRepresentationJoinerComp GqlRepresentationJoiner;
typedef imtgql::CApplicationInfoControllerComp ApplicationInfoController;
typedef imtgql::CRemoteGqlContextControllerComp RemoteGqlContextController;
typedef imtgql::CGqlContextControllerComp GqlContextController;
typedef imtgql::CObjectProviderComp ObjectProvider;
typedef imtgql::CCollectionObjectProviderComp CollectionObjectProvider;
typedef imtgql::CGqlControllerComp GqlController;
typedef imtgql::CSerializableObjectCollectionControllerComp SerializableObjectCollectionController;
typedef imtgql::COperationContextControllerComp OperationContextController;


} // namespace ImtGqlPck


