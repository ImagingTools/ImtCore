#pragma once


// ACF includes
#include <iser/IObject.h>

namespace imtauth
{


/**
    Interface for describing an user role.
    \ingroup User
*/
class IUserRoleInfo: virtual public iser::IObject
{
public:

    /**
        Get id of the role.
    */
    virtual QByteArray GetId() const = 0;

    /**
        Set id of the role.
    */
    virtual bool SetId(QByteArray id) = 0;

    /**
        Get name of the role.
    */
    virtual QString GetName() const = 0;

    /**
        Set name of the role.
    */
    virtual bool SetName(QString name) = 0;

    /**
        Get description of the role.
    */
    virtual QString GetDescription() const = 0;

    /**
        Set description of the role.
    */
    virtual bool SetDescription(QString name) = 0;

};


} // namespace imtauth


