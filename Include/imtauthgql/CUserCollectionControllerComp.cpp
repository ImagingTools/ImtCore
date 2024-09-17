#include <imtauthgql/CUserCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Users::CUserCollectionControllerCompBase)

bool CUserCollectionControllerComp::CreateRepresentationFromObject(
	const imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const sdl::imtauth::Users::CUsersListGqlRequest& usersListRequest,
	sdl::imtauth::Users::CUserItem& representationObject,
	QString& errorMessage) const
{
	imtauth::IUserInfo* contextUserInfoPtr = nullptr;
	const imtgql::IGqlContext* gqlContextPtr = usersListRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		contextUserInfoPtr = gqlContextPtr->GetUserInfo();
	}

	sdl::imtauth::Users::UsersListRequestArguments arguments = usersListRequest.GetRequestedArguments();

	QByteArray objectId = objectCollectionIterator.GetObjectId();
	QByteArray productId = arguments.input.GetProductId();

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr userDataPtr;
	if (objectCollectionIterator.GetObjectData(userDataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	bool ok = false;

	if (contextUserInfoPtr != nullptr){
		if (!contextUserInfoPtr->IsAdmin() && userInfoPtr->IsAdmin()){
			ok = true;
		}
	}

	if (ok){
		return true;
	}

	sdl::imtauth::Users::UsersListRequestInfo requestInfo = usersListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isUserIdRequested){
		representationObject.SetUserId(userInfoPtr->GetId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.SetName(userInfoPtr->GetName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.SetDescription(userInfoPtr->GetDescription());
	}

	if (requestInfo.items.isMailRequested){
		representationObject.SetMail(userInfoPtr->GetMail());
	}

	if (requestInfo.items.isSystemIdRequested){
		QByteArrayList systemIdList;
		imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
		for (imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
			systemIdList << systemInfo.systemId;
		}

		representationObject.SetSystemId(systemIdList.join(';'));
	}

	if (requestInfo.items.isSystemNameRequested){
		QStringList systemNameList;
		imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
		for (imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
			if (systemInfo.systemName.isEmpty()){
				systemNameList << QT_TR_NOOP("Internal");
			}
			else{
				systemNameList << systemInfo.systemName;
			}
		}

		representationObject.SetSystemName(systemNameList.join(';'));
	}

	if (requestInfo.items.isRolesRequested){
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

		representationObject.SetRoles(resultList.join(';'));
	}

	if (requestInfo.items.isGroupsRequested){
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
		representationObject.SetGroups(resultList.join(';'));
	}

	if (requestInfo.items.isLastConnectionRequested){
		QDateTime lastConnection = userInfoPtr->GetLastConnection();
		lastConnection.setTimeSpec(Qt::UTC);

		representationObject.SetLastConnection(lastConnection.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetAdded(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetLastModified(lastModified);
	}

	return true;
}


istd::IChangeable* CUserCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::imtauth::Users::CUserData& userDataRepresentation,
	QByteArray& newObjectId,
	QString& name,
	QString& description,
	QString& errorMessage) const
{
	if (!m_userInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_userInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtauth::IUserInfo> userInstancePtr = m_userInfoFactCompPtr.CreateInstance();
	if (!userInstancePtr.IsValid()){
		errorMessage = QString("Unable to create user instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return nullptr;
	}

	imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(userInstancePtr.GetPtr());
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to cast user instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return nullptr;
	}

	QByteArray productId = userDataRepresentation.GetProductId();

	QByteArray id = userDataRepresentation.GetId();
	if (id.isEmpty()){
		id = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	newObjectId = id;

	imtauth::IUserInfo* oldUserInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldUserdataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(id, oldUserdataPtr)){
		oldUserInfoPtr = dynamic_cast<imtauth::IUserInfo*>(oldUserdataPtr.GetPtr());
	}

	QByteArray username = userDataRepresentation.GetUsername();
	if (username.isEmpty()){
		SendWarningMessage(0, QString("Username can't be empty"), "CUserCollectionControllerComp");
		errorMessage = QT_TR_NOOP("Username can't be empty!");

		return nullptr;
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
				if (userObjectId != id){
					QByteArray currentUsername = currentUserInfoPtr->GetId();
					if (currentUsername.toLower() == username.toLower()){
						SendWarningMessage(0, QString("Username already exists"), "imtauthgql::CUserControllerComp");
						errorMessage = QT_TR_NOOP("Username already exists");

						return nullptr;
					}
				}
			}
		}
	}

	userInfoPtr->SetId(username);

	name = userDataRepresentation.GetName();
	userInfoPtr->SetName(name);

	QList<sdl::imtauth::Users::CSystemInfo> systemInfos = userDataRepresentation.GetSystemInfos();
	if (systemInfos.isEmpty()){
		// User from internal system
		imtauth::IUserInfo::SystemInfo systemInfo;
		userInfoPtr->AddToSystem(systemInfo);
	}
	else{
		for (const sdl::imtauth::Users::CSystemInfo& sdlSystemInfo : systemInfos){
			QByteArray systemId = sdlSystemInfo.GetId();
			QString systemName = sdlSystemInfo.GetName();
			bool enabled = sdlSystemInfo.GetEnabled();

			imtauth::IUserInfo::SystemInfo systemInfo;
			systemInfo.systemId = systemId;
			systemInfo.systemName = systemName;
			systemInfo.enabled = enabled;

			userInfoPtr->AddToSystem(systemInfo);
		}
	}

	for (imtauth::IUserInfo::SystemInfo& systemInfo : userInfoPtr->GetSystemInfos()){
		if (systemInfo.enabled && systemInfo.systemId.isEmpty()){
			QString password = userDataRepresentation.GetPassword();
			if (password.isEmpty()){
				errorMessage = QString("Password cannot be empty");

				return nullptr;
			}

			if (m_hashCalculatorCompPtr.IsValid()){
				if (oldUserInfoPtr == nullptr || (oldUserInfoPtr != nullptr && oldUserInfoPtr->GetPasswordHash() != password)){
					password = m_hashCalculatorCompPtr->GenerateHash(username + password.toUtf8());
				}
			}

			userInfoPtr->SetPasswordHash(password.toUtf8());

			break;
		}
	}

	QString mail = userDataRepresentation.GetEmail();
	userInfoPtr->SetMail(mail);

	imtauth::IUserInfo::FeatureIds permissions = userDataRepresentation.GetPermissions().split(';');
	permissions.removeAll("");
	userInfoPtr->SetLocalPermissions(productId, permissions);

	QByteArrayList roleIds = userDataRepresentation.GetRoles().split(';');
	if (!roleIds.isEmpty()){
		roleIds.removeAll("");
		userInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userInfoPtr->RemoveProduct(productId);
	}

	QByteArrayList groupIds = userDataRepresentation.GetGroups().split(';');
	groupIds.removeAll("");
	for (const QByteArray& groupId : groupIds){
		if (!groupId.isEmpty()){
			userInfoPtr->AddToGroup(groupId);
		}
	}

	return userInstancePtr.PopPtr();
}


bool CUserCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::imtauth::Users::CUserItemGqlRequest& userItemRequest,
	sdl::imtauth::Users::CUserDataPayload& representationPayload,
	QString& errorMessage) const
{
	const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(&data);
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Users::UserItemRequestArguments arguments = userItemRequest.GetRequestedArguments();
	QByteArray productId = arguments.input.GetProductId();
	sdl::imtauth::Users::CUserData userData;

	QByteArray objectId = userInfoPtr->GetObjectUuid();
	userData.SetId(objectId);

	QByteArray userName = userInfoPtr->GetId();
	userData.SetUsername(userName);

	QString name = userInfoPtr->GetName();
	userData.SetName(name);

	QByteArray password = userInfoPtr->GetPasswordHash();
	userData.SetPassword(password);

	QString mail = userInfoPtr->GetMail();
	userData.SetEmail(mail);

	QByteArrayList groupList = userInfoPtr->GetGroups();
	std::sort(groupList.begin(), groupList.end());
	userData.SetGroups(groupList.join(';'));

	QByteArrayList roleList = userInfoPtr->GetRoles(productId);
	std::sort(roleList.begin(), roleList.end());
	userData.SetRoles(roleList.join(';'));

	QByteArray permissions = userInfoPtr->GetPermissions(productId).join(';');
	std::sort(permissions.begin(), permissions.end());
	userData.SetPermissions(permissions);

	QList<sdl::imtauth::Users::CSystemInfo> list;
	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		sdl::imtauth::Users::CSystemInfo info;

		info.SetId(systemInfo.systemId);

		if (systemInfo.systemId.isEmpty()){
			info.SetName(QString("Internal"));
		}
		else{
			info.SetName(systemInfo.systemName);
		}

		info.SetEnabled(systemInfo.enabled);

		list << info;
	}
	userData.SetSystemInfos(list);

	representationPayload.SetUserData(userData);

	return true;
}


imtbase::CTreeItemModel* CUserCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const imtgql::CGqlObject paramsPtr = gqlRequest.GetParams();

	QByteArray productId = paramsPtr.GetFieldArgumentValue("ProductId").toByteArray();

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
						QByteArray roleProductId = rolePtr->GetProductId();

						int childrenIndex = children->InsertNewItem();
						children->SetData("Value", roleName + " (" + roleProductId + ")", childrenIndex);
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


} // namespace imtauth


