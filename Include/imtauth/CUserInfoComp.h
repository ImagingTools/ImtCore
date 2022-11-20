#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtbase/IObjectCollection.h>

namespace imtauth
{


class CUserInfoComp:
		public icomp::CComponentBase,
		public CUserInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserInfoComp);
		I_REGISTER_INTERFACE(IUserInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_featuresCompPtr, "Features", "All features avaliable of this user", true, "Features");
		I_ASSIGN(m_roleProviderCompPtr, "RoleProvider", "Role provider", true, "RoleProvider");
		I_ASSIGN(m_adminIdAttrPtr, "AdminId", "Id for admin user", true, "admin");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (iser::IUserInfo)
	virtual bool IsAdmin() const override;

private:
	I_REF(imtlic::IFeatureInfoProvider, m_featuresCompPtr);
	I_REF(imtauth::IRoleInfoProvider, m_roleProviderCompPtr);
	I_ATTR(QByteArray, m_adminIdAttrPtr);
};


} // namespace imtauth
