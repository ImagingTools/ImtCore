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


/**
	Tell whether the element is accessible. Inside an organization (\c isInOrganization) the organization permissions of the user
	apply as in a command check (\see imtauth::SelectApplicablePermissions, imtauth::CollectHeldPermissions).
*/
inline bool IsElementAccessible(
			const ICommandPermissionsProvider* permissionsProviderPtr,
			imtauth::IPermissionChecker* permissionCheckerPtr,
			const QByteArray& elementId,
			const imtauth::IUserInfo::FeatureIds& userPermissions,
			bool isAdmin,
			const QByteArray& permissionPath,
			bool isInOrganization = false,
			const QByteArrayList& organizationPermissions = QByteArrayList())
{
	if (permissionsProviderPtr == nullptr || permissionCheckerPtr == nullptr){
		return true;
	}

	if (isAdmin){
		return true;
	}

	const QByteArrayList declaredPermissions = permissionsProviderPtr->GetCommandPermissions(elementId);
	if (declaredPermissions.isEmpty()){
		return true;
	}

	const QByteArrayList elementPermissions = imtauth::SelectApplicablePermissions(declaredPermissions, isInOrganization);
	if (elementPermissions.isEmpty()){
		return false;
	}

	const QByteArrayList heldPermissions = imtauth::CollectHeldPermissions(userPermissions, organizationPermissions, elementPermissions);
	if (!permissionPath.isEmpty()){
		return imtauth::IsPermissionPathDeclared(elementPermissions, permissionPath)
				&& permissionCheckerPtr->CheckPermission(heldPermissions, {permissionPath});
	}

	return permissionCheckerPtr->CheckPermission(heldPermissions, elementPermissions);
}


} // namespace imtserverapp
