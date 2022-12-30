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
	virtual bool CheckPermission(const imtauth::IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissions) override;
};


} // namespace imtauth
