#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CRole.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtauth
{


class CRoleComp:
		public icomp::CComponentBase,
		public CRole
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRoleComp);
		I_REGISTER_INTERFACE(IRole);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_featuresCompPtr, "Features", "All features avaliable of this role", true, "Features");
		I_ASSIGN(m_roleIdAttrPtr, "RoleId", "Role id", true, "");
		I_ASSIGN(m_roleNameAttrPtr, "RoleName", "Role name", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtlic::IFeatureInfoProvider, m_featuresCompPtr);
	I_TEXTATTR(m_roleIdAttrPtr);
	I_TEXTATTR(m_roleNameAttrPtr);
};
} // namespace imtauth


