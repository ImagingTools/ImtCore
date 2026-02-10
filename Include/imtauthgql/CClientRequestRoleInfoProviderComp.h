// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/IRoleInfoProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CClientRequestRoleInfoProviderComp:
			virtual public imtauth::IRoleInfoProvider,
			public imtclientgql::CClientRequestManagerCompBase
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestRoleInfoProviderComp);
		I_REGISTER_INTERFACE(imtauth::IRoleInfoProvider);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
		I_ASSIGN(m_roleFactoryCompPtr, "RoleFactory", "Role factory", true, "RoleFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IRoleInfoProvider)
	virtual const imtbase::ICollectionInfo& GetRoleList() const override;
	virtual imtauth::IRoleUniquePtr GetRole(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_FACT(imtauth::IRole, m_roleFactoryCompPtr);
};


} // namespace imtauthgql


