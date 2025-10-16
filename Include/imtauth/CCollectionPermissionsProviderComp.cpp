#include <imtauth/CCollectionPermissionsProviderComp.h>

//ImtCore includes
#include <imtlic/IProductLicensingInfo.h>


namespace imtauth
{


const QByteArrayList CCollectionPermissionsProviderComp::GetPermissionInfos(const QByteArray& /*productId*/) const
{
	return QByteArrayList();
}


} // namespace imtauth
