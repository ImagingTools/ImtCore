#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


class IUserManager: virtual public istd::IPolymorphic
{
public:
	virtual QByteArrayList GetUserIds() const = 0;
	virtual imtauth::IUserInfoUniquePtr GetUser(const QByteArray& userId) const = 0;
	virtual bool RemoveUser(const QByteArray& userId) = 0;
	virtual QByteArray CreateUser(const QString& userName, const QByteArray& login, const QByteArray& password, const QString& email) = 0;
	virtual bool ChangeUserPassword(const QByteArray& login, const QByteArray& oldPassword, const QByteArray& newPassword) = 0;
	virtual bool AddRolesToUser(const QByteArray& userId, const QByteArrayList& roleIds) = 0;
	virtual bool RemoveRolesFromUser(const QByteArray& userId, const QByteArrayList& roleIds) = 0;
	virtual QByteArrayList GetUserPermissions(const QByteArray& userId) const = 0;
};


} // namespace imtauth


