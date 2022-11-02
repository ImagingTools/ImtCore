#include <imtauthgql/CUserControllerComp.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtlic/IFeatureInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserControllerComp::GetObject(
		const QList<imtgql::CGqlObject>& inputParams,
		const imtgql::CGqlObject& gqlObject,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	dataModel->SetData("UserId", "");
	dataModel->SetData("Username", "");
	dataModel->SetData("Name", "");
	dataModel->SetData("Password", "");
	dataModel->SetData("Email", "");

	imtbase::CTreeItemModel* permissionsModel = dataModel->AddTreeModel("Permissions");
	imtbase::CTreeItemModel* productsModel = dataModel->AddTreeModel("Products");

	if (m_rolesCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids rolesIds = m_rolesCollectionCompPtr->GetElementIds();

		for (const imtbase::ICollectionInfo::Id& roleId : rolesIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_rolesCollectionCompPtr->GetObjectData(roleId, dataPtr)){
				const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

				QByteArray productId = roleInfoPtr->GetProductId();

				int index = -1;
				for (int i = 0; i < productsModel->GetItemsCount(); i++){
					QByteArray currentProductId = productsModel->GetData("Id", i).toByteArray();

					if (currentProductId == productId){
						index = i;
						break;
					}
				}

				imtbase::CTreeItemModel* rolesModel = nullptr;
				if (index < 0){
					index = productsModel->InsertNewItem();
					productsModel->SetData("Id", productId, index);
					productsModel->SetData("Name", productId, index);
					rolesModel = productsModel->AddTreeModel("Roles", index);
				}
				else{
					rolesModel = productsModel->GetTreeItemModel("Roles", index);
				}

				if (rolesModel != nullptr){
					int roleIndex = rolesModel->InsertNewItem();

					QByteArray id = roleInfoPtr->GetRoleId();
					QString name = roleInfoPtr->GetRoleName();

					QByteArray roleObjectId = id + *m_separatorObjectIdAttrPtr + productId;

					rolesModel->SetData("Id", roleObjectId, roleIndex);
					rolesModel->SetData("Name", name, roleIndex);
					rolesModel->SetData("State", Qt::CheckState::Unchecked, roleIndex);
				}
			}
		}
	}

	QByteArray userId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());

		if (userInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an user info");
			return nullptr;
		}

		QByteArray username = userInfoPtr->GetUsername();
		QString name = userInfoPtr->GetName();
		QByteArray passwordHash = userInfoPtr->GetPasswordHash();
		QString mail = userInfoPtr->GetMail();

		dataModel->SetData("Username", username);
		dataModel->SetData("Name", name);
		dataModel->SetData("Password", passwordHash);
		dataModel->SetData("Email", mail);

		imtauth::IUserInfo::RoleIds roles = userInfoPtr->GetRoles();

		for (const QByteArray& roleId : roles){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_rolesCollectionCompPtr->GetObjectData(roleId, dataPtr)){
				const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

				if (roleInfoPtr != nullptr){
					QByteArray productId = roleInfoPtr->GetProductId();
					QByteArray productRoleId = roleInfoPtr->GetRoleId();

					for (int i = 0; i < productsModel->GetItemsCount(); i++){
						QByteArray productIdFromModel = productsModel->GetData("Id", i).toByteArray();

						if (productIdFromModel == productId){
							imtbase::CTreeItemModel* rolesModel = productsModel->GetTreeItemModel("Roles", i);

							if (rolesModel != nullptr){
								for (int j = 0; j < rolesModel->GetItemsCount(); j++){
									QByteArray roleIdFromModel = rolesModel->GetData("Id", j).toByteArray();
									QStringList data = QString(roleIdFromModel).split(*m_separatorObjectIdAttrPtr);
									if (data.size() == 2){
										roleIdFromModel = data[0].toUtf8();
										if (roleIdFromModel == productRoleId){
											rolesModel->SetData("State", Qt::CheckState::Checked, j);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable* CUserControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray &objectId,
		QString &name,
		QString &description,
		QString& errorMessage) const
{
	if (!m_userInfoFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create User: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtauth::IUserInfo> userInfoPtr = m_userInfoFactCompPtr.CreateInstance();
		if (userInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		QByteArray username;
		if (itemModel.ContainsKey("Username")){
			username = itemModel.GetData("Username").toByteArray();

			if (username.isEmpty()){
				errorMessage = QT_TR_NOOP("Username can't be empty!");

				return nullptr;
			}

			objectId = username;

			userInfoPtr->SetUsername(username);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			userInfoPtr->SetName(name);
		}

		if (itemModel.ContainsKey("Password")){
			QByteArray password = itemModel.GetData("Password").toByteArray();

			bool calculate = true;
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(username, dataPtr)){
				const imtauth::IUserInfo* currentuserInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
				if (currentuserInfoPtr != nullptr){
					QByteArray currentPasswordHash = currentuserInfoPtr->GetPasswordHash();
					if (currentPasswordHash == password){
						calculate = false;
					}
				}
			}

			if (calculate){
				if (m_hashCalculatorCompPtr.IsValid()){
					password = m_hashCalculatorCompPtr->Calculate(username + password);
				}
			}

			userInfoPtr->SetPasswordHash(password);
		}

		if (itemModel.ContainsKey("Email")){
			QString mail = itemModel.GetData("Email").toString();
			userInfoPtr->SetMail(mail);
		}

		if (itemModel.ContainsKey("Permissions")){
			imtbase::CTreeItemModel* permissionsModel = itemModel.GetTreeItemModel("Permissions");
			imtauth::IUserInfo::FeatureIds permissions;

			for(int index = 0; index < permissionsModel->GetItemsCount(); index++){
				QByteArray featureId;
				if (permissionsModel->ContainsKey("PermissionId")){
					featureId = permissionsModel->GetData("PermissionId", index).toByteArray();
					if (!featureId.isEmpty()){
						permissions << featureId;
					}
				}
			}

			userInfoPtr->SetLocalPermissions(permissions);
		}

		if (itemModel.ContainsKey("Products")){
			imtbase::CTreeItemModel* productsModel = itemModel.GetTreeItemModel("Products");
			imtauth::IUserInfo::RoleIds roles;

			for(int i = 0; i < productsModel->GetItemsCount(); i++){
				if (productsModel->ContainsKey("Id", i)){

					QByteArray productId = productsModel->GetData("Id", i).toByteArray();

					imtbase::CTreeItemModel* rolesModel = productsModel->GetTreeItemModel("Roles", i);

					for (int j = 0; j < rolesModel->GetItemsCount(); j++){
						QByteArray roleId = rolesModel->GetData("Id", j).toByteArray();
						int state = rolesModel->GetData("State", j).toInt();

						if (state == Qt::CheckState::Checked){
							roles << roleId;
						}
					}
				}
			}

			userInfoPtr->SetRoles(roles);
		}

		return userInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create user: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtauthgql


