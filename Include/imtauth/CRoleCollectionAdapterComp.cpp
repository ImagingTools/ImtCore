#include <imtauth/CRoleCollectionAdapterComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IRole.h>

namespace imtauth
{


// reimplemented (IRoleInfoProvider)

const imtbase::ICollectionInfo& CRoleCollectionAdapterComp::GetRoleList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	if (m_roleCollectionCompPtr.IsValid()){
		return *m_roleCollectionCompPtr;
	}

	return emptyCollection;
}


const IRole* CRoleCollectionAdapterComp::GetRole(const QByteArray& roleId, const QByteArray& productId) const
{
	if (m_roleCollectionCompPtr.IsValid()){
		QByteArray objectId = roleId + *m_separatorObjectIdAttrPtr + productId;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_roleCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			const IRole* rolePtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			return dynamic_cast<const imtauth::IRole*>(rolePtr->CloneMe());
		}
	}

	return nullptr;
}


} // namespace imtauth


