#include <imtauthgql/CUserCollectionControllerComp.h>


// STL includes
#include <cmath>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtauth/CUserInfo.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CUserCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);

	if (metaInfo.IsValid()){
		if (informationId == QByteArray("UserId")){
			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_USERNAME);
		}
		else if (informationId == QByteArray("Email")){
			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_EMAIL);
		}
		else if (informationId == QByteArray("Added")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("LastModified")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CUserCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QByteArray userId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			if (m_roleInfoProviderCompPtr.IsValid()){
				int index = dataModel->InsertNewItem();
				dataModel->SetData("Name", "Roles", index);

				imtbase::CTreeItemModel* children = dataModel->AddTreeModel("Children", index);

				imtauth::IUserInfo::RoleIds rolesIds = userInfoPtr->GetRoles();
				for (const QByteArray& productRoleId : rolesIds){
					const imtauth::IRole* rolePtr = m_roleInfoProviderCompPtr->GetRole(productRoleId);
					if (rolePtr != nullptr){
						QByteArray roleId = rolePtr->GetRoleId();
						QString roleName = rolePtr->GetRoleName();
						QByteArray productId = rolePtr->GetProductId();

						int childrenIndex = children->InsertNewItem();
						children->SetData("Value", roleName + " (" + productId + ")", childrenIndex);
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


bool CUserCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const imtbase::IObjectCollectionIterator* objectCollectionIterator,
		QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	bool retVal = true;
	QByteArray collectionId = objectCollectionIterator->GetObjectId();
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty()){
		const imtauth::IUserInfo* contextUserInfoPtr = nullptr;
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetGqlContext();
		if (contextPtr != nullptr){
			contextUserInfoPtr = contextPtr->GetUserInfo();
		}

		const imtauth::IUserInfo* userInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr userDataPtr;
		if (objectCollectionIterator->GetObjectData(userDataPtr)){
			userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
		}

		if (userInfoPtr != nullptr && contextUserInfoPtr != nullptr){
			bool ok = false;

			if (!userInfoPtr->IsAdmin()){
				ok = true;
			}
			else{
				ok = contextUserInfoPtr->IsAdmin();
			}

			if (ok){
				idoc::MetaInfoPtr elementMetaInfo = objectCollectionIterator->GetDataMetaInfo();
				for (QByteArray informationId : informationIds){
					QVariant elementInformation;

					if(informationId == "Id"){
						elementInformation = QString(collectionId);
					}
					else if(informationId == "Name"){
						elementInformation = objectCollectionIterator->GetElementInfo("Name");
					}
					else if(informationId == "Description"){
						elementInformation = objectCollectionIterator->GetElementInfo("Description");
					}
					else{
						if (elementMetaInfo.IsValid()){
							if (informationId == QByteArray("Added")){
								elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
										.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
							}
							else if (informationId == QByteArray("LastModified")){
								elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
										.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
							}
						}
					}

					if(elementInformation.isNull()){
						elementInformation = GetObjectInformation(informationId, collectionId);
					}
					if (elementInformation.isNull()){
						elementInformation = "";
					}

					retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
				}
			}
		}

		return true;
	}

	return false;
}


} // namespace imtauth


