#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/CRole.h>


namespace imtauth
{


class CRoleCollection: public imtbase::TAggergatedObjectCollectionWrap<IRoleInfoProvider, CRole>
{
public:
    typedef imtbase::TAggergatedObjectCollectionWrap<IRoleInfoProvider, CRole> BaseClass;

    CRoleCollection();

    // reimplemented (IRoleInfoProvider)
    virtual const imtbase::ICollectionInfo& GetRoleList() const override;
    virtual const IRole* GetRole(const QByteArray& roleId) const override;
};


} // namespace imtauth
