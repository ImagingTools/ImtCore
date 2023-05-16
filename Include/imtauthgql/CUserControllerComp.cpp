#include <imtauthgql/CUserControllerComp.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtlic/IFeatureInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	dataModel->SetData("UserId", "");
	dataModel->SetData("Username", "");
	dataModel->SetData("Name", "");
	dataModel->SetData("Password", "");
	dataModel->SetData("Email", "");

	QByteArray userId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			QByteArray objectUuid = userId;
			QByteArray username = userInfoPtr->GetId();
			QString name = userInfoPtr->GetName();
			QByteArray passwordHash = userInfoPtr->GetPasswordHash();
			QString mail = userInfoPtr->GetMail();
			QByteArray roles = userInfoPtr->GetRoles().join(';');
			QByteArray groups = userInfoPtr->GetGroups().join(';');

			dataModel->SetData("Id", objectUuid);
			dataModel->SetData("Username", username);
			dataModel->SetData("Name", name);
			dataModel->SetData("Password", passwordHash);
			dataModel->SetData("Email", mail);
			dataModel->SetData("Groups", groups);
			dataModel->SetData("Roles", roles);
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CUserControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray &objectId,
		QString &name,
		QString &description,
		QString& errorMessage) const
{
	if (!m_userInfoFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return nullptr;
	}

	objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		imtauth::IUserInfo* userInstancePtr = m_userInfoFactCompPtr.CreateInstance();
		imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(userInstancePtr);
		if (userInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		userInfoPtr->SetObjectUuid(objectId);

		QByteArray username;
		if (itemModel.ContainsKey("Username")){
			username = itemModel.GetData("Username").toByteArray();

			if (username.isEmpty()){
				errorMessage = QT_TR_NOOP("Username can't be empty!");

				return nullptr;
			}
		}

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (imtbase::ICollectionInfo::Id collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				imtauth::IUserInfo* currentUserInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
				if (currentUserInfoPtr != nullptr){
					if (collectionId != objectId){
						QByteArray currentUsername = currentUserInfoPtr->GetId();
						if (currentUsername == username){
							errorMessage = QT_TR_NOOP("Username already exists");
							return nullptr;
						}
					}
				}
			}
		}

		userInfoPtr->SetId(username);

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			userInfoPtr->SetName(name);
		}

		QByteArray password;
		if (itemModel.ContainsKey("Password")){
			password = itemModel.GetData("Password").toByteArray();
		}

		if (password.isEmpty()){
			errorMessage = QT_TR_NOOP("Password cannot be empty");

			return nullptr;
		}

		bool calculate = true;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
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
				password = m_hashCalculatorCompPtr->GenerateHash(username + password);
			}
		}

		userInfoPtr->SetPasswordHash(password);

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

		if (itemModel.ContainsKey("Roles")){
			QByteArray roles = itemModel.GetData("Roles").toByteArray();
			if (!roles.isEmpty()){
				QByteArrayList roleIds = roles.split(';');
				userInfoPtr->SetRoles(roleIds);
			}
		}

		if (itemModel.ContainsKey("Groups")){
			QByteArray groups = itemModel.GetData("Groups").toByteArray();
			if (!groups.isEmpty()){
				QByteArrayList groupIds = groups.split(';');
				for (const QByteArray& groupId : groupIds){
					userInfoPtr->AddToGroup(groupId);
				}
			}
		}

		return userInfoPtr;
	}

	errorMessage = QObject::tr("Can not create user: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtauthgql


