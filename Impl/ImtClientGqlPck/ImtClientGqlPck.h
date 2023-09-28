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


} // namespace ImtClientGqlPck


