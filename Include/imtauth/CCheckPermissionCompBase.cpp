// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCheckPermissionCompBase.h>


namespace imtauth
{


bool CCheckPermissionCompBase::CheckPermission(const IUserInfo::FeatureIds &userPermissions, const QByteArrayList &permissionIds)
{
	for (const QByteArray& permissionId : permissionIds){
		if (!userPermissions.contains(permissionId)){
			return false;
		}
	}

	return true;
}


} // namespace imtauth


