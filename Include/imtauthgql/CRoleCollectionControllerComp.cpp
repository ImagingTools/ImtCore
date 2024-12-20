#include <imtauthgql/CRoleCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <iprm/CTextParam.h>
#include <iprm/TParamsPtr.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CRole.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Roles::V1_0::CRoleCollectionControllerCompBase)

bool CRoleCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Roles::V1_0::CRolesListGqlRequest& rolesListRequest,
			sdl::imtauth::Roles::CRoleItem::V1_0& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const imtauth::IRole* roleInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr roleDataPtr;
	if (objectCollectionIterator.GetObjectData(roleDataPtr)){
		roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
	}

	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Roles::V1_0::RolesListRequestInfo requestInfo = rolesListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id.reset(new QByteArray(objectId));
	}

	if (requestInfo.items.isRoleNameRequested){
		representationObject.RoleName.reset(new QString(roleInfoPtr->GetRoleName()));
	}

	if (requestInfo.items.isRoleIdRequested){
		representationObject.RoleId.reset(new QByteArray(roleInfoPtr->GetRoleId()));
	}

	if (requestInfo.items.isRoleDescriptionRequested){
		representationObject.RoleDescription.reset(new QString(roleInfoPtr->GetRoleDescription()));
	}

	if (requestInfo.items.isParentRolesRequested){
		representationObject.ParentRoles.reset(new QByteArray(roleInfoPtr->GetIncludedRoles().join(';')));
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.ProductId.reset(new QByteArray(roleInfoPtr->GetProductId()));
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.TypeId.reset(new QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId)));
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


istd::IChangeable* CRoleCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtauth::Roles::CRoleData::V1_0& roleDataRepresentation,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_roleInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_roleInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtauth::IRole> roleInstancePtr = m_roleInfoFactCompPtr.CreateInstance();
	if (!roleInstancePtr.IsValid()){
		errorMessage = QString("Unable to create role instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return nullptr;
	}

	imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<imtauth::CIdentifiableRoleInfo*>(roleInstancePtr.GetPtr());
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to cast role instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return nullptr;
	}

	QByteArray id;
	if (roleDataRepresentation.Id){
		id = *roleDataRepresentation.Id;
	}
	if (id.isEmpty()){
		id = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	roleInfoPtr->SetObjectUuid(id);
	newObjectId = id;

	QByteArray roleId;
	if (roleDataRepresentation.RoleId){
		roleId = *roleDataRepresentation.RoleId;
	}
	if (roleId.isEmpty()){
		errorMessage = QString("Role-ID can't be empty!");
		return nullptr;
	}

	roleInfoPtr->SetRoleId(roleId);

	QByteArray productId;
	if (roleDataRepresentation.ProductId){
		productId = *roleDataRepresentation.ProductId;
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			imtauth::IRole* currentRoleInfoPtr = dynamic_cast<imtauth::IRole*>(dataPtr.GetPtr());
			if (currentRoleInfoPtr != nullptr){
				if (collectionId != id){
					QByteArray currentRoleId = currentRoleInfoPtr->GetRoleId();
					QByteArray currentProductId = currentRoleInfoPtr->GetProductId();

					if (currentRoleId == roleId && currentProductId == productId){
						errorMessage = QString("Role with ID: '%1' already exists").arg(qPrintable(currentRoleId));
						return nullptr;
					}
				}
			}
		}
	}

	roleInfoPtr->SetProductId(productId);

	QString roleName;
	if (roleDataRepresentation.Name){
		roleName = *roleDataRepresentation.Name;
	}
	name = roleName;
	roleInfoPtr->SetRoleName(roleName);

	QString roleDescription;
	if (roleDataRepresentation.Description){
		roleDescription = *roleDataRepresentation.Description;
	}
	description = roleDescription;
	roleInfoPtr->SetRoleDescription(roleDescription);

	QByteArray parentRoles;
	if (roleDataRepresentation.ParentRoles){
		parentRoles = *roleDataRepresentation.ParentRoles;
	}
	if (!parentRoles.isEmpty()){
		QByteArrayList parentRoleIds = parentRoles.split(';');
		parentRoleIds.removeAll("");

		for (const QByteArray& parentRoleId : parentRoleIds){
			if (parentRoleId == id || !roleInfoPtr->IncludeRole(parentRoleId)){
				errorMessage = QT_TR_NOOP(QString("Unable include role '%1' to the role '%2'. Check the dependencies between them.")
										.arg(qPrintable(parentRoleId))
										.arg(qPrintable(roleId)));

				return nullptr;
			}
		}
	}

	QByteArray permissions;
	if (roleDataRepresentation.Permissions){
		permissions = *roleDataRepresentation.Permissions;
	}
	QByteArrayList permissionIds = permissions.split(';');
	permissionIds.removeAll("");
	roleInfoPtr->SetLocalPermissions(permissionIds);

	bool isGuest = bool(roleDataRepresentation.IsGuest && *roleDataRepresentation.IsGuest);
	roleInfoPtr->SetGuest(isGuest);

	bool isDefault = bool(roleDataRepresentation.IsDefault && *roleDataRepresentation.IsDefault);
	roleInfoPtr->SetDefault(isDefault);

	if (isGuest || isDefault){
		if (m_gqlRequestProviderCompPtr.IsValid()){
			const imtgql::IGqlRequest* gqlRequestPtr = m_gqlRequestProviderCompPtr->GetGqlRequest();
			if (gqlRequestPtr != nullptr){
				const imtgql::IGqlContext* gqlContextPtr = gqlRequestPtr->GetRequestContext();
				if (gqlContextPtr != nullptr){
					if (!gqlContextPtr->GetUserInfo()->IsAdmin()){
						errorMessage = QString("Only the admin can change the default/guest role");
						return nullptr;
					}
				}
			}
		}
	}

	return roleInstancePtr.PopPtr();
}


bool CRoleCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtauth::Roles::V1_0::CRoleItemGqlRequest& roleItemRequest,
			sdl::imtauth::Roles::CRoleDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<const imtauth::CIdentifiableRoleInfo*>(&data);
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Roles::V1_0::RoleItemRequestArguments arguments = roleItemRequest.GetRequestedArguments();
	sdl::imtauth::Roles::CRoleData::V1_0 roleData;

	QByteArray id = roleInfoPtr->GetObjectUuid();
	roleData.Id.reset(new QByteArray(id));

	QByteArray roleId = roleInfoPtr->GetRoleId();
	roleData.RoleId.reset(new QByteArray(roleId));

	QByteArray productId = roleInfoPtr->GetProductId();
	roleData.ProductId.reset(new QByteArray(productId));

	QString name = roleInfoPtr->GetRoleName();
	roleData.Name.reset(new QString(name));

	QString description = roleInfoPtr->GetRoleDescription();
	roleData.Description.reset(new QString(description));

	QByteArrayList parentsRolesIds = roleInfoPtr->GetIncludedRoles();
	std::sort(parentsRolesIds.begin(), parentsRolesIds.end());
	roleData.ParentRoles.reset(new QByteArray(parentsRolesIds.join(';')));

	imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();
	std::sort(permissions.begin(), permissions.end());
	roleData.Permissions.reset(new QByteArray(permissions.join(';')));

	roleData.IsDefault.reset(new bool(roleInfoPtr->IsDefault()));
	roleData.IsGuest.reset(new bool(roleInfoPtr->IsGuest()));

	representationPayload.RoleData = std::make_unique<sdl::imtauth::Roles::CRoleData::V1_0>(roleData);

	return true;
}


imtbase::CTreeItemModel* CRoleCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const imtgql::CGqlObject paramsPtr = gqlRequest.GetParams();

	QByteArray productId;
	productId = paramsPtr.GetFieldArgumentValue("ProductId").toByteArray();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray roleObjectId = GetObjectIdFromInputParams(paramsPtr);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(roleObjectId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return nullptr;
		}

		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();
		QByteArray roleProductId = roleInfoPtr->GetProductId();

		int index = dataModelPtr->InsertNewItem();
		dataModelPtr->SetData("Id", "ParentRoles", index);
		dataModelPtr->SetData("Name", "Parent Roles", index);
		imtbase::CTreeItemModel* children = dataModelPtr->AddTreeModel("Children", index);

		QByteArrayList parentRolesIds = roleInfoPtr->GetIncludedRoles();

		if (parentRolesIds.isEmpty()){
			children->SetData("Value", "No parent roles");
		}
		else{
			for (const QByteArray& parentRoleId : parentRolesIds){
				imtbase::IObjectCollection::DataPtr parentDataPtr;
				if (m_objectCollectionCompPtr->GetObjectData(parentRoleId, parentDataPtr)){
					const imtauth::IRole* parentRoleInfoPtr = dynamic_cast<const imtauth::IRole*>(parentDataPtr.GetPtr());
					if (parentRoleInfoPtr != nullptr){
						QString parentRoleName = parentRoleInfoPtr->GetRoleName();

						int childrenIndex = children->InsertNewItem();

						children->SetData("Value", parentRoleName, childrenIndex);
					}
				}
			}
		}

		index = dataModelPtr->InsertNewItem();

		dataModelPtr->SetData("Id", "Permissions", index);
		dataModelPtr->SetData("Name", "Permissions", index);
		children = dataModelPtr->AddTreeModel("Children", index);

		imtauth::IRole::FeatureIds permissionsIds = roleInfoPtr->GetPermissions();

		if (permissionsIds.isEmpty()){
			children->SetData("Value", "No permissions");
		}
		else{
			for (const QByteArray& permissionId : permissionsIds){
				int childrenIndex = children->InsertNewItem();

				children->SetData("Value", permissionId, childrenIndex);
			}
		}
	}

	return rootModelPtr.PopPtr();
}


void CRoleCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	BaseClass::SetObjectFilter(gqlRequest, objectFilterModel, filterParams);

	const imtgql::CGqlObject* inputParamObjectPtr = gqlRequest.GetParamObject("input");
	if (inputParamObjectPtr == nullptr){
		return;
	}

	QByteArray productId = inputParamObjectPtr->GetFieldArgumentValue("ProductId").toByteArray();
	if (productId.isEmpty()){
		productId = gqlRequest.GetHeader("ProductId");
	}

	iprm::CTextParam* textParamPtr = new iprm::CTextParam;
	textParamPtr->SetText(productId);

	filterParams.SetEditableParameter("ProductId", textParamPtr, true);
}


bool CRoleCollectionControllerComp::UpdateObjectFromRepresentationRequest(const imtgql::CGqlRequest& /*rawGqlRequest*/, const sdl::imtauth::Roles::V1_0::CRoleUpdateGqlRequest& /*roleUpdateRequest*/, istd::IChangeable& /*object*/, QString& /*errorMessage*/) const
{
	return false;
}


} // namespace imtauth


