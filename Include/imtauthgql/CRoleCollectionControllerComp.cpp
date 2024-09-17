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

// reimplemented (sdl::imtauth::Roles::CRoleCollectionControllerCompBase)

bool CRoleCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Roles::CRolesListGqlRequest& rolesListRequest,
			sdl::imtauth::Roles::CRoleItem& representationObject,
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
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isRoleNameRequested){
		representationObject.SetRoleName(roleInfoPtr->GetRoleName());
	}

	if (requestInfo.items.isRoleIdRequested){
		representationObject.SetRoleId(roleInfoPtr->GetRoleId());
	}

	if (requestInfo.items.isRoleDescriptionRequested){
		representationObject.SetRoleDescription(roleInfoPtr->GetRoleDescription());
	}

	if (requestInfo.items.isParentRolesRequested){
		representationObject.SetParentRoles(roleInfoPtr->GetIncludedRoles().join(';'));
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.SetProductId(roleInfoPtr->GetProductId());
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.SetTypeId(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
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


istd::IChangeable* CRoleCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtauth::Roles::CRoleData& roleDataRepresentation,
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

	QByteArray id = roleDataRepresentation.GetId();
	if (id.isEmpty()){
		id = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	roleInfoPtr->SetObjectUuid(id);
	newObjectId = id;

	QByteArray roleId = roleDataRepresentation.GetRoleId();
	if (roleId.isEmpty()){
		errorMessage = QString("Role-ID can't be empty!");
		return nullptr;
	}

	roleInfoPtr->SetRoleId(roleId);

	QByteArray productId = roleDataRepresentation.GetProductId();
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

	QString roleName = roleDataRepresentation.GetName();
	name = roleName;
	roleInfoPtr->SetRoleName(roleName);

	QString roleDescription = roleDataRepresentation.GetDescription();
	description = roleDescription;
	roleInfoPtr->SetRoleDescription(roleDescription);

	QByteArray parentRoles = roleDataRepresentation.GetParentRoles();
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

	QByteArray permissions = roleDataRepresentation.GetPermissions();
	QByteArrayList permissionIds = permissions.split(';');
	permissionIds.removeAll("");
	roleInfoPtr->SetLocalPermissions(permissionIds);

	bool isGuest = roleDataRepresentation.GetIsGuest();
	roleInfoPtr->SetGuest(isGuest);

	bool isDefault = roleDataRepresentation.GetIsDefault();
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
			const sdl::imtauth::Roles::CRoleItemGqlRequest& roleItemRequest,
			sdl::imtauth::Roles::CRoleDataPayload& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<const imtauth::CIdentifiableRoleInfo*>(&data);
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Roles::RoleItemRequestArguments arguments = roleItemRequest.GetRequestedArguments();
	sdl::imtauth::Roles::CRoleData roleData;

	QByteArray id = roleInfoPtr->GetObjectUuid();
	roleData.SetId(id);

	QByteArray roleId = roleInfoPtr->GetRoleId();
	roleData.SetRoleId(roleId);

	QByteArray productId = roleInfoPtr->GetProductId();
	roleData.SetProductId(productId);

	QString name = roleInfoPtr->GetRoleName();
	roleData.SetName(name);

	QString description = roleInfoPtr->GetRoleDescription();
	roleData.SetDescription(description);

	QByteArrayList parentsRolesIds = roleInfoPtr->GetIncludedRoles();
	std::sort(parentsRolesIds.begin(), parentsRolesIds.end());
	roleData.SetParentRoles(parentsRolesIds.join(';'));

	imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();
	std::sort(permissions.begin(), permissions.end());
	roleData.SetPermissions(permissions.join(';'));

	roleData.SetIsDefault(roleInfoPtr->IsDefault());
	roleData.SetIsGuest(roleInfoPtr->IsGuest());

	representationPayload.SetRoleData(roleData);

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
	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

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
		dataModelPtr->SetData("Name", "Parent Roles", index);
		imtbase::CTreeItemModel* children = dataModelPtr->AddTreeModel("Children", index);

		QByteArrayList parentRolesIds = roleInfoPtr->GetIncludedRoles();

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

		index = dataModelPtr->InsertNewItem();

		dataModelPtr->SetData("Name", "Permissions", index);
		children = dataModelPtr->AddTreeModel("Children", index);

		imtauth::IRole::FeatureIds permissionsIds = roleInfoPtr->GetPermissions();

		for (const QByteArray& permissionId : permissionsIds){
			int childrenIndex = children->InsertNewItem();

			children->SetData("Value", permissionId, childrenIndex);
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr.PopPtr();
}


void CRoleCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlObject& /*viewParamsGql*/,
			iprm::CParamsSet* filterParams) const
{
	const imtgql::CGqlObject* inputParamObjectPtr = gqlRequest.GetParamObject("input");
	if (inputParamObjectPtr == nullptr){
		return;
	}

	iser::ISerializable* objectFilterPtr = filterParams->GetEditableParameter("ObjectFilter");
	if (objectFilterPtr == nullptr){
		return;
	}

	iprm::CParamsSet* objectParamSetPtr = dynamic_cast<iprm::CParamsSet*>(objectFilterPtr);
	if (objectParamSetPtr == nullptr){
		return;
	}

	QByteArray productId = inputParamObjectPtr->GetFieldArgumentValue("ProductId").toByteArray();
	if (productId.isEmpty()){
		const imtgql::CGqlObject* additionalParamsPtr = inputParamObjectPtr->GetFieldArgumentObjectPtr("addition");
		if (additionalParamsPtr != nullptr) {
			productId = additionalParamsPtr->GetFieldArgumentValue("ProductId").toByteArray();
		}
	}

	iprm::CTextParam* textParamPtr = new iprm::CTextParam;
	textParamPtr->SetText(productId);

	objectParamSetPtr->SetEditableParameter("ProductId", textParamPtr, true);
}


} // namespace imtauth


