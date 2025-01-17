#include <imtauthgql/CUserCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


// protected methods

bool CUserCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::imtauth::Users::CUserData::V1_0& representation,
			istd::IChangeable& object,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(&object);
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	QByteArray productId;
	if (representation.ProductId){
		productId = *representation.ProductId;
	}

	QByteArray username;
	if (representation.Username){
		username = *representation.Username;
	}

	if (username.isEmpty()){
		errorMessage = QT_TR_NOOP("Username can't be empty!");
		SendWarningMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
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
						errorMessage = QT_TR_NOOP("Username already exists");
						SendWarningMessage(0, errorMessage, "imtauthgql::CUserControllerComp");

						return false;
					}
				}
			}
		}
	}

	userInfoPtr->SetId(username);

	QString name;
	if (representation.Name){
		name = *representation.Name;
	}

	if (name.isEmpty()){
		errorMessage = QString("Name cannot be empty");
		return false;
	}

	userInfoPtr->SetName(name);

	QList<sdl::imtauth::Users::CSystemInfo::V1_0> systemInfos;
	if (representation.SystemInfos){
		systemInfos = *representation.SystemInfos;
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

	QString mail;
	if (representation.Email){
		mail = *representation.Email;
	}

	if (mail.isEmpty()){
		errorMessage = QString("Email cannot be empty");
		return false;
	}

	iprm::CParamsSet mailFilterParam;
	iprm::CParamsSet mailParamsSet;

	iprm::CTextParam mailParam;
	mailParam.SetText(mail);

	mailParamsSet.SetEditableParameter("Mail", &mailParam);
	mailFilterParam.SetEditableParameter("ObjectFilter", &mailParamsSet);

	userIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &mailFilterParam);
	if (!userIds.isEmpty()){
		QByteArray userObjectId = userIds[0];
		if (newObjectId != userObjectId){
			errorMessage = QT_TR_NOOP("Email already exists");
			return false;
		}
	}

	userInfoPtr->SetMail(mail);

	imtauth::IUserInfo::FeatureIds permissions;
	if (representation.Permissions){
		permissions = representation.Permissions->split(';');
	}
	permissions.removeAll("");
	userInfoPtr->SetLocalPermissions(productId, permissions);

	QByteArrayList roleIds;
	if (representation.Roles){
		roleIds = representation.Roles->split(';');
	}
	roleIds.removeAll("");
	if (!roleIds.isEmpty()){
		userInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userInfoPtr->RemoveProduct(productId);
	}

	QByteArrayList groupIds;
	if (representation.Groups){
		groupIds = representation.Groups->split(';');
	}
	groupIds.removeAll("");
	for (const QByteArray& groupId : groupIds){
		if (!groupId.isEmpty()){
			userInfoPtr->AddToGroup(groupId);
		}
	}

	return true;
}


