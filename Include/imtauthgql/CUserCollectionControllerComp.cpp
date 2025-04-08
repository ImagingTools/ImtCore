#include <imtauthgql/CUserCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
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
	if (representation.productId){
		productId = *representation.productId;
	}

	QByteArray username;
	if (representation.username){
		username = *representation.username;
	}

	if (username.isEmpty()){
		errorMessage = QT_TR_NOOP("Username can't be empty!");
		SendWarningMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "UserId";
	fieldFilter.filterValue = username;
	
	imtbase::IComplexCollectionFilter::GroupFilter groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFieldsFilter(groupFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

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
	if (representation.name){
		name = *representation.name;
	}

	if (name.isEmpty()){
		errorMessage = QString("Name cannot be empty");
		return false;
	}

	userInfoPtr->SetName(name);

	QList<sdl::imtauth::Users::CSystemInfo::V1_0> systemInfos;
	if (representation.systemInfos){
		systemInfos = *representation.systemInfos;
	}
	if (systemInfos.isEmpty()){
		// User from internal system
		imtauth::IUserInfo::SystemInfo systemInfo;
		userInfoPtr->AddToSystem(systemInfo);
	}
	else{
		for (const sdl::imtauth::Users::CSystemInfo::V1_0& sdlSystemInfo : systemInfos){
			QByteArray systemId;
			if (sdlSystemInfo.id){
				systemId = *sdlSystemInfo.id;
			}
			QString systemName;
			if (sdlSystemInfo.name){
				systemName = *sdlSystemInfo.name;
			}
			bool enabled = false;
			if (sdlSystemInfo.enabled){
				enabled = *sdlSystemInfo.enabled;
			}

			imtauth::IUserInfo::SystemInfo systemInfo;
			systemInfo.systemId = systemId;
			systemInfo.systemName = systemName;
			systemInfo.enabled = enabled;

			userInfoPtr->AddToSystem(systemInfo);
		}
	}

	QString mail;
	if (representation.email){
		mail = *representation.email;
	}

	if (!userInfoPtr->IsAdmin()){
		if (mail.isEmpty()){
			errorMessage = QString("Email cannot be empty");
			return false;
		}
		
		imtbase::IComplexCollectionFilter::FieldFilter mailFieldFilter;
		mailFieldFilter.fieldId = "Email";
		mailFieldFilter.filterValue = mail;
		
		imtbase::IComplexCollectionFilter::GroupFilter mailGroupFilter;
		mailGroupFilter.fieldFilters << mailFieldFilter;
		
		imtbase::CComplexCollectionFilter mailComplexFilter;
		mailComplexFilter.SetFieldsFilter(mailGroupFilter);
		
		iprm::CParamsSet mailFilterParam;
		mailFilterParam.SetEditableParameter("ComplexFilter", &mailComplexFilter);

		userIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &mailFilterParam);
		if (!userIds.isEmpty()){
			QByteArray userObjectId = userIds[0];
			if (newObjectId != userObjectId){
				errorMessage = QT_TR_NOOP("Email already exists");
				return false;
			}
		}
	}

	userInfoPtr->SetMail(mail);

	imtauth::IUserInfo::FeatureIds permissions;
	if (representation.permissions){
		permissions = representation.permissions->split(';');
	}
	permissions.removeAll("");
	userInfoPtr->SetLocalPermissions(productId, permissions);

	QByteArrayList roleIds;
	if (representation.roles){
		roleIds = representation.roles->split(';');
	}
	roleIds.removeAll("");
	if (!roleIds.isEmpty()){
		userInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userInfoPtr->RemoveProduct(productId);
	}

	QByteArrayList groupIds;
	if (representation.groups){
		groupIds = representation.groups->split(';');
	}
	groupIds.removeAll("");
	for (const QByteArray& groupId : groupIds){
		if (!groupId.isEmpty()){
			userInfoPtr->AddToGroup(groupId);
		}
	}

	return true;
}


// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus CUserCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CVisualStatus response = BaseClass::OnGetObjectVisualStatus(getObjectVisualStatusRequest, gqlRequest, errorMessage);

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = getObjectVisualStatusRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	if (response.Version_1_0->text->isEmpty()){
		response.Version_1_0->text = "<no name>";
	}

	QString translation = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QString(QT_TR_NOOP("Users")).toUtf8(), languageId, "CRoleCollectionControllerComp");
	response.Version_1_0->text = translation + QByteArrayLiteral(" / ") + *response.Version_1_0->text;
	return response;
}


// reimplemented (sdl::imtauth::Users::CUserCollectionControllerCompBase)

bool CUserCollectionControllerComp::CreateRepresentationFromObject(
	const imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const sdl::imtauth::Users::CUsersListGqlRequest& usersListRequest,
	sdl::imtauth::Users::CUserItem::V1_0& representationObject,
	QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	imtauth::IUserInfo* contextUserInfoPtr = nullptr;
	const imtgql::IGqlContext* gqlContextPtr = usersListRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		contextUserInfoPtr = gqlContextPtr->GetUserInfo();
	}

	if (contextUserInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'. Error: GraphQL context is invalid").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Users::UsersListRequestArguments arguments = usersListRequest.GetRequestedArguments();

	QByteArray productId;
	if (arguments.input.Version_1_0->productId){
		productId = *arguments.input.Version_1_0->productId;
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

	if (!contextUserInfoPtr->IsAdmin() && userInfoPtr->IsAdmin()){
		errorMessage = QString("Unable to create representation from object 'su'. Error: User '%1' permission denied").arg(qPrintable(contextUserInfoPtr->GetId()));
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Users::UsersListRequestInfo requestInfo = usersListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isUserIdRequested){
		representationObject.userId = QByteArray(userInfoPtr->GetId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = QString(userInfoPtr->GetName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = QString(userInfoPtr->GetDescription());
	}

	if (requestInfo.items.isMailRequested){
		representationObject.mail = QString(userInfoPtr->GetMail());
	}

	if (requestInfo.items.isSystemIdRequested){
		QByteArrayList systemIdList;
		imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
		for (imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
			systemIdList << systemInfo.systemId;
		}

		representationObject.systemId = QByteArray(systemIdList.join(';'));
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

		representationObject.systemName = QString(systemNameList.join(';'));
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

		representationObject.roles = QByteArray(resultList.join(';'));
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
		representationObject.groups = QByteArray(resultList.join(';'));
	}

	if (requestInfo.items.isLastConnectionRequested){
		QDateTime lastConnection = userInfoPtr->GetLastConnection();
		lastConnection.setTimeSpec(Qt::UTC);

		representationObject.lastConnection = QString(lastConnection.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = QString(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.lastModified = QString(lastModified);
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

	if (userDataRepresentation.id){
		newObjectId = *userDataRepresentation.id;
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
			if (userDataRepresentation.password){
				password = *userDataRepresentation.password;
			}

			if (password.isEmpty()) {
				errorMessage = QString("Password cannot be empty");
				return nullptr;
			}

			password = m_hashCalculatorCompPtr->GenerateHash(*userDataRepresentation.username + password.toUtf8());

			userInfoPtr->SetPasswordHash(password.toUtf8());

			break;
		}
	}

	return userInstancePtr.PopPtr();
}


bool CUserCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::imtauth::Users::CUserItemGqlRequest& userItemRequest,
	sdl::imtauth::Users::CUserData::V1_0& representationPayload,
	QString& errorMessage) const
{
	const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(&data);
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Users::UserItemRequestArguments arguments = userItemRequest.GetRequestedArguments();
	QByteArray productId;
	if (arguments.input.Version_1_0->productId){
		productId = *arguments.input.Version_1_0->productId;
	}
	
	QByteArray objectId = userInfoPtr->GetObjectUuid();
	representationPayload.id = QByteArray(objectId);

	representationPayload.productId = QByteArray(productId);

	QByteArray userName = userInfoPtr->GetId();
	representationPayload.username = QByteArray(userName);

	QString name = userInfoPtr->GetName();
	representationPayload.name = QString(name);

	QByteArray password = userInfoPtr->GetPasswordHash();
	representationPayload.password = QString(password);

	QString mail = userInfoPtr->GetMail();
	representationPayload.email = QString(mail);

	QByteArrayList groupList = userInfoPtr->GetGroups();
	std::sort(groupList.begin(), groupList.end());
	representationPayload.groups = QByteArray(groupList.join(';'));

	QByteArrayList roleList = userInfoPtr->GetRoles(productId);
	std::sort(roleList.begin(), roleList.end());
	representationPayload.roles = QByteArray(roleList.join(';'));

	QByteArrayList permissions = userInfoPtr->GetPermissions(productId);
	std::sort(permissions.begin(), permissions.end());
	representationPayload.permissions = permissions.join(';');

	QList<sdl::imtauth::Users::CSystemInfo::V1_0> list;
	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		sdl::imtauth::Users::CSystemInfo::V1_0 info;

		info.id = QByteArray(systemInfo.systemId);

		if (systemInfo.systemId.isEmpty()){
			info.name = QString("Internal");
		}
		else{
			info.name = QString(systemInfo.systemName);
		}

		info.enabled = bool(systemInfo.enabled);

		list << info;
	}
	representationPayload.systemInfos = std::make_optional<QList<sdl::imtauth::Users::CSystemInfo::V1_0>>(list);

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

	QByteArray userId = gqlInputParamPtr->GetFieldArgumentValue("id").toByteArray();
	QByteArray productId = gqlInputParamPtr->GetFieldArgumentValue("productId").toByteArray();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			int index = dataModel->InsertNewItem();
			dataModel->SetData("name", "Roles", index);

			imtbase::CTreeItemModel* children = dataModel->AddTreeModel("children", index);

			if (m_roleInfoProviderCompPtr.IsValid()){
				imtauth::IUserInfo::RoleIds rolesIds = userInfoPtr->GetRoles(productId);
				if (rolesIds.isEmpty()){
					children->SetData("value", "No roles");
				}
				else{
					for (const QByteArray& productRoleId: rolesIds){
						istd::TDelPtr<const imtauth::IRole> rolePtr = m_roleInfoProviderCompPtr->GetRole(productRoleId);
						if (rolePtr.IsValid()){
							QByteArray roleId = rolePtr->GetRoleId();
							QString roleName = rolePtr->GetRoleName();
							QByteArray roleProductId = rolePtr->GetProductId();

							int childrenIndex = children->InsertNewItem();
							children->SetData("value", roleName + " (" + roleProductId + ")", childrenIndex);
						}
					}
				}
			}

			index = dataModel->InsertNewItem();
			dataModel->SetData("name", "Groups", index);

			children = dataModel->AddTreeModel("children", index);

			if (m_userGroupInfoProviderCompPtr.IsValid()){
				QByteArrayList groupIds = userInfoPtr->GetGroups();
				if (groupIds.isEmpty()){
					children->SetData("value", "No groups");
				}
				else{
					for (const QByteArray& groupId : groupIds){
						istd::TDelPtr<const imtauth::IUserGroupInfo> userGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
						if (userGroupInfoPtr.IsValid()){
							QString groupName = userGroupInfoPtr->GetName();

							int childrenIndex = children->InsertNewItem();
							children->SetData("value", groupName, childrenIndex);
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
			const sdl::imtauth::Users::CUserUpdateGqlRequest& userUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::imtauth::Users::UserUpdateRequestArguments arguments =userUpdateRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		I_CRITICAL();
		return false;
	}
	
	if (!arguments.input.Version_1_0->item.has_value()){
		I_CRITICAL();
		return false;
	}
	
	sdl::imtauth::Users::CUserData::V1_0 userData = *arguments.input.Version_1_0->item;

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


