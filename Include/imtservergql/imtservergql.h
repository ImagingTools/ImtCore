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
		if (!permissions.contains(permissionPath)){
			refusalReason = QStringLiteral("the permission path '%1' is none of '%2' this request runs under")
					.arg(QString::fromUtf8(permissionPath), QString::fromUtf8(permissions.join(';')));

			return false;
		}

		if (!permissionCheckerPtr->CheckPermission(userInfoPtr->GetPermissions(), {permissionPath})){
			refusalReason = QStringLiteral("the user does not hold '%1'")
					.arg(QString::fromUtf8(permissionPath));

			return false;
		}

		return true;
	}

	if (!permissionCheckerPtr->CheckPermission(userInfoPtr->GetPermissions(), permissions)){
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
