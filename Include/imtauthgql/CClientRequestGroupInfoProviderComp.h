// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CClientRequestGroupInfoProviderComp:
			virtual public imtauth::IUserGroupInfoProvider,
			public imtclientgql::CClientRequestManagerCompBase
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestGroupInfoProviderComp);
		I_REGISTER_INTERFACE(imtauth::IUserGroupInfoProvider);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
		I_ASSIGN(m_userGroupFactoryCompPtr, "UserGroupFactory", "User group factory", true, "UserGroupFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IUserGroupInfoProvider)
	virtual const imtbase::ICollectionInfo& GetUserGroupList() const override;
	virtual imtauth::IUserGroupInfoSharedPtr GetUserGroup(const QByteArray& groupId, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	
protected:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_FACT(imtauth::IUserGroupInfo, m_userGroupFactoryCompPtr);
};


} // namespace imtauthgql


