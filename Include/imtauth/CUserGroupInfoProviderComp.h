// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual imtauth::IUserGroupInfoSharedPtr GetUserGroup(const QByteArray& groupId, const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
};


} // namespace imtauth


