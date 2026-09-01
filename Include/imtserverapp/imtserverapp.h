// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtauth/IPermissionChecker.h>
#include <imtserverapp/ICommandPermissionsProvider.h>


/**
	Library containing the server side representation of an application and its GUI elements.
*/
namespace imtserverapp
{


inline QByteArray GetPermissionPath(const iprm::IParamsSet* paramsPtr)
{
	iprm::TParamsPtr<iprm::IIdParam> permissionPathParamPtr(paramsPtr, "PermissionPath");
	if (permissionPathParamPtr.IsValid()){
		QByteArray permissionPath = permissionPathParamPtr->GetId();

		// A permission path always starts with '/', whoever named it.
		if (!permissionPath.isEmpty() && !permissionPath.startsWith('/')){
			permissionPath.prepend('/');
		}

		return permissionPath;
	}

	return QByteArray();
}


inline bool IsElementAccessible(
			const ICommandPermissionsProvider* permissionsProviderPtr,
			imtauth::IPermissionChecker* permissionCheckerPtr,
			const QByteArray& elementId,
			const imtauth::IUserInfo::FeatureIds& userPermissions,
			bool isAdmin,
			const QByteArray& permissionPath)
{
	if (permissionsProviderPtr == nullptr || permissionCheckerPtr == nullptr){
		return true;
	}

	if (isAdmin){
		return true;
	}

	const QByteArrayList elementPermissions = permissionsProviderPtr->GetCommandPermissions(elementId);
	if (elementPermissions.isEmpty()){
		return true;
	}

	if (!permissionPath.isEmpty()){
		return elementPermissions.contains(permissionPath)
				&& permissionCheckerPtr->CheckPermission(userPermissions, {permissionPath});
	}

	return permissionCheckerPtr->CheckPermission(userPermissions, elementPermissions);
}


} // namespace imtserverapp
