#include <imtauthgql/CRoleControllerComp.h>


// ImtCore includes
#include <imtauth/CRole.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CRoleControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get role object. Error: Attribute 'm_objectCollectionCompPtr' was not set").toUtf8();
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to get role object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();

	imtauth::IRole* roleInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		roleInfoPtr = dynamic_cast<imtauth::IRole*>(dataPtr.GetPtr());
	}

	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to get role with ID: '%1'. The role does not exist.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("Id", objectId);
	dataModelPtr->SetData("RoleId", roleInfoPtr->GetRoleId());
	dataModelPtr->SetData("ProductId", roleInfoPtr->GetProductId());
	dataModelPtr->SetData("Name", roleInfoPtr->GetRoleName());
	dataModelPtr->SetData("Description", roleInfoPtr->GetRoleDescription());

	QByteArrayList parentsRolesIds = roleInfoPtr->GetIncludedRoles();
	std::sort(parentsRolesIds.begin(), parentsRolesIds.end());
	dataModelPtr->SetData("ParentRoles", parentsRolesIds.join(';'));

	imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();
	std::sort(permissions.begin(), permissions.end());
	dataModelPtr->SetData("Permissions", permissions.join(';'));

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CRoleControllerComp::CreateObjectFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		QByteArray& objectId,
		QString& name,
		QString& description,
		QString& errorMessage) const
{
	if (!m_roleFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to create a role object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QByteArray productId = gqlRequest.GetHeader("ProductId");

	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	if (productId.isEmpty()){
		errorMessage = QString("Unable to create a role object for empty product.").toUtf8();
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	QByteArray itemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	imtbase::CTreeItemModel itemModel;
	if (!itemModel.CreateFromJson(itemData)){
		errorMessage = QString("Unable to create a role object. Error: Failed to create a tree model from json '%1'").arg(qPrintable(itemData));
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtauth::CIdentifiableRoleInfo> roleInfoPtr;
	roleInfoPtr.SetCastedOrRemove(m_roleFactCompPtr.CreateInstance());
	if (!roleInfoPtr.IsValid()){
		errorMessage = QString("Unable to create a role instance.").toUtf8();
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	roleInfoPtr->SetObjectUuid(objectId);

	QByteArray roleId = itemModel.GetData("RoleId").toByteArray();

	if (roleId.isEmpty()){
		errorMessage = QString("Role-ID can't be empty!");
		return nullptr;
	}

	roleInfoPtr->SetProductId(productId);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			imtauth::IRole* currentRoleInfoPtr = dynamic_cast<imtauth::IRole*>(dataPtr.GetPtr());
			if (currentRoleInfoPtr != nullptr){
				if (collectionId != objectId){
					QByteArray currentRoleId = currentRoleInfoPtr->GetRoleId();
					QByteArray currentProductId = currentRoleInfoPtr->GetProductId();

					if (currentRoleId == roleId && currentProductId == productId){
						errorMessage = QString("Role with ID: '%1' already exists").arg(qPrintable(currentRoleId));
						return nullptr;
					}
				}
			}
		}
	}

	roleInfoPtr->SetRoleId(roleId);

	if (itemModel.ContainsKey("Name")){
		name = itemModel.GetData("Name").toString();
		roleInfoPtr->SetRoleName(name);
	}

	if (itemModel.ContainsKey("Description")){
		description = itemModel.GetData("Description").toString();
		roleInfoPtr->SetRoleDescription(description);
	}

	if (itemModel.ContainsKey("ParentRoles")){
		QByteArray parentRoles = itemModel.GetData("ParentRoles").toByteArray();
		if (!parentRoles.isEmpty()){
			QByteArrayList parentRoleIds = parentRoles.split(';');
			parentRoleIds.removeAll("");

			for (const QByteArray& parentRoleId : parentRoleIds){
				if (parentRoleId == objectId || !roleInfoPtr->IncludeRole(parentRoleId)){
					errorMessage = QT_TR_NOOP(QString("Unable include role '%1' to the role '%2'. Check the dependencies between them.")
												.arg(qPrintable(parentRoleId))
												.arg(qPrintable(roleId)));

					return nullptr;
				}
			}
		}
	}

	if (itemModel.ContainsKey("Permissions")){
		QByteArray permissions = itemModel.GetData("Permissions").toByteArray();
		QByteArrayList permissionIds = permissions.split(';');
		permissionIds.removeAll("");

		roleInfoPtr->SetLocalPermissions(permissionIds);
	}

	return roleInfoPtr.PopPtr();
}


} // namespace imtauthgql


