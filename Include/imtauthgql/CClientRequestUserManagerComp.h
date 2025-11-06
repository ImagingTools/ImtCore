#pragma once


// ImtCore includes
#include <imtauth/IUserManager.h>
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
	I_END_COMPONENT;

	// reimplemented (imtauth::IUserManager)
	virtual QByteArrayList GetUserIds() const override;
	virtual imtauth::IUserInfoUniquePtr GetUser(const QByteArray& userId, const QByteArray& productId = QByteArray()) const override;
	virtual bool RemoveUser(const QByteArray& userId) override;
	virtual QByteArray CreateUser(const QString& userName, const QByteArray& login, const QByteArray& password) override;
	virtual bool ChangeUserPassword(const QByteArray& login, const QByteArray& oldPassword, const QByteArray& newPassword) override;
	virtual bool AddRolesToUser(const QByteArray& userId, const QByteArrayList& roleIds) override;
	virtual bool RemoveRolesFromUser(const QByteArray& userId, const QByteArrayList& roleIds) override;
	virtual QByteArrayList GetUserPermissions(const QByteArray& userId, const QByteArray& productId) const override;

private:
	bool GetUserDataSdl(const QByteArray& userId, sdl::imtauth::Users::CUserData::V1_0& userData) const;
	bool SetUserDataSdl(const QByteArray& userId, const sdl::imtauth::Users::CUserData::V1_0& userData) const;

private:
	I_FACT(imtauth::IUserInfo, m_userFactoryCompPtr);

private:
	imtauthgql::CUserRepresentationController m_userRepresentationController;
};


} // namespace imtauthgql


