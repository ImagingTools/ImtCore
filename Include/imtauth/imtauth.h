// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtauth/ITenantConnectionRequestManager.h>


/**
	Library containing interface and implementation related to user authentification and authorization.
*/
namespace imtauth
{


inline QString GetUserName(const imtbase::IObjectCollection& userCollection, const QByteArray& userId)
{
	if (userId.isEmpty()){
		return QString();
	}

	QString userName = userCollection.GetElementInfo(userId, imtbase::ICollectionInfo::EIT_NAME).toString();
	if (userName.isEmpty()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (userCollection.GetObjectData(userId, dataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userInfoPtr != nullptr){
				userName = userInfoPtr->GetName();
			}
		}
	}

	return userName;
}


/**
	Tell whether the given permission is among the ones held.

	A permission is the full path of its feature ("/Administration/Users/Edit/Add").
	The requested permission is held when it is held as such, or - for a group of
	the tree - when a permission inside it is held. A feature id alone names no
	permission: the same id appears under many parents.

	\param includeDescendants	whether a held descendant of the requested path
		counts. Without it only the requested node itself is matched, e.g. to tell
		which nodes of a permission tree are selected.

	This is the one place that rule is written down: every check of "does this
	user hold that permission" is to go through here, whatever container the
	held permissions come in (a list, a set of allowed ones, ...).
*/
template <class PermissionsType>
inline bool HasPermission(const PermissionsType& userPermissions, const QByteArray& permissionId, bool includeDescendants = true)
{
	if (!permissionId.startsWith('/')){
		return false;
	}

	if (userPermissions.contains(permissionId)){
		return true;
	}

	if (!includeDescendants){
		return false;
	}

	for (const QByteArray& userPermission : userPermissions){
		if (userPermission.size() > permissionId.size()
					&& userPermission.startsWith(permissionId)
					&& userPermission.at(permissionId.size()) == '/'){
			return true;
		}
	}

	return false;
}


/**
	Tell whether a request running under the permission path may use something declared with these permissions:
	the path is one of them or lies inside one of them, never above them.
*/
inline bool IsPermissionPathDeclared(const QByteArrayList& declaredPermissions, const QByteArray& permissionPath)
{
	const QByteArrayList pathPermissions{permissionPath};
	for (const QByteArray& declaredPermission : declaredPermissions){
		if (HasPermission(pathPermissions, declaredPermission)){
			return true;
		}
	}

	return false;
}


/**
	Check if including the parent role into the role would close a cycle of parent roles.
*/
inline bool CreatesRoleCycle(const IRoleInfoProvider* roleProviderPtr, const QByteArray& roleId, const QByteArray& parentRoleId)
{
	if (parentRoleId == roleId){
		return true;
	}

	if (roleProviderPtr == nullptr || roleId.isEmpty()){
		return false;
	}

	QByteArrayList pendingRoleIds = {parentRoleId};
	QSet<QByteArray> visitedRoleIds;
	while (!pendingRoleIds.isEmpty()){
		const QByteArray currentRoleId = pendingRoleIds.takeFirst();
		if (currentRoleId == roleId){
			return true;
		}
		if (visitedRoleIds.contains(currentRoleId)){
			continue;
		}
		visitedRoleIds.insert(currentRoleId);

		IRoleUniquePtr rolePtr = roleProviderPtr->GetRole(currentRoleId);
		if (rolePtr.IsValid()){
			pendingRoleIds += rolePtr->GetIncludedRoles();
		}
	}

	return false;
}


/**
	Check if adding the parent group to the group would close a cycle of parent groups.
*/
inline bool CreatesGroupCycle(const IUserGroupInfoProvider* groupProviderPtr, const QByteArray& groupId, const QByteArray& parentGroupId)
{
	if (parentGroupId == groupId){
		return true;
	}

	if (groupProviderPtr == nullptr || groupId.isEmpty()){
		return false;
	}

	QByteArrayList pendingGroupIds = {parentGroupId};
	QSet<QByteArray> visitedGroupIds;
	while (!pendingGroupIds.isEmpty()){
		const QByteArray currentGroupId = pendingGroupIds.takeFirst();
		if (currentGroupId == groupId){
			return true;
		}
		if (visitedGroupIds.contains(currentGroupId)){
			continue;
		}
		visitedGroupIds.insert(currentGroupId);

		IUserGroupInfoSharedPtr groupPtr = groupProviderPtr->GetUserGroup(currentGroupId);
		if (groupPtr.IsValid()){
			pendingGroupIds += groupPtr->GetParentGroups();
		}
	}

	return false;
}


/**
	Keep of \c permissions only those inside \c scope, the permission scope of an organization.
*/
inline QByteArrayList RestrictPermissions(const QByteArrayList& permissions, const QByteArrayList& scope)
{
	QByteArrayList retVal;
	for (const QByteArray& permission : permissions){
		if (HasPermission(scope, permission, false) && !retVal.contains(permission)){
			retVal.append(permission);
		}
	}

	return retVal;
}


/**
	Permission to create an organization.
*/
static const inline QByteArray s_addOrganizationPermission = QByteArrayLiteral("/OrganizationManagement/Add");


/**
	Permissions to open the organizations page and list. Everyone who belongs to an organization or is invited to one holds them.
*/
inline QByteArrayList GetOrganizationAccessPermissions()
{
	return {QByteArrayLiteral("/OrganizationManagement/ViewPage"), QByteArrayLiteral("/OrganizationManagement/View")};
}


/**
	The management of an organization from inside it: its owner and its creator hold this root, a member what the membership gives.
*/
static const inline QByteArray s_organizationManagementPermission = QByteArrayLiteral("/OrganizationManagement");


/**
	Tell whether the permission manages the inside of an organization (members, roles, groups, connections...).
	It is held only inside the organization, through the ownership or the membership, never through a product role;
	listing, creating and removing organizations are permissions of the product.
*/
inline bool IsOrganizationInternalPermission(const QByteArray& permission)
{
	static const QByteArrayList productPermissions = GetOrganizationAccessPermissions()
				+ QByteArrayList{s_addOrganizationPermission, QByteArrayLiteral("/OrganizationManagement/Remove")};

	return permission.startsWith(s_organizationManagementPermission + '/') && !productPermissions.contains(permission);
}


/**
	Tell whether the permission administers the users, roles or groups of the product.
	Inside an organization the organization permissions apply to them instead.
*/
inline bool IsUserAdministrationPermission(const QByteArray& permission)
{
	return permission.startsWith("/Administration/Users/")
				|| permission.startsWith("/Administration/Roles/")
				|| permission.startsWith("/Administration/Groups/");
}


/**
	Select the declared permissions that apply in the context of a check: inside an organization its organization
	permissions instead of the product administration of users, roles and groups, outside of one the product permissions only.
*/
inline QByteArrayList SelectApplicablePermissions(const QByteArrayList& declaredPermissions, bool isInOrganization)
{
	QByteArrayList retVal;
	for (const QByteArray& permission : declaredPermissions){
		const bool isApplicable = IsOrganizationInternalPermission(permission)
					? isInOrganization
					: !(isInOrganization && IsUserAdministrationPermission(permission));
		if (isApplicable){
			retVal.append(permission);
		}
	}

	return retVal;
}


/**
	Get the permissions held for a check of the applicable ones. The organization permissions come from the ownership,
	which holds all of them, or the membership, never from the permissions of a role.
*/
inline QByteArrayList CollectHeldPermissions(
			const QByteArrayList& userPermissions,
			const QByteArrayList& organizationPermissions,
			const QByteArrayList& applicablePermissions)
{
	QByteArrayList retVal = userPermissions;
	retVal.removeIf([](const QByteArray& permission){
		return IsOrganizationInternalPermission(permission);
	});

	if (!organizationPermissions.contains(s_organizationManagementPermission)){
		return retVal + organizationPermissions;
	}

	for (const QByteArray& permission : applicablePermissions){
		if (IsOrganizationInternalPermission(permission)){
			retVal.append(permission);
		}
	}

	return retVal;
}



/**
	Tell whether the two organizations are linked by an active connection.
*/
inline bool AreTenantsConnected(const ITenantConnectionRequestManager& connectionManager, const QByteArray& tenantId, const QByteArray& otherTenantId)
{
	if (tenantId.isEmpty() || otherTenantId.isEmpty()){
		return false;
	}

	for (const QByteArray& connectionId : connectionManager.GetConnectionIds(tenantId)){
		ITenantConnectionInfoUniquePtr connectionPtr = connectionManager.GetConnection(connectionId);
		if (		connectionPtr.IsValid() &&
					connectionPtr->GetStatus() == ITenantConnectionInfo::CS_ACTIVE &&
					((connectionPtr->GetTenantAId() == tenantId && connectionPtr->GetTenantBId() == otherTenantId) ||
					(connectionPtr->GetTenantAId() == otherTenantId && connectionPtr->GetTenantBId() == tenantId))){
			return true;
		}
	}

	return false;
}


} // namespace imtauth
