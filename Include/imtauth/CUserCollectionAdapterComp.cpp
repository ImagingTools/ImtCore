// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserCollectionAdapterComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IUserInfo.h>


namespace imtauth
{


// reimplemented (IRoleInfoProvider)

const imtbase::ICollectionInfo& CUserCollectionAdapterComp::GetUserList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	if (m_userCollectionCompPtr.IsValid()){
		return *m_userCollectionCompPtr;
	}

	return emptyCollection;
}


imtauth::IUserInfoUniquePtr CUserCollectionAdapterComp::GetUser(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr) const
{
	if (m_userCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(objectId, dataPtr, paramsPtr)){
			const imtauth::IUserInfo* userPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userPtr != nullptr){
				imtauth::IUserInfoUniquePtr userInfoUniquePtr;
				userInfoUniquePtr.MoveCastedPtr(userPtr->CloneMe());

				return userInfoUniquePtr;
			}
		}
	}

	return nullptr;
}


} // namespace imtauth


