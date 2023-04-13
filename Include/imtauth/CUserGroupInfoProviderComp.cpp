#include <imtauth/CUserGroupInfoProviderComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IUserGroupInfo.h>


namespace imtauth
{


// public methods

// reimplemented (IUserGroupInfoProvider)

const imtbase::ICollectionInfo& CUserGroupInfoProviderComp::GetUserGroupList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	if (m_userGroupCollectionCompPtr.IsValid()){
		return *m_userGroupCollectionCompPtr;
	}

	return emptyCollection;
}


const imtauth::IUserGroupInfo* CUserGroupInfoProviderComp::GetUserGroup(const QByteArray& groupId) const
{
	if (m_userGroupCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
			const IUserGroupInfo* groupPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
			if (groupPtr != nullptr){
				return dynamic_cast<const imtauth::IUserGroupInfo*>(groupPtr->CloneMe());
			}
		}
	}

	return nullptr;
}


} // namespace imtauth


