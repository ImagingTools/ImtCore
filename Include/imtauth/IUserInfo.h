#pragma once


// ACF includes
#include <iser/IObject.h>
#include <imtauth/IRole.h>

namespace imtauth
{


/**
    Interface for describing an user.
    \ingroup User
*/
class IUserInfo:
		virtual public iser::IObject,
		virtual public imtauth::IRole
{
public:

    /**
        Get id of the user.
    */
    virtual QByteArray GetId() const = 0;

    /**
        Set id of the user.
    */
	virtual void SetId(QByteArray id) = 0;

    /**
        Get username of the user.
    */
    virtual QString GetUsername() const = 0;

    /**
        Set username of the user.
    */
	virtual void SetUsername(QString username) = 0;

    /**
        Get name of the user.
    */
    virtual QString GetName() const = 0;

    /**
        Set name of the user.
    */
	virtual void SetName(QString name) = 0;

    /**
        Get password hash of the user.
    */
    virtual QByteArray GetPasswordHash() const = 0;

    /**
        Set password hash of the user.
    */
	virtual void SetPasswordHash(QByteArray passwordHash) = 0;

    /**
        Get e-mail of the user.
    */
    virtual QString GetMail() const = 0;

    /**
        Set e-mail of the user.
    */
	virtual void SetMail(QString mail) = 0;


};


} // namespace imtauth


