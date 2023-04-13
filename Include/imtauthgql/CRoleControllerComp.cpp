#include <imtauthgql/CRoleControllerComp.h>


// ImtCore includes
#include <imtauth/CRole.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CFeaturePackage.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CRoleControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString &errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

	const QList<imtgql::CGqlObject>* inputParams = gqlRequest.GetParams();
	QByteArray objectId = GetObjectIdFromInputParams(*inputParams);

	dataModelPtr->SetData("Id", "");
	dataModelPtr->SetData("Name", "");

	if (!inputParams->empty()){
		QByteArray productId = inputParams->at(0).GetFieldArgumentValue("ProductId").toByteArray();
		dataModelPtr->SetData("ProductId", productId);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<const imtauth::CIdentifiableRoleInfo*>(dataPtr.GetPtr());
		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return nullptr;
		}

		QByteArray objectUuid = roleInfoPtr->GetObjectUuid();
		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();
		QByteArray productId = roleInfoPtr->GetProductId();

		dataModelPtr->SetData("Id", objectUuid);
		dataModelPtr->SetData("RoleId", roleId);
		dataModelPtr->SetData("ProductId", productId);
		dataModelPtr->SetData("Name", roleName);

		QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
		dataModelPtr->SetData("Description", description);

		QByteArrayList parentsRolesIds = roleInfoPtr->GetIncludedRoles();
		dataModelPtr->SetData("ParentRoles", parentsRolesIds.join(';'));

		imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();
		dataModelPtr->SetData("Permissions", permissions.join(';'));
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr.PopPtr();
}


istd::IChangeable *CRoleControllerComp::CreateObject(
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

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtauth::CIdentifiableRoleInfo> roleInfoPtr = new imtauth::CIdentifiableRoleInfo();
		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Id")){
			QByteArray id = itemModel.GetData("Id").toByteArray();
			if (!id.isEmpty()){
				objectId = id;
			}
		}

		if (objectId.isEmpty()){
			objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		roleInfoPtr->SetObjectUuid(objectId);

		QByteArray roleId;
		if (itemModel.ContainsKey("RoleId")){
			roleId = itemModel.GetData("RoleId").toByteArray();
			if (roleId.isEmpty()){

				errorMessage = QT_TR_NOOP("Role-ID can't be empty!");
				return nullptr;
			}

			roleInfoPtr->SetRoleId(roleId);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			roleInfoPtr->SetRoleName(name);
		}

		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();
			roleInfoPtr->SetRoleDescription(description);
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

		if (itemModel.ContainsKey("ParentRoles")){
			QByteArray parentRoles = itemModel.GetData("ParentRoles").toByteArray();
			if (!parentRoles.isEmpty()){
				QByteArrayList parentRoleIds = parentRoles.split(';');
				for (const QByteArray& parentRoleId : parentRoleIds){
					if (!roleInfoPtr->IncludeRole(parentRoleId)){
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
				roleInfoPtr->SetLocalPermissions(imtauth::IRole::FeatureIds(permissionIds.begin(), permissionIds.end()));
			}
		}

		return roleInfoPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtauthgql
