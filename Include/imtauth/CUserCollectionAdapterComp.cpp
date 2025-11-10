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


const imtauth::IUserInfoUniquePtr CUserCollectionAdapterComp::GetUser(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr) const
{
	if (m_userCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(objectId, dataPtr, paramsPtr)){
			const imtauth::IUserInfo* userPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userPtr != nullptr){
				imtauth::IUserInfoUniquePtr userInfoUniquePtr(dynamic_cast<imtauth::IUserInfo*>(userPtr->CloneMe()));
				return userInfoUniquePtr.PopInterfacePtr();
			}
		}
	}

	return nullptr;
}


} // namespace imtauth


