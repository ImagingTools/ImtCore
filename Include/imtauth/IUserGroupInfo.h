#pragma once


// ACF includes
#include <istd/TPointerVector.h>

//ImtCore includes
#include <imtauth/IUserBaseInfo.h>
#include <imtauth/IUserInfoProvider.h>
#include <imtauth/IUserGroupInfoProvider.h>


namespace imtauth
{


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
	virtual void AddParentGroup(const QByteArray& parentGroupId) = 0;

	/**
		Remove parent group from this group.
	*/
	virtual bool RemoveParentGroup(const QByteArray& parentGroupId) = 0;

	/**
		Get list of all availiable users.
	*/
	virtual const imtauth::IUserInfoProvider* GetUserProvider() const = 0;

	/**
		Get list of all availiable user groups.
	*/
	virtual const imtauth::IUserGroupInfoProvider* GetUserGroupProvider() const = 0;
};


} // namespace imtauth


