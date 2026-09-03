// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserInfo.h>


/**
	Library containing interface and implementation related to user authentification and account management.
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
	paths, however, are only the same permission when they are equal - the same
	id under a different parent is a different feature.

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
			continue;
		}

		if (userPermission.mid(userPermission.lastIndexOf('/') + 1) == featureId){
			return true;
		}
	}

	return false;
}


} // namespace imtauth


