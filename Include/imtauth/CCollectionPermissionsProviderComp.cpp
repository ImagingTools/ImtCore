// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
