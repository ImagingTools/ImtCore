#pragma once


// ACF includes
#include <iser/IObject.h>

namespace imtauth
{


/**
    Interface for describing an user permission.
    \ingroup User
*/
class IUserPermissionInfo: virtual public iser::IObject
{
public:

    /**
        Get id of the permission.
    */
    virtual QByteArray GetId() const = 0;

    /**
        Set id of the permission.
    */
    virtual bool SetId(QByteArray id) = 0;

    /**
        Get name of the permission.
    */
    virtual QString GetName() const = 0;

    /**
        Set name of the permission.
    */
    virtual bool SetName(QString name) = 0;

    /**
        Get description of the permission.
    */
    virtual QString GetDescription() const = 0;

    /**
        Set description of the permission.
    */
    virtual bool SetDescription(QString name) = 0;

};


} // namespace imtauth


