#include <imtauth/CRoleCollection.h>


namespace imtauth
{


// public methods

CRoleCollection::CRoleCollection()
    :BaseClass("Role", "Role", "Roles")
{
}


// reimplemented (IRoleInfoProvider)

const imtbase::ICollectionInfo& CRoleCollection::GetRoleList() const
{
    return m_collection;
}


const IRole* CRoleCollection::GetRole(const QByteArray& roleId) const
{
    return dynamic_cast<const IRole*>(m_collection.GetObjectPtr(roleId));
}


} // namespace imtauth


