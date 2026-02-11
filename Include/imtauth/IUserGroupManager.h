// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IUserGroupInfo.h>


namespace imtauth
{


class IUserGroupManager: virtual public istd::IPolymorphic
{
public:
	virtual QByteArrayList GetGroupIds() const = 0;
	virtual QByteArray CreateGroup(const QString& groupName, const QString& description) = 0;
	virtual bool RemoveGroup(const QByteArray& groupId) = 0;
	virtual IUserGroupInfoUniquePtr GetGroup(const QByteArray& groupId) const = 0;
	virtual bool AddUsersToGroup(const QByteArray& groupId, const QByteArrayList& userIds) = 0;
	virtual bool RemoveUsersFromGroup(const QByteArray& groupId, const QByteArrayList& userIds) = 0;
	virtual bool AddRolesToGroup(const QByteArray& groupId, const QByteArrayList& roleIds) = 0;
	virtual bool RemoveRolesFromGroup(const QByteArray& groupId, const QByteArrayList& roleIds) = 0;
};


} // namespace imtauth


