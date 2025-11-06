#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/IUserManager.h>
#include <imtauth/IAccessTokenProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtauthgql/CUserRepresentationController.h>


namespace imtauthgql
{


class CClientRequestUserManagerComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public imtauth::IUserManager
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestUserManagerComp)
		I_REGISTER_INTERFACE(imtauth::IUserManager);
		I_ASSIGN(m_userFactoryCompPtr, "UserFactory", "User factory", true, "UserFactory");
		I_ASSIGN(m_accessTokenProviderCompPtr, "AccessTokenProvider", "Access token provider", false, "AccessTokenProvider");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
	I_END_COMPONENT;

	// reimplemented (imtauth::IUserManager)
	virtual QByteArrayList GetUserIds() const override;
	virtual imtauth::IUserInfoUniquePtr GetUser(const QByteArray& userId) const override;
	virtual bool RemoveUser(const QByteArray& userId) override;
	virtual QByteArray CreateUser(const QString& userName, const QByteArray& login, const QByteArray& password, const QString& email) override;
	virtual bool ChangeUserPassword(const QByteArray& login, const QByteArray& oldPassword, const QByteArray& newPassword) override;
	virtual bool AddRolesToUser(const QByteArray& userId, const QByteArrayList& roleIds) override;
	virtual bool RemoveRolesFromUser(const QByteArray& userId, const QByteArrayList& roleIds) override;
	virtual QByteArrayList GetUserPermissions(const QByteArray& userId) const override;

private:
	bool GetUserDataSdl(const QByteArray& userId, sdl::imtauth::Users::CUserData::V1_0& userData) const;
	bool SetUserDataSdl(const QByteArray& userId, const sdl::imtauth::Users::CUserData::V1_0& userData) const;

	template<class Arguments, class Payload, class SdlRequest>
	bool SendModelRequestInternal(Arguments arguments, Payload& payload) const;

private:
	I_FACT(imtauth::IUserInfo, m_userFactoryCompPtr);
	I_REF(imtauth::IAccessTokenProvider, m_accessTokenProviderCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);

private:
	imtauthgql::CUserRepresentationController m_userRepresentationController;
};


} // namespace imtauthgql


