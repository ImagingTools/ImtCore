// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserGroupManager.h>
#include <imtauthgql/CClientRequestManagerCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Groups.h>


namespace imtauthgql
{


class CClientRequestGroupManagerComp:
			public imtauthgql::CClientRequestManagerCompBase,
			virtual public imtauth::IUserGroupManager
{
public:
	typedef imtauthgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestGroupManagerComp)
		I_REGISTER_INTERFACE(imtauth::IUserGroupManager);
		I_ASSIGN(m_userGroupFactoryCompPtr, "UserGroupFactory", "User group factory", true, "UserGroupFactory");
	I_END_COMPONENT;

	// reimplemented (imtauth::IUserGroupManager)
	virtual QByteArrayList GetGroupIds() const override;
	virtual QByteArray CreateGroup(const QString& groupName, const QString& description) override;
	virtual bool RemoveGroup(const QByteArray& groupId) override;
	virtual imtauth::IUserGroupInfoUniquePtr GetGroup(const QByteArray& groupId) const override;
	virtual bool AddUsersToGroup(const QByteArray& groupId, const QByteArrayList& userIds) override;
	virtual bool RemoveUsersFromGroup(const QByteArray& groupId, const QByteArrayList& userIds) override;
	virtual bool AddRolesToGroup(const QByteArray& groupId, const QByteArrayList& roleIds) override;
	virtual bool RemoveRolesFromGroup(const QByteArray& groupId, const QByteArrayList& roleIds) override;

private:
	bool GetGroupDataSdl(const QByteArray& groupId, sdl::imtauth::Groups::CGroupData::V1_0& groupData) const;
	bool SetGroupDataSdl(const QByteArray& groupId, const sdl::imtauth::Groups::CGroupData::V1_0& groupData) const;

private:
	I_FACT(imtauth::IUserGroupInfo, m_userGroupFactoryCompPtr);
};


} // namespace imtauthgql


