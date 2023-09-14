#include <imtauthgql/CUserControllerComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtlic/IFeatureInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserControllerComp::GetObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "GetObject m_objectCollectionCompPtr is not valid", "imtauthgql::CUserControllerComp");
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	QByteArray userId;
	QByteArray productId;

	bool isJsonSerialized = false;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
		userId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
		isJsonSerialized = inputParamPtr->GetFieldArgumentValue("IsJsonSerialized").toBool();
	}

	const QList<imtgql::CGqlObject> fieldsPtr = gqlRequest.GetFields();

	QByteArrayList allFields;
	for (const imtgql::CGqlObject& object : fieldsPtr){
		QByteArrayList fields = object.GetFieldIds();

		allFields += fields;
	}

	dataModel->SetData("UserId", "");
	dataModel->SetData("Username", "");
	dataModel->SetData("Name", "");
	dataModel->SetData("Password", "");
	dataModel->SetData("Email", "");

	userId = userId.replace('{', "").replace('}', "");

	bool objectExists = false;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			objectExists = true;

			if (isJsonSerialized){
				QByteArray userJson;
				{
					iser::CJsonMemWriteArchive archive(userJson);
					if (!userInfoPtr->Serialize(archive)){
						SendErrorMessage(0, QString("Error when try to serialize json: %1").arg(qPrintable(userJson)), "imtauthgql::CUserControllerComp");
						errorMessage = QString("Error when try to serialize json: %1").arg(qPrintable(userJson));

						return nullptr;
					}
				}

				if (!dataModel->CreateFromJson(userJson)){
					SendErrorMessage(0, QString("Error when try to create model from json: %1").arg(qPrintable(userJson)), "imtauthgql::CUserControllerComp");
					errorMessage = QString("Error when try to create model from json: %1").arg(qPrintable(userJson));

					return nullptr;
				}
			}
			else{
				QByteArray objectUuid = userId;
				QByteArray username = userInfoPtr->GetId();
				QString name = userInfoPtr->GetName();
				QByteArray passwordHash = userInfoPtr->GetPasswordHash();
				QString mail = userInfoPtr->GetMail();
				QByteArray roles = userInfoPtr->GetRoles(productId).join(';');
				QByteArray groups = userInfoPtr->GetGroups().join(';');

				dataModel->SetData("Id", objectUuid);
				dataModel->SetData("Username", username);
				dataModel->SetData("Name", name);
				dataModel->SetData("Password", passwordHash);
				dataModel->SetData("Email", mail);
				dataModel->SetData("Groups", groups);
				dataModel->SetData("Roles", roles);

				if (allFields.contains("Permissions")){
					QByteArray permissions = userInfoPtr->GetPermissions(productId).join(';');
					dataModel->SetData("Permissions", permissions);
				}
			}
		}
	}

	if (!objectExists && !userId.isEmpty()){
		errorMessage = QObject::tr("Object with ID %1 does not exist").arg(qPrintable(userId));

		imtbase::CTreeItemModel* errorsModelPtr = rootModelPtr->AddTreeModel("errors");
		errorsModelPtr->SetData("message", errorMessage);

		rootModelPtr->SetExternTreeModel("errors", errorsModelPtr);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CUserControllerComp::UpdateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "m_objectCollectionCompPtr is not valid", "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	QByteArray representationData;
	QByteArray productId;
	QByteArray objectId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		representationData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
		productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		SendErrorMessage(0, "userInfoPtr is nullptr", "imtauthgql::CUserControllerComp");
		errorMessage = QString("Unable to update user with ID: %1").arg(qPrintable(objectId));

		return nullptr;
	}

	imtbase::CTreeItemModel representationModel;
	if (!representationModel.CreateFromJson(representationData)){
		SendErrorMessage(0, QString("Error when try to create a representation model from json: %1").arg(qPrintable(representationData)), "imtauthgql::CUserControllerComp");
		errorMessage = QString("Error when try to create a representation model from json% 1").arg(qPrintable(representationData));

		return nullptr;
	}

	if (representationModel.ContainsKey("Username")){
		QByteArray username = representationModel.GetData("Username").toByteArray();

		iprm::CParamsSet filterParam;
		iprm::CParamsSet paramsSet;

		iprm::CTextParam userId;
		userId.SetText(username);

		paramsSet.SetEditableParameter("Id", &userId);
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

		imtbase::IObjectCollection::Ids userIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!userIds.isEmpty()){
			QByteArray userObjectId = userIds[0];
			if (userObjectId != objectId){
				SendWarningMessage(0, QString("Unable to update user username %1 already exists.").arg(qPrintable(username)), "imtauthgql::CUserControllerComp");
				errorMessage = QString("Username already exists").arg(qPrintable(representationData));

				return nullptr;
			}
		}

		userInfoPtr->SetId(username);
	}

	if (representationModel.ContainsKey("Name")){
		QString name = representationModel.GetData("Name").toString();

		userInfoPtr->SetName(name);
	}

	if (representationModel.ContainsKey("Password")){
		QByteArray password = representationModel.GetData("Password").toByteArray();

//		if (m_hashCalculatorCompPtr.IsValid()){
//			password = m_hashCalculatorCompPtr->GenerateHash(userInfoPtr->GetId() + password);
//		}

		userInfoPtr->SetPasswordHash(password);
	}

	if (representationModel.ContainsKey("Email")){
		QByteArray email = representationModel.GetData("Email").toByteArray();

		userInfoPtr->SetMail(email);
	}

	if (representationModel.ContainsKey("Roles")){
		for (const QByteArray& userProductId : userInfoPtr->GetProducts()){
			userInfoPtr->RemoveProduct(userProductId);
		}

		QByteArray roles = representationModel.GetData("Roles").toByteArray();
		if (!roles.isEmpty()){
			QByteArrayList roleIds = roles.split(';');
			userInfoPtr->SetRoles(productId, roleIds);
		}
	}

	if (representationModel.ContainsKey("Groups")){
		for (const QByteArray& groupId : userInfoPtr->GetGroups()){
			userInfoPtr->RemoveFromGroup(groupId);
		}

		QByteArray groups = representationModel.GetData("Groups").toByteArray();
		if (!groups.isEmpty()){
			QByteArrayList groupIds = groups.split(';');
			for (const QByteArray& groupId : groupIds){
				userInfoPtr->AddToGroup(groupId);
			}
		}
	}

	if (!m_objectCollectionCompPtr->SetObjectData(objectId, *userInfoPtr)){
		SendWarningMessage(0, QString("Unable to save user with ID: %1").arg(qPrintable(objectId)), "imtauthgql::CUserControllerComp");
		errorMessage = QString("Unable to save user with ID: %1").arg(qPrintable(objectId));

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("updatedNotification");

	notificationModelPtr->SetData("Id", objectId);
	notificationModelPtr->SetData("Name", userInfoPtr->GetName());

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CUserControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString&/*description*/,
			QString& errorMessage) const
{
	if (!m_userInfoFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		SendErrorMessage(0, QString("m_userInfoFactCompPtr or m_objectCollectionCompPtr is not valid"), "imtauthgql::CUserControllerComp");
		errorMessage = QString("Internal error");

		return nullptr;
	}

	objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray productId;
	if (!inputParams.empty()){
		productId = inputParams.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		imtauth::IUserInfo* userInstancePtr = m_userInfoFactCompPtr.CreateInstance();
		imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(userInstancePtr);
		if (userInfoPtr == nullptr){
			SendErrorMessage(0, QString("Unable to get an account info"), "imtauthgql::CUserControllerComp");
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
				SendWarningMessage(0, QString("Username can't be empty"), "imtauthgql::CUserControllerComp");
				errorMessage = QT_TR_NOOP("Username can't be empty!");

				return nullptr;
			}
		}

		iprm::CParamsSet filterParam;
		iprm::CParamsSet paramsSet;

		iprm::CTextParam userId;
		userId.SetText(username);

		paramsSet.SetEditableParameter("Id", &userId);
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

		imtbase::IObjectCollection::Ids userIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!userIds.isEmpty()){
			QByteArray userObjectId = userIds[0];
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
				const imtauth::CUserInfo* currentUserInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
				if (currentUserInfoPtr != nullptr){
					if (userObjectId != objectId){
						QByteArray currentUsername = currentUserInfoPtr->GetId();
						if (currentUsername == username){
							SendWarningMessage(0, QString("Username already exists"), "imtauthgql::CUserControllerComp");
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
		const imtauth::IUserInfo* currentuserInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			currentuserInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (currentuserInfoPtr != nullptr){
				QByteArray currentPasswordHash = currentuserInfoPtr->GetPasswordHash();
				if (currentPasswordHash == password){
					calculate = false;
				}

				for (const QByteArray& userProductId : currentuserInfoPtr->GetProducts()){
					userInfoPtr->SetRoles(userProductId, currentuserInfoPtr->GetRoles(userProductId));
				}
			}
		}

//		if (calculate){
//			if (m_hashCalculatorCompPtr.IsValid()){
//				password = m_hashCalculatorCompPtr->GenerateHash(username + password);
//			}
//		}

		userInfoPtr->SetPasswordHash(password);

		if (itemModel.ContainsKey("Email")){
			QString mail = itemModel.GetData("Email").toString();
			userInfoPtr->SetMail(mail);
		}

		if (itemModel.ContainsKey("Permissions")){
			imtbase::CTreeItemModel* permissionsModel = itemModel.GetTreeItemModel("Permissions");
			if (permissionsModel != nullptr){
				imtauth::IUserInfo::FeatureIds permissions;

				for(int index = 0; index < permissionsModel->GetItemsCount(); index++){
					QByteArray featureId;
					if (permissionsModel->ContainsKey("PermissionId", index)){
						featureId = permissionsModel->GetData("PermissionId", index).toByteArray();
						if (!featureId.isEmpty()){
							permissions << featureId;
						}
					}
				}

				userInfoPtr->SetLocalPermissions(productId, permissions);
			}
		}

		if (itemModel.ContainsKey("Roles")){
			QByteArray roles = itemModel.GetData("Roles").toByteArray();
			if (!roles.isEmpty()){
				QByteArrayList roleIds = roles.split(';');
				userInfoPtr->SetRoles(productId, roleIds);
			}
			else{
				userInfoPtr->RemoveProduct(productId);
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

	SendErrorMessage(0, QString("Can not create user: %1").arg(QString(objectId)), "imtauthgql::CUserControllerComp");
	errorMessage = QObject::tr("Can not create user: %1").arg(QString(objectId));

	return nullptr;
}


imtbase::CTreeItemModel* CUserControllerComp::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = QObject::tr("No object-ID could not be extracted from the request");

		return nullptr;
	}

	QByteArray removedUserId;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			removedUserId = userInfoPtr->GetId();
		}
	}

	if (!removedUserId.isEmpty()){
		imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		if (contextPtr != nullptr){
			const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
			if (userInfoPtr != nullptr){
				QByteArray userId = userInfoPtr->GetId();
				if (removedUserId == userId){
					errorMessage = QObject::tr("It is not possible to delete a user");

					return nullptr;
				}
			}
		}
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	if (m_objectCollectionCompPtr->RemoveElement(objectId)){
		imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* notificationModel = new imtbase::CTreeItemModel();

		notificationModel->SetData("Id", objectId);
		dataModel->SetExternTreeModel("removedNotification", notificationModel);

		rootModelPtr->SetExternTreeModel("data", dataModel);
	}
	else{
		errorMessage = QObject::tr("Can't remove object: %1").arg(QString(objectId));
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModelPtr->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


