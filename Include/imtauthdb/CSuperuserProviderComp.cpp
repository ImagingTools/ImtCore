#include <imtauthdb/CSuperuserProviderComp.h>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtauth::ISuperuserProvider)

bool CSuperuserProviderComp::SuperuserExists() const
{
	if (m_userCollectionCompPtr.IsValid()) {
		iprm::CParamsSet filterParam;
		iprm::CParamsSet paramsSet;

		iprm::CTextParam suId;
		suId.SetText(*m_superuserIdAttrPtr);

		paramsSet.SetEditableParameter("Id", &suId);
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

		imtbase::ICollectionInfo::Ids userObjectIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		for (const imtbase::ICollectionInfo::Id& userObjectId : userObjectIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
				const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
				if (userInfoPtr != nullptr){
					if (*m_superuserIdAttrPtr == userInfoPtr->GetId()){
						return true;
					}
				}
			}
		}
	}

	return false;
}


QByteArray CSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthdb


