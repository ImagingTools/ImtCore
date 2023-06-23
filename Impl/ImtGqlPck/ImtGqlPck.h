#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgql/CApiClientComp.h>
#include <imtgql/CHttpGraphQLServletComp.h>
#include <imtgql/CGqlDemultiplexerComp.h>
#include <imtgql/CGqlRemoteRepresentationControllerComp.h>
#include <imtgql/CGqlClientEngineComp.h>
#include <imtgql/CGqlRepresentationDataControllerComp.h>
#include <imtgql/CUserSettingsControllerComp.h>
#include <imtgql/CGqlRepresentationJoinerComp.h>
#include <imtgql/CApplicationInfoControllerComp.h>
#include <imtgql/CGqlObjectCollectionComp.h>
#include <imtgql/CGqlContextCacheControllerComp.h>
#include <imtgql/CRemoteGqlContextControllerComp.h>
#include <imtgql/CPumaGqlRemoteRepresentationControllerComp.h>
#include <imtgql/CGqlContextControllerComp.h>


/**
	ImtGqlPck package
*/
namespace ImtGqlPck
{


typedef imtgql::CApiClientComp ApiClient;
typedef imtgql::CHttpGraphQLServletComp GraphQLServlet;
typedef imtgql::CGqlDemultiplexerComp GraphQLDemultiplexer;
typedef imtgql::CGqlRemoteRepresentationControllerComp GraphQLRemoteRepresentationController;
typedef imtgql::CGqlClientEngineComp GraphQLClientEngine;
typedef imtgql::CGqlRepresentationDataControllerComp GqlRepresentationDataController;
typedef imtgql::CUserSettingsControllerComp UserSettingsController;
typedef imtgql::CGqlRepresentationJoinerComp GqlRepresentationJoiner;
typedef imtgql::CApplicationInfoControllerComp ApplicationInfoController;
typedef icomp::TModelCompWrap<imtgql::CGqlObjectCollectionComp> GqlObjectCollection;
typedef imtgql::CGqlContextCacheControllerComp GqlContextCacheController;
typedef imtgql::CRemoteGqlContextControllerComp RemoteGqlContextController;
typedef imtgql::CGqlContextControllerComp GqlContextController;
typedef imtgql::CPumaGqlRemoteRepresentationControllerComp PumaGqlRemoteRepresentationController;



} // namespace ImtGqlPck


