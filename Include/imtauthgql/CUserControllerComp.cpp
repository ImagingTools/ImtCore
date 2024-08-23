#include <imtauthgql/CUserControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserControllerComp::GetObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get an user object. Error: Attribute 'm_objectCollectionCompPtr' was not set.").toUtf8();
		SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to get an user object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray userId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QByteArray productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to get an user with ID: '%1'. The user does not exist.").arg(qPrintable(userId));
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	dataModelPtr->SetData("Id", userId);
	dataModelPtr->SetData("Username", userInfoPtr->GetId());
	dataModelPtr->SetData("Name", userInfoPtr->GetName());
	dataModelPtr->SetData("Password", userInfoPtr->GetPasswordHash());
	dataModelPtr->SetData("Email", userInfoPtr->GetMail());

	QByteArrayList groupList = userInfoPtr->GetGroups();
	std::sort(groupList.begin(), groupList.end());
	dataModelPtr->SetData("Groups", groupList.join(';'));

	QByteArrayList roleList = userInfoPtr->GetRoles(productId);
	std::sort(roleList.begin(), roleList.end());
	dataModelPtr->SetData("Roles", roleList.join(';'));

	QByteArray permissions = userInfoPtr->GetPermissions(productId).join(';');
	std::sort(permissions.begin(), permissions.end());
	dataModelPtr->SetData("Permissions", permissions);

	imtbase::CTreeItemModel* systemInfosModelPtr = dataModelPtr->AddTreeModel("SystemInfos");
	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		int index = systemInfosModelPtr->InsertNewItem();

		systemInfosModelPtr->SetData("Id", systemInfo.systemId, index);

		if (systemInfo.systemName.isEmpty()){
			systemInfosModelPtr->SetData("Name", QT_TR_NOOP("Internal"), index);
		}
		else{
			systemInfosModelPtr->SetData("Name", systemInfo.systemName, index);
		}

		systemInfosModelPtr->SetData("Enabled", systemInfo.enabled, index);
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

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update an user object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return nullptr;
	}

	QByteArray representationData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	QByteArray productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to update user with ID: '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	imtbase::CTreeItemModel representationModel;
	if (!representationModel.CreateFromJson(representationData)){
		errorMessage = QString("Error when try to create a representation model from json '%1'").arg(qPrintable(representationData));
		SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	QByteArray username;
	if (representationModel.ContainsKey("Username")){
		username = representationModel.GetData("Username").toByteArray();

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
				errorMessage = QString("Unable to set username to user. Error: username '%1' already exists").arg(qPrintable(username));
				SendWarningMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

				return nullptr;
			}
		}

		userInfoPtr->SetId(username);
	}

	if (representationModel.ContainsKey("Name")){
		QString name = representationModel.GetData("Name").toString();

		userInfoPtr->SetName(name);
	}

	QByteArray password;
	if (representationModel.ContainsKey("Password")){
		password = representationModel.GetData("Password").toByteArray();
	}

	bool calculate = true;
	if (userInfoPtr != nullptr){
		if (userInfoPtr->GetPasswordHash() == password){
			calculate = false;
		}
	}

	if (calculate && m_hashCalculatorCompPtr.IsValid()){
		password = m_hashCalculatorCompPtr->GenerateHash(username + password);
	}

	userInfoPtr->SetPasswordHash(password);

	if (representationModel.ContainsKey("Email")){
		QByteArray email = representationModel.GetData("Email").toByteArray();

		userInfoPtr->SetMail(email);
	}

	if (representationModel.ContainsKey("Roles")){
		for (const QByteArray& userProductId : userInfoPtr->GetProducts()){
			if (userProductId == productId){
				userInfoPtr->RemoveProduct(userProductId);
			}
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
		QByteArrayList groupIds = groups.split(';');
		groupIds.removeAll("");

		for (const QByteArray& groupId : groupIds){
			userInfoPtr->AddToGroup(groupId);
		}
	}

	if (representationModel.ContainsKey("SystemInfos")){
		imtbase::CTreeItemModel* systemInfosModelPtr = representationModel.GetTreeItemModel("SystemInfos");
		if (systemInfosModelPtr != nullptr){
			for (imtauth::IUserInfo::SystemInfo& systemInfo : userInfoPtr->GetSystemInfos()){
				userInfoPtr->RemoveFromSystem(systemInfo.systemId);
			}

			for (int i = 0; i < systemInfosModelPtr->GetItemsCount(); i++){
				QByteArray systemId = systemInfosModelPtr->GetData("Id", i).toByteArray();
				QString systemName = systemInfosModelPtr->GetData("Name", i).toString();
				bool enabled = systemInfosModelPtr->GetData("Enabled", i).toBool();

				imtauth::IUserInfo::SystemInfo systemInfo;
				systemInfo.systemId = systemId;
				systemInfo.systemName = systemName;
				systemInfo.enabled = enabled;

				userInfoPtr->AddToSystem(systemInfo);
			}
		}
	}

	if (!m_objectCollectionCompPtr->SetObjectData(objectId, *userInfoPtr)){
		errorMessage = QString("Unable to save user with ID: '%1'").arg(qPrintable(objectId));
		SendWarningMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtrPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtrPtr->AddTreeModel("updatedNotification");

	notificationModelPtr->SetData("Id", objectId);
	notificationModelPtr->SetData("Name", userInfoPtr->GetName());

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CUserControllerComp::CreateObjectFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		QByteArray& objectId,
		QString& name,
		QString&/*description*/,
		QString& errorMessage) const
{
	if (!m_userInfoFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get an user object. Error: Attribute 'm_objectCollectionCompPtr' was not set.").toUtf8();
		SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to create an user object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return nullptr;
	}

	objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();

	istd::TDelPtr<imtauth::CIdentifiableUserInfo> userInfoPtr;
	userInfoPtr.SetCastedOrRemove(m_userInfoFactCompPtr.CreateInstance());
	if (!userInfoPtr.IsValid()){
		errorMessage = QString("Unable to create an user instance");
		SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	QByteArray itemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	imtbase::CTreeItemModel itemModel;
	if (!itemModel.CreateFromJson(itemData)){
		errorMessage = QString("Unable to create an user object. Error: Failed to create a tree model from json '%1'").arg(qPrintable(itemData));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return nullptr;
	}

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

	if (m_hashCalculatorCompPtr.IsValid()){
		password = m_hashCalculatorCompPtr->GenerateHash(username + password);
	}

	// User from internal system
	imtauth::IUserInfo::SystemInfo systemInfo;
	userInfoPtr->AddToSystem(systemInfo);

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

			permissions.removeAll("");

			userInfoPtr->SetLocalPermissions(productId, permissions);
		}
	}

	if (itemModel.ContainsKey("Roles")){
		QByteArray roles = itemModel.GetData("Roles").toByteArray();
		if (!roles.isEmpty()){
			QByteArrayList roleIds = roles.split(';');
			roleIds.removeAll("");

			userInfoPtr->SetRoles(productId, roleIds);
		}
		else{
			userInfoPtr->RemoveProduct(productId);
		}
	}

	if (itemModel.ContainsKey("Groups")){
		QByteArray groups = itemModel.GetData("Groups").toByteArray();
		QByteArrayList groupIds = groups.split(';');
		for (const QByteArray& groupId : groupIds){
			if (!groupId.isEmpty()){
				userInfoPtr->AddToGroup(groupId);
			}
		}
	}

	return userInfoPtr.PopPtr();
}


