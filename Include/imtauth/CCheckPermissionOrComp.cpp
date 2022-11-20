#include <imtauth/CCheckPermissionOrComp.h>


namespace imtauth
{


bool CCheckPermissionOrComp::CheckPermission(const IUserInfo::FeatureIds &userPermissions, const QByteArrayList &permissionIds)
{
	for (const QByteArray& permissionId : permissionIds){
		if (userPermissions.contains(permissionId)){
			return true;
		}
	}

	return false;
}


} // namespace imtauth
