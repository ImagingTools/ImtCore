#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ICheckPermission.h>


namespace imtauth
{


class CCheckPermissionCompBase:
		public icomp::CComponentBase,
		virtual public imtauth::ICheckPermission
{

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCheckPermissionCompBase);
		I_REGISTER_INTERFACE(imtauth::ICheckPermission);
	I_END_COMPONENT;

	// reimplemented (imtauth::ICheckPermission)
	virtual bool CheckPermission(const imtauth::IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissions) override;
};


} // namespace imtauth


