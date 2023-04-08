#include <imtauthgql/CUserGroupCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

bool CUserGroupCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const QByteArray& collectionId,
		QString& errorMessage) const
{
	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr groupDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, groupDataPtr)){
			userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(groupDataPtr.GetPtr());
		}

		if (userGroupInfoPtr != nullptr){
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id"){
					QByteArray objectUuid = userGroupInfoPtr->GetObjectUuid();
					elementInformation = objectUuid;
				}
				else if(informationId == "Name"){
					elementInformation = userGroupInfoPtr->GetName();
				}
				else if(informationId == "Description"){
					elementInformation = userGroupInfoPtr->GetDescription();
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}
		}

		return true;
	}

	return false;
}


} // namespace imtauth


