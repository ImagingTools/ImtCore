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


