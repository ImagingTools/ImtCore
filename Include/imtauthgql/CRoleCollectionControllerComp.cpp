#include <imtauthgql/CRoleCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CRole.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

QVariant CRoleCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	if (informationId == QByteArray("Added")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);;
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
	}
	else if (informationId == QByteArray("LastModified")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CRoleCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!paramsPtr.isEmpty()){
		productId = paramsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

	QByteArray roleObjectId = GetObjectIdFromInputParams(paramsPtr);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(roleObjectId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return nullptr;
		}

		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();
		QByteArray roleProductId = roleInfoPtr->GetProductId();

		int index = dataModelPtr->InsertNewItem();
		dataModelPtr->SetData("Name", "Parent Roles", index);
		imtbase::CTreeItemModel* children = dataModelPtr->AddTreeModel("Children", index);

		QByteArrayList parentRolesIds = roleInfoPtr->GetIncludedRoles();

		for (const QByteArray& parentRoleId : parentRolesIds){
			imtbase::IObjectCollection::DataPtr parentDataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(parentRoleId, parentDataPtr)){
				const imtauth::IRole* parentRoleInfoPtr = dynamic_cast<const imtauth::IRole*>(parentDataPtr.GetPtr());
				if (parentRoleInfoPtr != nullptr){
					QString parentRoleName = parentRoleInfoPtr->GetRoleName();

					int childrenIndex = children->InsertNewItem();

					children->SetData("Value", parentRoleName, childrenIndex);
				}
			}
		}

		index = dataModelPtr->InsertNewItem();

		dataModelPtr->SetData("Name", "Permissions", index);
		children = dataModelPtr->AddTreeModel("Children", index);

		imtauth::IRole::FeatureIds permissionsIds = roleInfoPtr->GetPermissions();

		for (const QByteArray& permissionId : permissionsIds){
			int childrenIndex = children->InsertNewItem();

			children->SetData("Value", permissionId, childrenIndex);
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr.PopPtr();
}


bool CRoleCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const imtbase::IObjectCollectionIterator* objectCollectionIterator,
		QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Unable to setup GQL item for role");
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
		const imtauth::IRole* roleInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr roleDataPtr;
		if (objectCollectionIterator->GetObjectData(roleDataPtr)){
			roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
		}

		idoc::MetaInfoPtr elementMetaInfo = objectCollectionIterator->GetDataMetaInfo();
		for (QByteArray informationId : informationIds){
			QVariant elementInformation;

			if(informationId == "Id"){
				elementInformation = QString(collectionId);
			}
			if(informationId == "Name"){
				elementInformation = roleInfoPtr->GetRoleName();
			}
			else if(informationId == "RoleId"){
				elementInformation = roleInfoPtr->GetRoleId();
			}
			else if(informationId == "ProductId"){
				elementInformation = roleInfoPtr->GetProductId();
			}
			else if(informationId == "RoleName"){
				elementInformation = roleInfoPtr->GetRoleName();
			}
			else if(informationId == "ParentRoles"){
				elementInformation = roleInfoPtr->GetIncludedRoles().join(';');
			}
			else if(informationId == "RoleDescription"){
				elementInformation = roleInfoPtr->GetRoleDescription();
			}
			else if(informationId == "Added"){
				QDateTime addedTime =  objectCollectionIterator->GetElementInfo("Added").toDateTime();
				addedTime.setTimeSpec(Qt::UTC);

				elementInformation = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
			}
			else if(informationId == "LastModified"){
				QDateTime lastTime =  objectCollectionIterator->GetElementInfo("LastModified").toDateTime();
				lastTime.setTimeSpec(Qt::UTC);

				elementInformation = lastTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
			}

			if(elementInformation.isNull()){
				elementInformation = GetObjectInformation(informationId, collectionId);
			}
			if (elementInformation.isNull()){
				elementInformation = "";
			}

			retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
		}

		return true;
	}

	return false;
}


} // namespace imtauth


