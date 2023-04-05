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


const IUserInfo* CUserCollectionAdapterComp::GetUser(const QByteArray &objectId) const
{
	if (m_userCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			const imtauth::IUserInfo* userPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userPtr != nullptr){
				return dynamic_cast<const imtauth::IUserInfo*>(userPtr->CloneMe());
			}
		}
	}

	return nullptr;
}


} // namespace imtauth


