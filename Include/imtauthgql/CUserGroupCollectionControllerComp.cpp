#include <imtauthgql/CUserGroupCollectionControllerComp.h>

// ACF includes
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

	gqlRequest.GetFields();
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		imtauth::CUserGroupInfo* userGroupInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, orderDataPtr)){
			userGroupInfoPtr = dynamic_cast<imtauth::CUserGroupInfo*>(orderDataPtr.GetPtr());
		}

		if (userGroupInfoPtr != nullptr){
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id"){
					QByteArray objectUuid = userGroupInfoPtr->GetId();
					elementInformation = objectUuid;
				}
				else if(informationId == "Name"){
					elementInformation = userGroupInfoPtr->GetName();
				}
				else if(informationId == "Description"){
					elementInformation = userGroupInfoPtr->GetDescription();
				}
				else if(informationId == "Added"){
					idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetElementMetaInfo(collectionId);
					if (metaInfoPtr.IsValid()){
						elementInformation = metaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
								.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
					}
				}
				else if(informationId == "LastModified"){
					idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetElementMetaInfo(collectionId);
					if (metaInfoPtr.IsValid()){
						elementInformation = metaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
								.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
					}
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


