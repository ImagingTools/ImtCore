// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtbase/COperationContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


/**
	\file imttaggql.h
	\brief Main header file for the imttaggql module - GraphQL API of tags
*/

/**
	\namespace imttaggql
	\brief GraphQL API of the tag catalog and tag assignments (Sdl/imttag/1.0/Tags.sdl).
*/
namespace imttaggql
{


/**
	Permission to see the tag catalog and the tags of entities.
*/
static const QByteArray s_viewTagsPermission = QByteArrayLiteral("ViewTags");

/**
	Permission to assign and unassign tags.
*/
static const QByteArray s_assignTagsPermission = QByteArrayLiteral("AssignTags");

/**
	Permission to manage the tags of the own tenant. System tags require the superuser.
*/
static const QByteArray s_manageTagsPermission = QByteArrayLiteral("ManageTags");


inline bool IsSuperuserRequest(const imtgql::CGqlRequest& gqlRequest)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr){
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();

	return (userInfoPtr != nullptr) && userInfoPtr->IsAdmin();
}


inline QByteArray GetRequestTenantId(const imtgql::CGqlRequest& gqlRequest)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();

	return (contextPtr != nullptr) ? contextPtr->GetTenantId() : QByteArray();
}


/**
	Create an operation context carrying the author and tenant of the request.
*/
inline void InitOperationContext(const imtgql::CGqlRequest& gqlRequest, const QString& description, imtbase::COperationContext& operationContext)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		QString userName;
		const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userName = userInfoPtr->GetName();
		}

		operationContext.SetOperationOwnerId(imtbase::IOperationContext::IdentifableObjectInfo(contextPtr->GetUserId(), userName));
		operationContext.SetTenantId(contextPtr->GetTenantId());
	}

	operationContext.SetOperationDescription(description);
}


} // namespace imttaggql


