#pragma once


// ACF includes
#include <istd/TPointerVector.h>

//ImtCore includes
#include <imtauth/IUserBaseInfo.h>
#include <imtauth/IUserInfoProvider.h>


namespace imtauth
{


/**
	Interface for describing an user group.
	\ingroup User
*/
class IUserGroupInfo: virtual public IUserBaseInfo
{
public:
	typedef QSet<QByteArray> UserIds;

	/**
		Get description of the group.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set description of the group.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get users.
	*/
	virtual UserIds GetUsers() const = 0;

	/**
		Set users.
	*/
	virtual void SetUsers(const UserIds& users) = 0;

	/**
		Get list of all availiable users.
	*/
	virtual const imtauth::IUserInfoProvider* GetUserProvider() const = 0;

	/**
		Get the parent group pointer.
	*/
	virtual const IUserGroupInfo* GetParentGroup() const = 0;

	/**
		Get sub groups for this group.
	*/
	virtual const istd::TPointerVector<const IUserGroupInfo>& GetSubGroups() const  = 0;

	/**
		Insert sub group.
	*/
	virtual bool InsertSubGroup(const IUserGroupInfo* subGroupInfo) = 0;

	/**
		Remove sub group.
	*/
	virtual void DeleteSubGroup(const QByteArray& subGroupId) = 0;
};


typedef istd::TPointerVector<const IUserGroupInfo> UserGroupInfoList;


} // namespace imtauth


