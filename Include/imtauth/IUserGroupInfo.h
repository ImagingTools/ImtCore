#pragma once


// ACF includes
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
};


} // namespace imtauth