imtbase::CTreeItemModel* CUserControllerComp::DeleteObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("No collection component was set");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to delete an user. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		errorMessage = QString("No object-ID could not be extracted from the request");
		SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to delete an user with ID: '%1'. User does not exist.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	QByteArray removedUserId = userInfoPtr->GetId();

	if (!removedUserId.isEmpty()){
		imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		if (contextPtr != nullptr){
			const imtauth::IUserInfo* contextUserInfoPtr = contextPtr->GetUserInfo();
			if (contextUserInfoPtr != nullptr){
				QByteArray userId = contextUserInfoPtr->GetId();
				if (removedUserId == userId){
					errorMessage = QString("It is impossible to delete yourself. User-ID: '%1'.").arg(qPrintable(userId));
					SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

					return nullptr;
				}
			}
		}
	}

	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		if(!systemInfo.systemId.isEmpty()){
			errorMessage = QString("Unable to delete an user with User-ID: '%1'. The user is on a different system.").arg(qPrintable(removedUserId));
			SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

			return nullptr;
		}
	}

	if (!m_objectCollectionCompPtr->RemoveElement(objectId)){
		errorMessage = QString("The user cannot be deleted: '%1'").arg(QString(objectId));
		SendErrorMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("removedNotification");
	notificationModelPtr->SetData("Id", objectId);

	if (m_groupCollectionCompPtr.IsValid()){
		QByteArrayList groupIds = userInfoPtr->GetGroups();

		for (const QByteArray& groupId : groupIds){
			imtbase::IObjectCollection::DataPtr groupDataPtr;
			if (m_groupCollectionCompPtr->GetObjectData(groupId, groupDataPtr)){
				imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
				if (groupInfoPtr != nullptr){
					groupInfoPtr->RemoveUser(objectId);

					if (!m_groupCollectionCompPtr->SetObjectData(groupId, *groupInfoPtr)){
						SendWarningMessage(0, QString("Unable to update a group"));
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