// reimplemented (sdl::imtbase::ImtCollection::V1_0::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus::V1_0 CUserCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::V1_0::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CVisualStatus::V1_0 response = BaseClass::OnGetObjectVisualStatus(getObjectVisualStatusRequest, gqlRequest, errorMessage);

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = getObjectVisualStatusRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	if (response.Text->isEmpty()){
		response.Text = "<no name>";
	}

	QString translation = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QString(QT_TR_NOOP("Users")).toUtf8(), languageId, "CRoleCollectionControllerComp");
	response.Text = translation + QByteArrayLiteral(" / ") + *response.Text;
	return response;
}


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
		representationObject.Id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.TypeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isUserIdRequested){
		representationObject.UserId = QByteArray(userInfoPtr->GetId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.Name = QString(userInfoPtr->GetName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = QString(userInfoPtr->GetDescription());
	}

	if (requestInfo.items.isMailRequested){
		representationObject.Mail = QString(userInfoPtr->GetMail());
	}

	if (requestInfo.items.isSystemIdRequested){
		QByteArrayList systemIdList;
		imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
		for (imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
			systemIdList << systemInfo.systemId;
		}

		representationObject.SystemId = QByteArray(systemIdList.join(';'));
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

		representationObject.SystemName = QString(systemNameList.join(';'));
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

		representationObject.Roles = QByteArray(resultList.join(';'));
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
		representationObject.Groups = QByteArray(resultList.join(';'));
	}

	if (requestInfo.items.isLastConnectionRequested){
		QDateTime lastConnection = userInfoPtr->GetLastConnection();
		lastConnection.setTimeSpec(Qt::UTC);

		representationObject.LastConnection = QString(lastConnection.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = QString(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.LastModified = QString(lastModified);
	}

	return true;
}


istd::IChangeable* CUserCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::imtauth::Users::CUserData::V1_0& userDataRepresentation,
	QByteArray& newObjectId,
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

	if (userDataRepresentation.Id){
		newObjectId = *userDataRepresentation.Id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	userInfoPtr->SetObjectUuid(newObjectId);

	if (!FillObjectFromRepresentation(userDataRepresentation, *userInfoPtr, newObjectId, errorMessage)){
		return nullptr;
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

			password = m_hashCalculatorCompPtr->GenerateHash(*userDataRepresentation.Username + password.toUtf8());

			userInfoPtr->SetPasswordHash(password.toUtf8());

			break;
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
	userData.Id = QByteArray(objectId);

	userData.ProductId = QByteArray(productId);

	QByteArray userName = userInfoPtr->GetId();
	userData.Username = QByteArray(userName);

	QString name = userInfoPtr->GetName();
	userData.Name = QString(name);

	QByteArray password = userInfoPtr->GetPasswordHash();
	userData.Password = QString(password);

	QString mail = userInfoPtr->GetMail();
	userData.Email = QString(mail);

	QByteArrayList groupList = userInfoPtr->GetGroups();
	std::sort(groupList.begin(), groupList.end());
	userData.Groups = QByteArray(groupList.join(';'));

	QByteArrayList roleList = userInfoPtr->GetRoles(productId);
	std::sort(roleList.begin(), roleList.end());
	userData.Roles = QByteArray(roleList.join(';'));

	QByteArray permissions = userInfoPtr->GetPermissions(productId).join(';');
	std::sort(permissions.begin(), permissions.end());
	userData.Permissions = QByteArray(permissions);

	QList<sdl::imtauth::Users::CSystemInfo::V1_0> list;
	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		sdl::imtauth::Users::CSystemInfo::V1_0 info;

		info.Id = QByteArray(systemInfo.systemId);

		if (systemInfo.systemId.isEmpty()){
			info.Name = QString("Internal");
		}
		else{
			info.Name = QString(systemInfo.systemName);
		}

		info.Enabled = bool(systemInfo.enabled);

		list << info;
	}
	userData.SystemInfos = std::make_optional<QList<sdl::imtauth::Users::CSystemInfo::V1_0>>(list);

	representationPayload.UserData = std::make_optional<sdl::imtauth::Users::CUserData::V1_0>(userData);

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


bool CUserCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::imtauth::Users::V1_0::CUserUpdateGqlRequest& userUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CUserData::V1_0 userData = *userUpdateRequest.GetRequestedArguments().input.Item;

	imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(&object);
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to cast user instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	QByteArray objectId = userInfoPtr->GetObjectUuid();

	QByteArrayList groupIds = userInfoPtr->GetGroups();
	for (const QByteArray& groupId : groupIds){
		if (!groupId.isEmpty()){
			userInfoPtr->RemoveFromGroup(groupId);
		}
	}

	imtauth::IUserInfo::SystemInfoList systemList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemList){
		userInfoPtr->RemoveFromSystem(systemInfo.systemId);
	}

	return FillObjectFromRepresentation(userData, object, objectId, errorMessage);
}


} // namespace imtauth


