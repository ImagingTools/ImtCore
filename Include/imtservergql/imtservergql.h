// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
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

	const QByteArray productId = gqlContext.GetProductId();
	return (productId.isEmpty() ? userInfoPtr->GetPermissions() : userInfoPtr->GetPermissions(productId)) + gqlContext.GetImpliedPermissions();
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

	// A superuser is not bound by the permission tree; the owner of an organization holds its scope as permissions.
	if (userInfoPtr->IsAdmin()){
		return true;
	}

	if (permissions.isEmpty()){
		return true;
	}

	const QByteArray permissionPath = GetPermissionPath(gqlRequest);
	if (!permissionPath.isEmpty()){
		if (!imtauth::IsPermissionPathDeclared(permissions, permissionPath)){
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


/**
	Check a command on the revisions of a collection.

	The revisions of every collection have a permission area of their own, given
	in \c revisionPermissionPaths in the order of \c collectionIds (e.g.
	"/SensorManagement/Revisions"); the command needs the leaf of its action in
	that area: View for the list, Restore and Delete for a revision.
*/
inline bool CheckRevisionPermissions(
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArrayList& collectionIds,
			const QByteArrayList& revisionPermissionPaths,
			QString& refusalReason)
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr != nullptr ? gqlContextPtr->GetUserInfo() : nullptr;
	if (userInfoPtr == nullptr){
		refusalReason = QStringLiteral("the caller is not authenticated");

		return false;
	}

	if (userInfoPtr->IsAdmin()){
		return true;
	}

	static const QMap<QByteArray, QByteArray> actions{
		{QByteArrayLiteral("GetRevisionInfoList"), QByteArrayLiteral("View")},
		{QByteArrayLiteral("RestoreRevision"), QByteArrayLiteral("Restore")},
		{QByteArrayLiteral("DeleteRevision"), QByteArrayLiteral("Delete")}};
	const QByteArray action = actions.value(gqlRequest.GetCommandId());
	if (action.isEmpty()){
		refusalReason = QStringLiteral("the command is not available on revisions");

		return false;
	}

	const imtgql::CGqlParamObject* inputPtr = gqlRequest.GetParamObject("input");
	const QByteArray collectionId = inputPtr != nullptr ? inputPtr->GetParamArgumentValue("collectionId").toByteArray() : QByteArray();
	const qsizetype index = collectionIds.indexOf(collectionId);
	if (index < 0 || index >= revisionPermissionPaths.size() || revisionPermissionPaths[index].isEmpty()){
		refusalReason = QStringLiteral("no revision permissions are declared for the collection '%1'").arg(QString::fromUtf8(collectionId));

		return false;
	}

	const QByteArray permission = revisionPermissionPaths[index] + '/' + action;
	if (!imtauth::HasPermission(GetUserPermissions(*gqlContextPtr), permission)){
		refusalReason = QStringLiteral("the user does not hold '%1'").arg(QString::fromUtf8(permission));

		return false;
	}

	return true;
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

	if (userInfoPtr->IsAdmin()){
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
