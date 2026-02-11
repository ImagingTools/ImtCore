// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/IUserInfoProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtauthgql/CUserRepresentationController.h>


namespace imtauthgql
{


class CClientRequestUserInfoProviderComp:
			virtual public imtauth::IUserInfoProvider,
			public imtclientgql::CClientRequestManagerCompBase
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestUserInfoProviderComp);
		I_REGISTER_INTERFACE(imtauth::IUserInfoProvider);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
		I_ASSIGN(m_userFactoryCompPtr, "UserFactory", "User factory", true, "UserFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IUserInfoProvider)
	virtual const imtbase::ICollectionInfo& GetUserList() const override;
	virtual imtauth::IUserInfoUniquePtr GetUser(const QByteArray& userId, const iprm::IParamsSet* paramsPtr) const override;
	
protected:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_FACT(imtauth::IUserInfo, m_userFactoryCompPtr);

private:
	imtauthgql::CUserRepresentationController m_userRepresentationController;
};


} // namespace imtauthgql


