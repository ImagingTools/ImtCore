// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IPermissionChecker.h>


namespace imtauth
{


class CCheckPermissionOrComp:
			public icomp::CComponentBase,
			virtual public imtauth::IPermissionChecker
{

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCheckPermissionOrComp);
		I_REGISTER_INTERFACE(imtauth::IPermissionChecker);
	I_END_COMPONENT;

	// reimplemented (imtauth::IPermissionChecker)
	virtual bool CheckPermission(const IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissionIds) override;
};


} // namespace imtauth
