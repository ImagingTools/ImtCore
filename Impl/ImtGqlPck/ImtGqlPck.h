#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgql/CApiClientComp.h>
#include <imtgql/CHttpGraphQLServletComp.h>
#include <imtgql/CGqlDemultiplexerComp.h>
#include <imtgql/CSingleCommandPermissionsProviderComp.h>
#include <imtgql/CCommandPermissionsProviderJoinerComp.h>
#include <imtgql/CPermissionsControllerComp.h>
#include <imtgql/CGqlRepresentationControllerComp.h>
#include <imtgql/CGqlClientEngineComp.h>



/**
	ImtGqlPck package
*/
namespace ImtGqlPck
{


typedef imtgql::CApiClientComp ApiClient;
typedef imtgql::CHttpGraphQLServletComp GraphQLServlet;
typedef imtgql::CGqlDemultiplexerComp GraphQLDemultiplexer;
typedef imtgql::CSingleCommandPermissionsProviderComp SingleCommandPermissionsProvider;
typedef imtgql::CCommandPermissionsProviderJoinerComp CommandPermissionsProviderJoiner;
typedef imtgql::CPermissionsControllerComp PermissionsController;
typedef imtgql::CGqlRepresentationControllerComp GraphQLRepresentationController;
typedef imtgql::CGqlClientEngineComp GraphQLClientEngine;



} // namespace ImtGqlPck



