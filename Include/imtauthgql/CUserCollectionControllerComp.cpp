#include <imtauthgql/CUserCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Users::V1_0::CUserCollectionControllerCompBase)

bool CUserCollectionControllerComp::CreateRepresentationFromObject(
	const imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const sdl::imtauth::Users::V1_0::CUsersListGqlRequest& usersListRequest,
	sdl::imtauth::Users::CUserItem::V1_0& representationObject,
	QString& errorMessage) const
{
	imtauth::IUserInfo* contextUserInfoPtr = nullptr;
	const imtgql::IGqlContext* gqlContextPtr = usersListRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		contextUserInfoPtr = gqlContextPtr->GetUserInfo();
	}

	sdl::imtauth::Users::V1_0::UsersListRequestArguments arguments = usersListRequest.GetRequestedArguments();

	QByteArray objectId = objectCollectionIterator.GetObjectId();
	QByteArray productId;
	if (arguments.input.ProductId){
		productId = *arguments.input.ProductId;
	}

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

	sdl::imtauth::Users::V1_0::UsersListRequestInfo requestInfo = usersListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id.reset(new QByteArray(objectId));
	}

	if (requestInfo.items.isUserIdRequested){
		representationObject.UserId.reset(new QByteArray(userInfoPtr->GetId()));
	}

	if (requestInfo.items.isNameRequested){
		representationObject.Name.reset(new QString(userInfoPtr->GetName()));
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description.reset(new QString(userInfoPtr->GetDescription()));
	}

	if (requestInfo.items.isMailRequested){
		representationObject.Mail.reset(new QString(userInfoPtr->GetMail()));
	}

	if (requestInfo.items.isSystemIdRequested){
		QByteArrayList systemIdList;
		imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
		for (imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
			systemIdList << systemInfo.systemId;
		}

		representationObject.SystemId.reset(new QByteArray(systemIdList.join(';')));
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

		representationObject.SystemName.reset(new QString(systemNameList.join(';')));
	}

	if (requestInfo.items.isRolesRequested){
		QByteArrayList resultList;
		if (m_roleInfoProviderCompPtr.IsValid()){
			for (const QByteArray& roleId: userInfoPtr->GetRoles(productId)){
				istd::TDelPtr<const imtauth::IRole> roleInfoPtr = m_roleInfoProviderCompPtr->GetRole(roleId);
				if (roleInfoPtr.IsValid()){
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

		representationObject.Roles.reset(new QByteArray(resultList.join(';')));
	}

	if (requestInfo.items.isGroupsRequested){
		QByteArrayList resultList;
		if (m_userGroupInfoProviderCompPtr.IsValid()){
			for (const QByteArray& groupId: userInfoPtr->GetGroups()){
				istd::TDelPtr<const imtauth::IUserGroupInfo> userGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
				if (userGroupInfoPtr.IsValid()){
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
		representationObject.Groups.reset(new QByteArray(resultList.join(';')));
	}

	if (requestInfo.items.isLastConnectionRequested){
		QDateTime lastConnection = userInfoPtr->GetLastConnection();
		lastConnection.setTimeSpec(Qt::UTC);

		representationObject.LastConnection.reset(new QString(lastConnection.toLocalTime().toString("dd.MM.yyyy hh:mm:ss")));
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added.reset(new QString(added));
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.LastModified.reset(new QString(lastModified));
	}

	return true;
}


istd::IChangeable* CUserCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::imtauth::Users::CUserData::V1_0& userDataRepresentation,
	QByteArray& newObjectId,
	QString& name,
	QString& /*description*/,
	QString& errorMessage) const
{
	if (!m_userInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserFactory' was not set", "CUserCollectionControllerComp");
		return nullptr;
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserCollectionControllerComp");
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

	QByteArray productId;
	if (userDataRepresentation.ProductId){
		productId = *userDataRepresentation.ProductId;
	}

	if (userDataRepresentation.Id){
		newObjectId = *userDataRepresentation.Id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	userInfoPtr->SetObjectUuid(newObjectId);

	imtauth::IUserInfo* oldUserInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldUserdataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(newObjectId, oldUserdataPtr)){
		oldUserInfoPtr = dynamic_cast<imtauth::IUserInfo*>(oldUserdataPtr.GetPtr());
	}

	QByteArray username;
	if (userDataRepresentation.Username){
		username = *userDataRepresentation.Username;
	}
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
				if (userObjectId != newObjectId){
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

	if (userDataRepresentation.Name){
		name = *userDataRepresentation.Name;
	}
	userInfoPtr->SetName(name);

	QList<sdl::imtauth::Users::CSystemInfo::V1_0> systemInfos;
	if (userDataRepresentation.SystemInfos){
		systemInfos = *userDataRepresentation.SystemInfos;
	}
	if (systemInfos.isEmpty()){
		// User from internal system
		imtauth::IUserInfo::SystemInfo systemInfo;
		userInfoPtr->AddToSystem(systemInfo);
	}
	else{
		for (const sdl::imtauth::Users::CSystemInfo::V1_0& sdlSystemInfo : systemInfos){
			QByteArray systemId;
			if (sdlSystemInfo.Id){
				systemId = *sdlSystemInfo.Id;
			}
			QString systemName;
			if (sdlSystemInfo.Name){
				systemName = *sdlSystemInfo.Name;
			}
			bool enabled = false;
			if (sdlSystemInfo.Enabled){
				enabled = *sdlSystemInfo.Enabled;
			}

			imtauth::IUserInfo::SystemInfo systemInfo;
			systemInfo.systemId = systemId;
			systemInfo.systemName = systemName;
			systemInfo.enabled = enabled;

			userInfoPtr->AddToSystem(systemInfo);
		}
	}

	for (imtauth::IUserInfo::SystemInfo& systemInfo : userInfoPtr->GetSystemInfos()){
		if (systemInfo.enabled && systemInfo.systemId.isEmpty()){
			QString password;
			if (userDataRepresentation.Password){
				password = *userDataRepresentation.Password;
			}
			if (password.isEmpty()) {
				errorMessage = QString("Password cannot be empty");

				return nullptr;
			}

			if (oldUserInfoPtr == nullptr){
				password = m_hashCalculatorCompPtr->GenerateHash(username + password.toUtf8());
			}
			else{
				password = oldUserInfoPtr->GetPasswordHash();
			}

			userInfoPtr->SetPasswordHash(password.toUtf8());

			break;
		}
	}

	QString mail;
	if (userDataRepresentation.Email){
		mail = *userDataRepresentation.Email;
	}
	userInfoPtr->SetMail(mail);

	imtauth::IUserInfo::FeatureIds permissions;
	if (userDataRepresentation.Permissions){
		permissions = userDataRepresentation.Permissions->split(';');
	}
	permissions.removeAll("");
	userInfoPtr->SetLocalPermissions(productId, permissions);

	if (oldUserInfoPtr != nullptr){
		for (const QByteArray& oldProductId : oldUserInfoPtr->GetProducts()){
			QByteArrayList roles = oldUserInfoPtr->GetRoles(oldProductId);
			for (const QByteArray& roleId : roles){
				userInfoPtr->AddRole(oldProductId, roleId);
			}
		}
	}

	QByteArrayList roleIds;
	if (userDataRepresentation.Roles){
		roleIds = userDataRepresentation.Roles->split(';');
	}
	roleIds.removeAll("");
	if (!roleIds.isEmpty()){
		userInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userInfoPtr->RemoveProduct(productId);
	}

	QByteArrayList groupIds;
	if (userDataRepresentation.Groups){
		groupIds = userDataRepresentation.Groups->split(';');
	}
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
	const sdl::imtauth::Users::V1_0::CUserItemGqlRequest& userItemRequest,
	sdl::imtauth::Users::CUserDataPayload::V1_0& representationPayload,
	QString& errorMessage) const
{
	const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(&data);
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Users::V1_0::UserItemRequestArguments arguments = userItemRequest.GetRequestedArguments();
	QByteArray productId;
	if (arguments.input.ProductId){
		productId = *arguments.input.ProductId;
	}
	sdl::imtauth::Users::CUserData::V1_0 userData;

	QByteArray objectId = userInfoPtr->GetObjectUuid();
	userData.Id.reset(new QByteArray(objectId));

	QByteArray userName = userInfoPtr->GetId();
	userData.Username.reset(new QByteArray(userName));

	QString name = userInfoPtr->GetName();
	userData.Name.reset(new QString(name));

	QByteArray password = userInfoPtr->GetPasswordHash();
	userData.Password.reset(new QString(password));

	QString mail = userInfoPtr->GetMail();
	userData.Email.reset(new QString(mail));

	QByteArrayList groupList = userInfoPtr->GetGroups();
	std::sort(groupList.begin(), groupList.end());
	userData.Groups.reset(new QByteArray(groupList.join(';')));

	QByteArrayList roleList = userInfoPtr->GetRoles(productId);
	std::sort(roleList.begin(), roleList.end());
	userData.Roles.reset(new QByteArray(roleList.join(';')));

	QByteArray permissions = userInfoPtr->GetPermissions(productId).join(';');
	std::sort(permissions.begin(), permissions.end());
	userData.Permissions.reset(new QByteArray(permissions));

	QList<sdl::imtauth::Users::CSystemInfo::V1_0> list;
	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		sdl::imtauth::Users::CSystemInfo::V1_0 info;

		info.Id.reset(new QByteArray(systemInfo.systemId));

		if (systemInfo.systemId.isEmpty()){
			info.Name.reset(new QString("Internal"));
		}
		else{
			info.Name.reset(new QString(systemInfo.systemName));
		}

		info.Enabled.reset(new bool(systemInfo.enabled));

		list << info;
	}
	userData.SystemInfos = std::make_unique<QList<sdl::imtauth::Users::CSystemInfo::V1_0>>(list);

	representationPayload.UserData = std::make_unique<sdl::imtauth::Users::CUserData::V1_0>(userData);

	return true;
}


imtbase::CTreeItemModel* CUserCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CUserCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to get a meta info. Error: GraphQL params is invalid");
		return nullptr;
	}

	QByteArray userId = gqlInputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QByteArray productId = gqlInputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

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
				if (rolesIds.isEmpty()){
					children->SetData("Value", "No roles");
				}
				else{
					for (const QByteArray& productRoleId: rolesIds){
						istd::TDelPtr<const imtauth::IRole> rolePtr = m_roleInfoProviderCompPtr->GetRole(productRoleId);
						if (rolePtr.IsValid()){
							QByteArray roleId = rolePtr->GetRoleId();
							QString roleName = rolePtr->GetRoleName();
							QByteArray roleProductId = rolePtr->GetProductId();

							int childrenIndex = children->InsertNewItem();
							children->SetData("Value", roleName + " (" + roleProductId + ")", childrenIndex);
						}
					}
				}
			}

			index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Groups", index);

			children = dataModel->AddTreeModel("Children", index);

			if (m_userGroupInfoProviderCompPtr.IsValid()){
				QByteArrayList groupIds = userInfoPtr->GetGroups();
				if (groupIds.isEmpty()){
					children->SetData("Value", "No groups");
				}
				else{
					for (const QByteArray& groupId : groupIds){
						istd::TDelPtr<const imtauth::IUserGroupInfo> userGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
						if (userGroupInfoPtr.IsValid()){
							QString groupName = userGroupInfoPtr->GetName();

							int childrenIndex = children->InsertNewItem();
							children->SetData("Value", groupName, childrenIndex);
						}
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}

bool CUserCollectionControllerComp::UpdateObjectFromRepresentationRequest(const imtgql::CGqlRequest& rawGqlRequest, const sdl::imtauth::Users::V1_0::CUserUpdateGqlRequest& userUpdateRequest, istd::IChangeable& object, QString& errorMessage) const
{
	SendCriticalMessage(0, "Unimplemented method call!", __func__);

	Q_ASSERT_X(false, "Unimplemented method call!", __func__);

	return false;
}


} // namespace imtauth


