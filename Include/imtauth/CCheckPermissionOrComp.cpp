// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCheckPermissionOrComp.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IPermissionChecker)

bool CCheckPermissionOrComp::CheckPermission(const IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissionIds)
{
	for (const QByteArray& permissionId : permissionIds){
		if (permissionId == "*"){
			return true;
		}

		if (userPermissions.contains(permissionId)){
			return true;
		}
	}

	return false;
}


} // namespace imtauth


