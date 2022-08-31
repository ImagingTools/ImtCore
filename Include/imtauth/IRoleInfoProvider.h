#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtbase
{
    class ICollectionInfo;
}


namespace imtauth
{

class IRole;


/**
    Interface for a collection of roles.
    \ingroup Role
*/
class IRoleInfoProvider: virtual public istd::IChangeable
{
public:
    enum MetaInfoTypes
    {
        /**
            List of role infos given as QStringList.
        */
        MIT_ROLE_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1
    };

    /**
        Get list of all available roles.
    */
    virtual const imtbase::ICollectionInfo& GetRoleList() const = 0;

    /**
        Get information for role.
    */
    virtual const imtauth::IRole* GetRole(const QByteArray& roleId) const = 0;
};


} // namespace imtauth


