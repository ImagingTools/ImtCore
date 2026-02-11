// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <ibase/IApplicationInfo.h>
#include <icomp/CComponentBase.h>
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtbase/CTreeItemModel.h>
#include <imtauth/IAccessTokenProvider.h>
#include <imtauth/IPermissionChecker.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/ISuperuserProvider.h>
#include <imtauth/IUserPermissionsController.h>


namespace imtauthgql
{


class CSimpleLoginWrapComp:
			public imtclientgql::CClientRequestManagerCompBase,
			public iauth::ILogin,
			public iauth::IRightsProvider,
			public imtauth::IAccessTokenProvider,
			virtual public imtauth::IUserPermissionsController
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleLoginWrapComp);
		I_REGISTER_INTERFACE(ILogin);
		I_REGISTER_INTERFACE(IRightsProvider);
		I_REGISTER_INTERFACE(IAccessTokenProvider);
		I_REGISTER_INTERFACE(imtauth::IUserPermissionsController);
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user", true, "UserFactory");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
		I_ASSIGN(m_superuserProviderCompPtr, "SuperuserProvider", "Superuser provider", false, "SuperuserProvider");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application Info", true, "");
	I_END_COMPONENT;

	CSimpleLoginWrapComp();

	// reimplemented (iauth::ILogin)
	virtual iauth::CUser* GetLoggedUser() const override;
	virtual bool Login(const QString& userName, const QString& password) override;
	virtual bool Logout() override;

	// Additional login methods
	virtual bool LoginWithRefreshToken(const QString& userName, const QByteArray& refreshToken);
	virtual QByteArray GetRefreshToken() const;

	// reimplemented (iauth::IRightsProvider)
	virtual bool HasRight(
				const QByteArray& operationId,
				bool beQuiet = false) const override;

	// reimplemented (imtauth::IAccessTokenProvider)
	virtual QByteArray GetToken(const QByteArray& userId) const override;

	// reimplemented (imtauth::IUserPermissionsController)
	virtual QByteArrayList GetPermissions(const QByteArray& userId) const override;
	virtual void SetPermissions(const QByteArray& userId, const QByteArrayList& permissions) override;

private:
	mutable iauth::CUser m_loggedUser;
	QByteArray m_loggedUserId;
	QByteArray m_loggedUserPassword;
	QByteArray m_loggedUserToken;
	QByteArray m_loggedUserRefreshToken;
	QByteArrayList m_userPermissionIds;
	imtauth::IUserInfoSharedPtr m_userInfoPtr;
	imtgql::IGqlContextSharedPtr m_gqlContextSharedPtr;

protected:
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
	I_REF(imtauth::ISuperuserProvider, m_superuserProviderCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtauthgql


