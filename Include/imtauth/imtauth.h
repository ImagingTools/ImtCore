// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/IUserGroupInfoProvider.h>


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

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (userCollection.GetObjectData(userId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			return userInfoPtr->GetName();
		}
	}

	return QString();
}


/**
	Tell whether the given permission is among the ones held.

	Permissions used to be stored by feature id alone ("EditUser") and are
	stored as the full feature path today ("/Administration/EditUser"). Both
	forms are answered for, in either direction: a permission stored the old way
	satisfies a check written the new way, and the other way round. Two full
	paths, however, are only the same permission when they are equal or
	the held permission is a descendant of the requested path. The same id under
	a different parent is a different feature.

	This is the one place that rule is written down: every check of "does this
	user hold that permission" is to go through here, whatever container the
	held permissions come in (a list, a set of allowed ones, ...).
*/
template <class PermissionsType>
inline bool HasPermission(const PermissionsType& userPermissions, const QByteArray& permissionId)
{
	if (permissionId.isEmpty()){
		return false;
	}

	if (userPermissions.contains(permissionId)){
		return true;
	}

	const bool isPath = permissionId.startsWith('/');
	const QByteArray featureId = isPath ? permissionId.mid(permissionId.lastIndexOf('/') + 1) : permissionId;

	for (const QByteArray& userPermission : userPermissions){
		if (isPath && userPermission.startsWith('/')){
			if (userPermission.startsWith(permissionId)
					&& userPermission.size() > permissionId.size()
					&& userPermission.at(permissionId.size()) == '/'){
				return true;
			}

			continue;
		}

		if (userPermission.mid(userPermission.lastIndexOf('/') + 1) == featureId){
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


} // namespace imtauth
