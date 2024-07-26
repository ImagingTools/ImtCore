#include <imtauth/CCheckPermissionOrComp.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IPermissionChecker)

bool CCheckPermissionOrComp::CheckPermission(const IUserInfo::FeatureIds &userPermissions, const QByteArrayList &permissionIds)
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


