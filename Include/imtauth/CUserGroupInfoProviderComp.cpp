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


imtauth::IUserGroupInfoSharedPtr CUserGroupInfoProviderComp::GetUserGroup(const QByteArray& groupId) const
{
	if (m_userGroupCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
			IUserGroupInfoSharedPtr retVal;
			retVal.SetCastedPtr(dataPtr);

			return retVal;
		}
	}

	return nullptr;
}


} // namespace imtauth


