// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <algorithm>

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QString>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtauth/IPermissionChecker.h>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <imtgql/imtgql.h>
#include <imtserverapp/ICommandPermissionsProvider.h>


/**
	Library containing the GraphQL server implementation.
*/
namespace imtservergql
{


/**
	Machine codes of a refused or ended subscription, in the extensions of its error frame: a client decides by them, not by the message.
*/
/// The credentials expired or their session is closed: the client authenticates again.
static const inline QByteArray s_subscriptionUnauthorizedCode = QByteArrayLiteral("Unauthorized");
/// The access is refused: subscribing again does not help.
static const inline QByteArray s_subscriptionForbiddenCode = QByteArrayLiteral("Forbidden");
/// The permissions changed: a new registration decides on the current ones.
static const inline QByteArray s_subscriptionPermissionsChangedCode = QByteArrayLiteral("PermissionsChanged");
/// The access could not be confirmed for now: the client subscribes again later.
static const inline QByteArray s_subscriptionAuthorizationUnavailableCode = QByteArrayLiteral("AuthorizationUnavailable");


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

	Within an organization only the permissions of its scope are held, whether
	held directly, through a role or included by another permission.
	\see imtgql::CollectTenantScopedPermissions
*/
inline imtauth::IUserInfo::FeatureIds GetUserPermissions(const imtgql::IGqlContext& gqlContext)
{
	const imtauth::IUserInfo* userInfoPtr = gqlContext.GetUserInfo();
	if (userInfoPtr == nullptr){
		return imtauth::IUserInfo::FeatureIds();
	}

	const QByteArray productId = gqlContext.GetProductId();
	const imtauth::IUserInfo::FeatureIds userPermissions = productId.isEmpty() ? userInfoPtr->GetPermissions() : userInfoPtr->GetPermissions(productId);

	return imtgql::CollectTenantScopedPermissions(gqlContext, userPermissions) + gqlContext.GetImpliedPermissions();
}


/**
	What decides the access of a context, as values: a snapshot that does not change with the objects it was read from.
	The lists are sorted and hold every item once.
*/
struct AuthorizationData
{
	int principalKind = 0;
	QByteArray userId;
	QByteArray tenantId;
	QByteArray productId;
	QByteArray serverProductId;
	bool hasUser = false;
	bool isAdmin = false;
	bool isTenantOwner = false;
	// The groups open objects shared with them, whatever permissions they give
	QByteArrayList groups;
	QByteArrayList permissions;
	QByteArrayList organizationPermissions;
	QByteArrayList tenantPermissions;
	// No bound and a bound allowing nothing differ
	bool hasBoundary = false;
	QByteArrayList boundary;
	// What the request may give in its product, apart from what it may do
	bool hasGrantingAuthority = false;
	bool isGrantingAdmin = false;
	QByteArrayList grantingPermissions;
};


inline AuthorizationData GetAuthorizationData(const imtgql::IGqlContext& gqlContext)
{
	auto sorted = [](QByteArrayList list){
		std::sort(list.begin(), list.end());
		list.erase(std::unique(list.begin(), list.end()), list.end());

		return list;
	};

	AuthorizationData retVal;
	retVal.principalKind = gqlContext.GetPrincipalKind();
	retVal.userId = gqlContext.GetUserId();
	retVal.tenantId = gqlContext.GetTenantId();
	retVal.productId = gqlContext.GetProductId();
	retVal.serverProductId = gqlContext.GetServerProductId();

	const imtauth::IUserInfo* userInfoPtr = gqlContext.GetUserInfo();
	retVal.hasUser = userInfoPtr != nullptr;
	retVal.isAdmin = userInfoPtr != nullptr && userInfoPtr->IsAdmin();
	retVal.isTenantOwner = gqlContext.IsTenantOwner();
	if (userInfoPtr != nullptr){
		retVal.groups = sorted(userInfoPtr->GetGroups());
	}
	retVal.permissions = sorted(GetUserPermissions(gqlContext));
	retVal.organizationPermissions = sorted(gqlContext.GetOrganizationPermissions());
	retVal.tenantPermissions = sorted(gqlContext.GetTenantPermissions());
	retVal.hasBoundary = gqlContext.HasPermissionBoundary();
	if (retVal.hasBoundary){
		retVal.boundary = sorted(gqlContext.GetPermissionBoundary());
	}
	retVal.hasGrantingAuthority = gqlContext.GetGrantingAuthority(retVal.isGrantingAdmin, retVal.grantingPermissions);
	retVal.grantingPermissions = sorted(retVal.grantingPermissions);

	return retVal;
}


/**
	Tell whether two accesses have the same recipient: the principal, the user, its organization, its product and the server of the product.
	The administrator and the owner of an organization stay the same recipient.
*/
inline bool IsSameRecipient(const AuthorizationData& registered, const AuthorizationData& current)
{
	return registered.principalKind == current.principalKind
				&& registered.userId == current.userId
				&& registered.tenantId == current.tenantId
				&& registered.productId == current.productId
				&& registered.serverProductId == current.serverProductId;
}


/**
	Tell whether the current access holds all of the registered one: the same recipient with the same administrator and owner flags,
	and no less of any permission, group, token bound or authority to grant. A bound or an authority to grant that appears or goes
	is no such access: none and an empty one differ.
*/
inline bool IsAccessWithin(const AuthorizationData& registered, const AuthorizationData& current)
{
	auto isWithin = [](const QByteArrayList& sortedPart, const QByteArrayList& sortedWhole){
		return std::includes(sortedWhole.cbegin(), sortedWhole.cend(), sortedPart.cbegin(), sortedPart.cend());
	};

	return IsSameRecipient(registered, current)
				&& registered.hasUser == current.hasUser
				&& registered.isAdmin == current.isAdmin
				&& registered.isTenantOwner == current.isTenantOwner
				&& isWithin(registered.groups, current.groups)
				&& isWithin(registered.permissions, current.permissions)
				&& isWithin(registered.organizationPermissions, current.organizationPermissions)
				&& isWithin(registered.tenantPermissions, current.tenantPermissions)
				&& registered.hasBoundary == current.hasBoundary
				&& isWithin(registered.boundary, current.boundary)
				&& registered.hasGrantingAuthority == current.hasGrantingAuthority
				&& registered.isGrantingAdmin == current.isGrantingAdmin
				&& isWithin(registered.grantingPermissions, current.grantingPermissions);
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

	const QByteArrayList applicablePermissions = imtauth::SelectApplicablePermissions(permissions, !gqlContextPtr->GetTenantId().isEmpty());
	if (applicablePermissions.isEmpty()){
		refusalReason = QStringLiteral("none of '%1' applies here")
				.arg(QString::fromUtf8(permissions.join(';')));

		return false;
	}

	const imtauth::IUserInfo::FeatureIds heldPermissions = imtauth::CollectHeldPermissions(
				GetUserPermissions(*gqlContextPtr),
				gqlContextPtr->GetOrganizationPermissions(),
				applicablePermissions);

	const QByteArray permissionPath = GetPermissionPath(gqlRequest);
	if (!permissionPath.isEmpty()){
		if (!imtauth::IsPermissionPathDeclared(applicablePermissions, permissionPath)){
			refusalReason = QStringLiteral("the permission path '%1' is none of '%2' this request runs under")
					.arg(QString::fromUtf8(permissionPath), QString::fromUtf8(applicablePermissions.join(';')));

			return false;
		}

		if (!permissionCheckerPtr->CheckPermission(heldPermissions, {permissionPath})){
			refusalReason = QStringLiteral("the user does not hold '%1'")
					.arg(QString::fromUtf8(permissionPath));

			return false;
		}

		return true;
	}

	if (!permissionCheckerPtr->CheckPermission(heldPermissions, applicablePermissions)){
		refusalReason = QStringLiteral("the user holds none of '%1'")
				.arg(QString::fromUtf8(applicablePermissions.join(';')));

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
