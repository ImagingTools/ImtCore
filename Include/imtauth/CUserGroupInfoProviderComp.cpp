// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


imtauth::IUserGroupInfoSharedPtr CUserGroupInfoProviderComp::GetUserGroup(const QByteArray& groupId, const iprm::IParamsSet* paramsPtr) const
{
	if (m_userGroupCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;

		auto cacheIt = m_groupCache.constFind(groupId);
		if (cacheIt != m_groupCache.constEnd()){
			dataPtr = cacheIt.value();
		}
		else if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr, paramsPtr)){
			m_groupCache.insert(groupId, dataPtr);
		}

		if (dataPtr.IsValid()){
			IUserGroupInfoSharedPtr retVal;
			retVal.SetCastedPtr(dataPtr);

			return retVal;
		}
	}

	return nullptr;
}


} // namespace imtauth


