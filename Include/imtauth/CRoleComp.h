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
		I_REGISTER_INTERFACE(imtauth::IRoleInfoProvider);
		I_ASSIGN(m_parentRoleProviderCompPtr, "ParentRoleProvider", "Parent role provider", true, "ParentRoleProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(imtauth::IRoleInfoProvider, m_parentRoleProviderCompPtr);
};


} // namespace imtauth


