#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserGroupInfoProvider.h>


namespace imtauth
{


class CUserGroupInfoProviderComp: public icomp::CComponentBase, virtual public IUserGroupInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupInfoProviderComp);
		I_REGISTER_INTERFACE(IUserGroupInfoProvider);
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "User group collection", true, "UserGroupCollection");
	I_END_COMPONENT;

	// reimplemented (IUserGroupInfoProvider)
	virtual const imtbase::ICollectionInfo& GetUserGroupList() const override;
	virtual const imtauth::IUserGroupInfo* GetUserGroup(const QByteArray& groupId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
};


} // namespace imtauth


