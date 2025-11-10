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


const imtauth::IRoleUniquePtr CRoleCollectionAdapterComp::GetRole(const QByteArray& objectId, const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (m_roleCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_roleCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			const IRole* rolePtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			if (rolePtr != nullptr){
				imtauth::IRoleUniquePtr roleInfoUniquePtr(dynamic_cast<imtauth::IRole*>(rolePtr->CloneMe()));
				return roleInfoUniquePtr.PopInterfacePtr();
			}
		}
	}

	return nullptr;
}


} // namespace imtauth


