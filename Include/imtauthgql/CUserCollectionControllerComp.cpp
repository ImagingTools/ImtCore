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

	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!paramsPtr.empty()){
		productId = paramsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QByteArray userId = GetObjectIdFromInputParams(paramsPtr);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			int index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Roles", index);

			imtbase::CTreeItemModel* children = dataModel->AddTreeModel("Children", index);

			if (m_roleInfoProviderCompPtr.IsValid()){
				imtauth::IUserInfo::RoleIds rolesIds = userInfoPtr->GetRoles(productId);
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

			index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Groups", index);

			children = dataModel->AddTreeModel("Children", index);

			if (m_userGroupInfoProviderCompPtr.IsValid()){
				QByteArrayList groupIds = userInfoPtr->GetGroups();
				for (const QByteArray& groupId : groupIds){
					const imtauth::IUserGroupInfo* userGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
					if (userGroupInfoPtr != nullptr){
						QString groupName = userGroupInfoPtr->GetName();

						int childrenIndex = children->InsertNewItem();
						children->SetData("Value", groupName, childrenIndex);
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

	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!paramsPtr.empty()){
		productId = paramsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
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

		bool ok = false;
		if (contextUserInfoPtr != nullptr){
			ok = contextUserInfoPtr->IsAdmin();
		}

		if (!ok){
			if (userInfoPtr != nullptr){
				ok = !userInfoPtr->IsAdmin();
			}
		}

		if (contextPtr == nullptr){
			ok = true;
		}

		if (ok){
			idoc::MetaInfoPtr elementMetaInfo = objectCollectionIterator->GetDataMetaInfo();
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "Id"){
					elementInformation = QString(collectionId);
				}
				else if(informationId == "UserId"){
					elementInformation = userInfoPtr->GetId();
				}
				else if(informationId == "Name"){
					elementInformation = userInfoPtr->GetName();
				}
				else if(informationId == "Description"){
					elementInformation = userInfoPtr->GetDescription();
				}
				else if(informationId == "Mail"){
					elementInformation = userInfoPtr->GetMail();
				}
				else if(informationId == "Roles"){
					QByteArrayList resultList;
					if (m_roleInfoProviderCompPtr.IsValid()){
						for (const QByteArray& roleId: userInfoPtr->GetRoles(productId)){
							const imtauth::IRole* roleInfoPtr = m_roleInfoProviderCompPtr->GetRole(roleId);
							if (roleInfoPtr != nullptr){
								QString roleName = roleInfoPtr->GetRoleName();
								QString roleDescription = roleInfoPtr->GetRoleDescription();

								QString result = roleName;
								if (!roleDescription.isEmpty()){
									result += " (" + roleDescription + ")";
								}
								resultList << result.toUtf8();
							}
						}
					}
					elementInformation = resultList.join(';');
				}
				else if(informationId == "Groups"){
					QByteArrayList resultList;
					if (m_userGroupInfoProviderCompPtr.IsValid()){
						for (const QByteArray& groupId: userInfoPtr->GetGroups()){
							const imtauth::IUserGroupInfo* userGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
							if (userGroupInfoPtr != nullptr){
								QString groupName = userGroupInfoPtr->GetName();
								QString groupDescription = userGroupInfoPtr->GetDescription();

								QString result = groupName;
								if (!groupDescription.isEmpty()){
									result += " (" + groupDescription + ")";
								}

								resultList << result.toUtf8();
							}
						}
					}
					elementInformation = resultList.join(';');
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

		return true;
	}

	return false;
}


} // namespace imtauth


