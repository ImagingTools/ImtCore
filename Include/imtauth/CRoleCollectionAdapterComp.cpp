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

		return GetRole(objectId);
	}

	return nullptr;
}


const IRole* CRoleCollectionAdapterComp::GetRole(const QByteArray &objectId) const
{
	if (m_roleCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_roleCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			const IRole* rolePtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			if (rolePtr != nullptr){
				istd::IChangeable* clonedPtr = rolePtr->CloneMe();
				istd::TDelPtr<const imtauth::IRole> clonedUserInfoPtr;
				clonedUserInfoPtr.SetCastedOrRemove(clonedPtr);
				if (clonedUserInfoPtr.IsValid()){
					return clonedUserInfoPtr.PopPtr();
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtauth


