#pragma once


// ACF includes
#include <iser/IObject.h>

namespace imtauth
{


/**
    Interface for describing an user group.
    \ingroup User
*/
class IUserGroupInfo: virtual public iser::IObject
{
public:

    /**
        Get id of the group.
    */
    virtual QByteArray GetId() const = 0;

    /**
        Set id of the group.
    */
    virtual bool SetId(QByteArray id) = 0;

    /**
        Get name of the group.
    */
    virtual QString GetName() const = 0;

    /**
        Set name of the group.
    */
    virtual bool SetName(QString name) = 0;

    /**
        Get description of the group.
    */
    virtual QString GetDescription() const = 0;

    /**
        Set description of the group.
    */
    virtual bool SetDescription(QString name) = 0;

};


} // namespace imtauth


