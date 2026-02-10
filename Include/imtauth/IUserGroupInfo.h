// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserBaseInfo.h>



namespace imtauth
{



class IUserInfoProvider;


/**
	Interface for describing an user group.
	\ingroup User
*/
class IUserGroupInfo: virtual public IUserBaseInfo
{
public:
	typedef QByteArrayList UserIds;
	typedef QByteArrayList GroupIds;

	/**
		Get users.
	*/
	virtual UserIds GetUsers() const = 0;

	/**
		Set users.
	*/
	virtual void SetUsers(const UserIds& users) = 0;

	/**
		Add user to group.
	*/
	virtual void AddUser(const QByteArray& userId) = 0;

	/**
		Remove user from this group.
	*/
	virtual bool RemoveUser(const QByteArray& userId) = 0;

	/**
		Get parent groups.
	*/
	virtual GroupIds GetParentGroups() const = 0;

	/**
		Add parent group to this group.
	*/
	virtual bool AddParentGroup(const QByteArray& parentGroupId) = 0;

	/**
		Remove parent group from this group.
	*/
	virtual bool RemoveParentGroup(const QByteArray& parentGroupId) = 0;

	/**
		Get list of all availiable users.
	*/
	virtual const imtauth::IUserInfoProvider* GetUserProvider() const = 0;
};


typedef istd::TUniqueInterfacePtr<IUserGroupInfo> IUserGroupInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IUserGroupInfo> IUserGroupInfoSharedPtr;


} // namespace imtauth


