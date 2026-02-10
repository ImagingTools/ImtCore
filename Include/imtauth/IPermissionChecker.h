// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


/**
	Interface for check permissions.
	\ingroup Permission
*/
class IPermissionChecker: virtual public istd::IPolymorphic
{
public:
	/**
		Check user permissions.
	*/
	virtual bool CheckPermission(const imtauth::IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissions) = 0;
};


} // namespace imtauth


