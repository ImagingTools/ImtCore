#include <imtauthgql/CRoleControllerComp.h>


// ImtCore includes
#include <imtauth/CRole.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CRoleControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> dataModelPtr = new imtbase::CTreeItemModel();

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
	QByteArray objectId = GetObjectIdFromInputParams(inputParams);

	dataModelPtr->SetData("Id", "");
	dataModelPtr->SetData("Name", "");

	if (!inputParams.empty()){
		QByteArray productId = inputParams.at(0).GetFieldArgumentValue("ProductId").toByteArray();
		dataModelPtr->SetData("ProductId", productId);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");

			return nullptr;
		}

		QByteArray objectUuid = objectId;
		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();
		QString roleDescription = roleInfoPtr->GetRoleDescription();
		QByteArray productId = roleInfoPtr->GetProductId();

		dataModelPtr->SetData("Id", objectUuid);
		dataModelPtr->SetData("RoleId", roleId);
		dataModelPtr->SetData("ProductId", productId);
		dataModelPtr->SetData("Name", roleName);

		dataModelPtr->SetData("Description", roleDescription);

		QByteArrayList parentsRolesIds = roleInfoPtr->GetIncludedRoles();
		dataModelPtr->SetData("ParentRoles", parentsRolesIds.join(';'));

		imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();
		dataModelPtr->SetData("Permissions", permissions.join(';'));
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr->SetExternTreeModel("data", dataModelPtr.PopPtr());

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CRoleControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_roleFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);

		return nullptr;
	}

	objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		imtauth::IRole* roleInstancePtr = m_roleFactCompPtr.CreateInstance();
		if (roleInstancePtr == nullptr){
			return nullptr;
		}

		imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<imtauth::CIdentifiableRoleInfo*>(roleInstancePtr);
		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		roleInfoPtr->SetObjectUuid(objectId);

		QByteArray roleId;
		if (itemModel.ContainsKey("RoleId")){
			roleId = itemModel.GetData("RoleId").toByteArray();
			if (roleId.isEmpty()){
				errorMessage = QT_TR_NOOP("Role-ID can't be empty!");
				return nullptr;
			}
		}

		QByteArray productId;
		if (itemModel.ContainsKey("ProductId")){
			productId = itemModel.GetData("ProductId").toByteArray();
			if (productId.isEmpty()){
				errorMessage = QT_TR_NOOP("Product-ID can't be empty!");
				return nullptr;
			}

			roleInfoPtr->SetProductId(productId);
		}

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (imtbase::ICollectionInfo::Id collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				imtauth::IRole* currentRoleInfoPtr = dynamic_cast<imtauth::IRole*>(dataPtr.GetPtr());
				if (currentRoleInfoPtr != nullptr){
					if (collectionId != objectId){
						QByteArray currentRoleId = currentRoleInfoPtr->GetRoleId();
						QByteArray currentProductId = currentRoleInfoPtr->GetProductId();

						if (currentRoleId == roleId && currentProductId == productId){
							errorMessage = QT_TR_NOOP("Role already exists");
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
				for (const QByteArray& parentRoleId : parentRoleIds){
					if (parentRoleId == objectId || !roleInfoPtr->IncludeRole(parentRoleId)){
						errorMessage = QT_TR_NOOP(QString("Unable include role %1 to the role %2. Check the dependencies between them.")
									.arg(qPrintable(parentRoleId))
									.arg(qPrintable(roleId)));

						return nullptr;
					}
				}
			}
		}

		if (itemModel.ContainsKey("Permissions")){
			QByteArray permissions = itemModel.GetData("Permissions").toByteArray();
			if (!permissions.isEmpty()){
				QByteArrayList permissionIds = permissions.split(';');

				roleInfoPtr->SetLocalPermissions(permissionIds);
			}
		}

		return roleInfoPtr;
	}

	return nullptr;
}


} // namespace imtauthgql


