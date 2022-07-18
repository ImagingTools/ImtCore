#pragma once


// ACF includes
#include <iser/IObject.h>

namespace imtauth
{


/**
    Interface for describing an user.
    \ingroup User
*/
class IUserInfo: virtual public iser::IObject
{
public:

    /**
        Get id of the user.
    */
    virtual QByteArray GetId() const = 0;

    /**
        Set id of the user.
    */
    virtual bool SetId(QByteArray id) = 0;

    /**
        Get username of the user.
    */
    virtual QString GetUsername() const = 0;

    /**
        Set username of the user.
    */
    virtual bool SetUsername(QString username) = 0;

    /**
        Get name of the user.
    */
    virtual QString GetName() const = 0;

    /**
        Set name of the user.
    */
    virtual bool SetName(QString name) = 0;

    /**
        Get password hash of the user.
    */
    virtual QByteArray GetPasswordHash() const = 0;

    /**
        Set password hash of the user.
    */
    virtual bool SetPasswordHash(QByteArray passwordHash) = 0;

    /**
        Get e-mail of the user.
    */
    virtual QString GetMail() const = 0;

    /**
        Set e-mail of the user.
    */
    virtual bool SetMail(QString mail) = 0;

};


} // namespace imtauth


