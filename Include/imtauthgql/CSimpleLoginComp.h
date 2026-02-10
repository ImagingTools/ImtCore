// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>

// ImtCore includes
#include <imtauth/IAccessTokenController.h>
#include <imtauth/IUserPermissionsController.h>


namespace imtauthgql
{


class CSimpleLoginComp:
			public icomp::CComponentBase,
			virtual public iauth::ILogin,
			virtual public iauth::IRightsProvider,
			virtual public imtauth::IUserPermissionsController,
			virtual public imtauth::IAccessTokenController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleLoginComp);
		I_REGISTER_INTERFACE(ILogin);
		I_REGISTER_INTERFACE(IRightsProvider);
		I_REGISTER_INTERFACE(IAccessTokenProvider);
		I_REGISTER_INTERFACE(IAccessTokenController);
		I_REGISTER_INTERFACE(IUserPermissionsController);
	I_END_COMPONENT;

	// reimplemented (iauth::ILogin)
	virtual iauth::CUser* GetLoggedUser() const override;
	virtual bool Login(const QString& userName, const QString& password) override;
	virtual bool Logout() override;

	// reimplemented (iauth::IRightsProvider)
	virtual bool HasRight(
				const QByteArray& operationId,
				bool beQuiet = false) const override;

	// reimplemented (imtauth::IUserPermissionsController)
	virtual QByteArrayList GetPermissions(const QByteArray& userId) const override;
	virtual void SetPermissions(const QByteArray& userId, const QByteArrayList& permissions) override;

	// reimplemented (imtauth::IAccessTokenProvider)
	virtual QByteArray GetToken(const QByteArray& userId) const override;

	// reimplemented (imtauth::IAccessTokenController)
	virtual bool SetToken(const QByteArray& userId, const QByteArray& accessToken) override;

private:
	mutable iauth::CUser m_loggedUser;
	QByteArray m_loggedUserId;
	QByteArray m_loggedUserPassword;
	QByteArray m_loggedUserToken;
	QByteArrayList m_userPermissionIds;
};


} // namespace imtauthgql


