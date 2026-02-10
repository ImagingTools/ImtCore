// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual QByteArray GetUserObjectId(const QByteArray& login) const = 0;
	virtual IUserInfoUniquePtr GetUser(const QByteArray& userId) const = 0;
	virtual bool RemoveUser(const QByteArray& userId) = 0;
	virtual QByteArray CreateUser(const QString& userName, const QByteArray& login, const QByteArray& password, const QString& email) = 0;
	virtual bool ChangeUserPassword(const QByteArray& userId, const QByteArray& oldPassword, const QByteArray& newPassword) = 0;
	virtual bool AddRolesToUser(const QByteArray& userId, const QByteArray& productId, const QByteArrayList& roleIds) = 0;
	virtual bool RemoveRolesFromUser(const QByteArray& userId, const QByteArray& productId, const QByteArrayList& roleIds) = 0;
	virtual QByteArrayList GetUserPermissions(const QByteArray& userId, const QByteArray& productId) const = 0;
	virtual bool GetUserAuthSystem(const QByteArray& login, imtauth::IUserInfo::SystemInfo& systemInfo) const = 0;
};


} // namespace imtauth


