#include <imtauthgql/CRoleCollectionControllerComp.h>


// ACF includes
#include <iqt/iqt.h>
#include <iprm/CTextParam.h>



#include <imtbase/CComplexCollectionFilterHelper.h>

// ImtCore includes
#include <imtauth/CRole.h>


namespace imtauthgql
{


// protected methods

bool CRoleCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::imtauth::Roles::CRoleData::V1_0& roleDataRepresentation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& errorMessage) const
{
	imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<imtauth::CIdentifiableRoleInfo*>(&object);
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	QByteArray roleId;
	if (roleDataRepresentation.RoleId){
		roleId = *roleDataRepresentation.RoleId;
	}

	if (roleId.isEmpty()){
		errorMessage = QString("Role-ID can't be empty!");
		return false;
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
				if (collectionId != objectId){
					QByteArray currentRoleId = currentRoleInfoPtr->GetRoleId();
					QByteArray currentProductId = currentRoleInfoPtr->GetProductId();

					if (currentRoleId == roleId && currentProductId == productId){
						errorMessage = QString("Role with ID: '%1' already exists").arg(qPrintable(currentRoleId));
						return false;
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
	roleInfoPtr->SetRoleName(roleName);

	QString roleDescription;
	if (roleDataRepresentation.Description){
		roleDescription = *roleDataRepresentation.Description;
	}
	roleInfoPtr->SetRoleDescription(roleDescription);

	QByteArray parentRoles;
	if (roleDataRepresentation.ParentRoles){
		parentRoles = *roleDataRepresentation.ParentRoles;
	}
	if (!parentRoles.isEmpty()){
		QByteArrayList parentRoleIds = parentRoles.split(';');
		parentRoleIds.removeAll("");

		for (const QByteArray& parentRoleId : parentRoleIds){
			if (parentRoleId == objectId || !roleInfoPtr->IncludeRole(parentRoleId)){
				errorMessage = QT_TR_NOOP(QString("Unable include role '%1' to the role '%2'. Check the dependencies between them.")
											.arg(qPrintable(parentRoleId), qPrintable(roleId)));

				return false;
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
						return false;
					}
				}
			}
		}
	}

	return true;
}


// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus CRoleCollectionControllerComp::OnGetObjectVisualStatus(
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

	if (response.Version_1_0->Text->isEmpty()){
		response.Version_1_0->Text = "<no name>";
	}

	QString translation = iqt::GetTranslation(
		m_translationManagerCompPtr.GetPtr(),
		QString(QT_TR_NOOP("Roles")).toUtf8(),
		languageId,
		"CRoleCollectionControllerComp");
	response.Version_1_0->Text = translation + QByteArrayLiteral(" / ") + *response.Version_1_0->Text;
	return response;
}


// reimplemented (sdl::imtauth::Roles::CRoleCollectionControllerCompBase)

bool CRoleCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Roles::CRolesListGqlRequest& rolesListRequest,
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

	sdl::imtauth::Roles::RolesListRequestInfo requestInfo = rolesListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = QByteArray(objectId);
	}

	if (requestInfo.items.isRoleNameRequested){
		representationObject.RoleName = QString(roleInfoPtr->GetRoleName());
	}

	if (requestInfo.items.isRoleIdRequested){
		representationObject.RoleId = QByteArray(roleInfoPtr->GetRoleId());
	}

	if (requestInfo.items.isRoleDescriptionRequested){
		representationObject.RoleDescription = QString(roleInfoPtr->GetRoleDescription());
	}

	if (requestInfo.items.isParentRolesRequested){
		representationObject.ParentRoles = QByteArray(roleInfoPtr->GetIncludedRoles().join(';'));
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.ProductId = QByteArray(roleInfoPtr->GetProductId());
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.TypeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();
		
		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = QString(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime().toUTC();

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.LastModified = QString(lastModified);
	}

	return true;
}


istd::IChangeable* CRoleCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtauth::Roles::CRoleData::V1_0& roleDataRepresentation,
			QByteArray& newObjectId,
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

	if (!FillObjectFromRepresentation(roleDataRepresentation, *roleInfoPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return roleInstancePtr.PopPtr();
}


bool CRoleCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtauth::Roles::CRoleItemGqlRequest& roleItemRequest,
			sdl::imtauth::Roles::CRoleData::V1_0& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<const imtauth::CIdentifiableRoleInfo*>(&data);
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Roles::RoleItemRequestArguments arguments = roleItemRequest.GetRequestedArguments();

	QByteArray id = roleInfoPtr->GetObjectUuid();
	representationPayload.Id = QByteArray(id);

	QByteArray roleId = roleInfoPtr->GetRoleId();
	representationPayload.RoleId = QByteArray(roleId);

	QByteArray productId = roleInfoPtr->GetProductId();
	representationPayload.ProductId = QByteArray(productId);

	QString name = roleInfoPtr->GetRoleName();
	representationPayload.Name = QString(name);

	QString description = roleInfoPtr->GetRoleDescription();
	representationPayload.Description = QString(description);

	QByteArrayList parentsRolesIds = roleInfoPtr->GetIncludedRoles();
	std::sort(parentsRolesIds.begin(), parentsRolesIds.end());
	representationPayload.ParentRoles = QByteArray(parentsRolesIds.join(';'));

	imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();
	std::sort(permissions.begin(), permissions.end());
	representationPayload.Permissions = QByteArray(permissions.join(';'));

	representationPayload.IsDefault = bool(roleInfoPtr->IsDefault());
	representationPayload.IsGuest = bool(roleInfoPtr->IsGuest());

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


void CRoleCollectionControllerComp::SetAdditionalFilters(
	const imtgql::CGqlRequest& gqlRequest,
	imtbase::CComplexCollectionFilter& complexFilter) const
{
	QByteArray productId = GetInputArgumentFromRequest(gqlRequest, "ProductId").toByteArray();
	if (productId.isEmpty()){
		SendErrorMessage(0, "Unable to set product-ID to collection filter", "CRoleCollectionControllerComp");
		return;
	}
	
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "ProductId";
	fieldFilter.filterValue = productId;
	
	complexFilter.AddFieldFilter(fieldFilter);
}


bool CRoleCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::imtauth::Roles::CRoleUpdateGqlRequest& roleUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::imtauth::Roles::RoleUpdateRequestArguments arguments = roleUpdateRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return false;
	}
	
	if (!arguments.input.Version_1_0->Item.has_value()){
		Q_ASSERT(false);
		return false;
	}
	
	sdl::imtauth::Roles::CRoleData::V1_0 roleDataRepresentation = *roleUpdateRequest.GetRequestedArguments().input.Version_1_0->Item;
	imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<imtauth::CIdentifiableRoleInfo*>(&object);
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to cast role instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	QByteArrayList includedRole = roleInfoPtr->GetIncludedRoles();
	for (const QByteArray& roleId : includedRole){
		roleInfoPtr->ExcludeRole(roleId);
	}

	QByteArray objectId = roleInfoPtr->GetObjectUuid();
	return FillObjectFromRepresentation(roleDataRepresentation, object, objectId, errorMessage);
}


} // namespace imtauth


