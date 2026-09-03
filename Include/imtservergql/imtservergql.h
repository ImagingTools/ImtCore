// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtauth/IPermissionChecker.h>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <imtserverapp/ICommandPermissionsProvider.h>


/**
	Library containing the GraphQL server implementation.
*/
namespace imtservergql
{


inline QByteArray GetPermissionPath(const imtgql::CGqlRequest& gqlRequest)
{
	QByteArray permissionPath = gqlRequest.GetHeader(imtbase::s_permissionPathHeaderId);

	// A permission path always starts with '/', whoever named it.
	if (!permissionPath.isEmpty() && !permissionPath.startsWith('/')){
		permissionPath.prepend('/');
	}

	return permissionPath;
}


/**
	Get every permission the user of this request holds.

	A permission grants what it requires: a user holding 'A' whose requirements
	name 'B' holds 'B' as well, and so on through the requirements of 'B'. Those
	requirements are declared in the feature tree of the product, so a context
	without product info answers with the permissions held directly only.
*/
inline imtauth::IUserInfo::FeatureIds GetUserPermissions(const imtgql::IGqlContext& gqlContext)
{
	const imtauth::IUserInfo* userInfoPtr = gqlContext.GetUserInfo();
	if (userInfoPtr == nullptr){
		return imtauth::IUserInfo::FeatureIds();
	}

	return userInfoPtr->GetPermissions() + gqlContext.GetImpliedPermissions();
}


/**
	Check the permissions held by the user of the given context, requirements included.
*/
inline bool CheckUserPermissions(
			const imtgql::IGqlContext& gqlContext,
			imtauth::IPermissionChecker& permissionChecker,
			const QByteArrayList& permissions)
{
	if (gqlContext.GetUserInfo() == nullptr){
		return false;
	}

	return permissionChecker.CheckPermission(GetUserPermissions(gqlContext), permissions);
}


inline bool CheckPermissions(
			const imtgql::CGqlRequest& gqlRequest,
			imtauth::IPermissionChecker* permissionCheckerPtr,
			const QByteArrayList& permissions,
			QString& refusalReason)
{
	if (permissionCheckerPtr == nullptr){
		return true;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		refusalReason = QStringLiteral("the request carries no context");

		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		refusalReason = QStringLiteral("the caller is not authenticated");

		return false;
	}

	// A superuser and the owner of the organization are not bound by the permission tree.
	if (userInfoPtr->IsAdmin() || gqlContextPtr->IsTenantOwner()){
		return true;
	}

	if (permissions.isEmpty()){
		return true;
	}

	const QByteArray permissionPath = GetPermissionPath(gqlRequest);
	if (!permissionPath.isEmpty()){
		// The path the request runs under and the permissions declared for the
		// command may be written in different generations of the permission
		// format, so they are matched by the same rule as the held ones.
		if (!imtauth::HasPermission(permissions, permissionPath)){
			refusalReason = QStringLiteral("the permission path '%1' is none of '%2' this request runs under")
					.arg(QString::fromUtf8(permissionPath), QString::fromUtf8(permissions.join(';')));

			return false;
		}

		if (!CheckUserPermissions(*gqlContextPtr, *permissionCheckerPtr, {permissionPath})){
			refusalReason = QStringLiteral("the user does not hold '%1'")
					.arg(QString::fromUtf8(permissionPath));

			return false;
		}

		return true;
	}

	if (!CheckUserPermissions(*gqlContextPtr, *permissionCheckerPtr, permissions)){
		refusalReason = QStringLiteral("the user holds none of '%1'")
				.arg(QString::fromUtf8(permissions.join(';')));

		return false;
	}

	return true;
}


inline bool CheckPermissions(
			const imtgql::CGqlRequest& gqlRequest,
			imtauth::IPermissionChecker* permissionCheckerPtr,
			const QByteArrayList& permissions)
{
	QString refusalReason;

	return CheckPermissions(gqlRequest, permissionCheckerPtr, permissions, refusalReason);
}


inline bool CheckRequestPermissions(
			const imtgql::CGqlRequest& gqlRequest,
			const imtserverapp::ICommandPermissionsProvider* permissionsProviderPtr,
			imtauth::IPermissionChecker* permissionCheckerPtr,
			bool denyUndeclared,
			QString& refusalReason)
{
	if (permissionsProviderPtr == nullptr){
		return true;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		refusalReason = QStringLiteral("the request carries no context");

		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		refusalReason = QStringLiteral("the caller is not authenticated");

		return false;
	}

	if (userInfoPtr->IsAdmin() || gqlContextPtr->IsTenantOwner()){
		return true;
	}

	const QByteArray commandId = gqlRequest.GetCommandId();
	if (!permissionsProviderPtr->GetCommandIds().contains(commandId)){
		refusalReason = QStringLiteral("no permission is declared for this command");

		return !denyUndeclared;
	}

	return CheckPermissions(
			gqlRequest,
			permissionCheckerPtr,
			permissionsProviderPtr->GetCommandPermissions(commandId),
			refusalReason);
}


} // namespace imtservergql
