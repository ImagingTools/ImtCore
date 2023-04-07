#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtbase/IObjectCollection.h>

namespace imtauth
{


class CUserGroupInfoComp:
		public icomp::CComponentBase,
		public CUserGroupInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupInfoComp);
		I_REGISTER_INTERFACE(IUserGroupInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_roleProviderCompPtr, "RoleProvider", "Role provider", true, "RoleProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtauth::IRoleInfoProvider, m_roleProviderCompPtr);
};


} // namespace imtauth
