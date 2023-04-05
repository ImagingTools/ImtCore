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
        Get id of the group.
    */
    virtual QByteArray GetId() const = 0;

    /**
        Set id of the group.
    */
    virtual bool SetId(QByteArray id) = 0;

    /**
        Get description of the group.
    */
    virtual QString GetDescription() const = 0;

    /**
        Set description of the group.
    */
    virtual bool SetDescription(QString name) = 0;

    /**
		Get roles.
	*/
	virtual RoleIds GetUsers() const = 0;

	/**
		Set roles.
	*/
	virtual void SetUsers(const UserIds &users) = 0;

    /**
		Get list of all availiable users.
	*/
	virtual const imtauth::IUserInfoProvider* GetUserProvider() const = 0;


};


} // namespace imtauth


