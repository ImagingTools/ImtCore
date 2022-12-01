#include <imtauthdb/CRoleDatabaseDelegateComp.h>

// ACF includes
#include <iprm/TParamsPtr.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CRoleDatabaseDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_roleInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtauth::IRole> rolePtr = m_roleInfoFactCompPtr.CreateInstance();
	if (!rolePtr.IsValid()){
		return nullptr;
	}

	QByteArray roleId;
	if (record.contains("RoleId")){
		roleId = record.value("RoleId").toByteArray();
		rolePtr->SetRoleId(roleId);
	}

	QByteArray productId;
	if (record.contains("ProductId")){
		productId = record.value("ProductId").toByteArray();
		rolePtr->SetProductId(productId);
	}

	QString roleName;
	if (record.contains("Name")){
		roleName = record.value("Name").toString();
		rolePtr->SetRoleName(roleName);
	}

	QByteArray selectRolePermissions = QString("SELECT * FROM \"RolePermissions\" WHERE RoleId = '%1' AND ProductId = '%2'")
			.arg(qPrintable(roleId))
			.arg(qPrintable(productId))
			.toUtf8();

	QSqlError error;
	QSqlQuery rolePermissionsQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectRolePermissions, &error);

	imtauth::IRole::FeatureIds permissionsIds;
	imtauth::IRole::FeatureIds prohibitionsIds;

	while (rolePermissionsQuery.next()){
		QSqlRecord permissionRecord = rolePermissionsQuery.record();
		QByteArray permissionId;

		if (permissionRecord.contains("PermissionId")){
			permissionId = permissionRecord.value("PermissionId").toByteArray();
		}

		if (permissionRecord.contains("PermissionState")){
			bool permissionState = permissionRecord.value("PermissionState").toBool();

			if (permissionState){
				permissionsIds << permissionId;
			}
			else{
				prohibitionsIds << permissionId;
			}
		}
	}

	rolePtr->SetLocalPermissions(permissionsIds);
	rolePtr->SetProhibitions(prohibitionsIds);

	QByteArray parentRoles = QString("SELECT * FROM \"ParentRoles\" WHERE RoleId = '%1' AND ProductId = '%2';")
			.arg(qPrintable(roleId))
			.arg(qPrintable(productId))
			.toUtf8();

	QSqlQuery parentRolesQuery = m_databaseEngineCompPtr->ExecSqlQuery(parentRoles, &error);

	while (parentRolesQuery.next()){
		QSqlRecord parentRolesRecord = parentRolesQuery.record();

		if (parentRolesRecord.contains("ParentRoleId")){
			QByteArray parentRoleId = parentRolesRecord.value("ParentRoleId").toByteArray();
			rolePtr->IncludeRole(parentRoleId);
		}
	}

	return rolePtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CRoleDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& /*proposedObjectId*/,
		const QString& objectName,
		const QString& objectDescription,
		const istd::IChangeable* valuePtr) const
{
	const imtauth::IRole* rolePtr = dynamic_cast<const imtauth::IRole*>(valuePtr);
	if (rolePtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray roleId = rolePtr->GetRoleId();
	if (roleId.isEmpty()){
		return NewObjectQuery();
	}

	QByteArray productId = rolePtr->GetProductId();
	if (productId.isEmpty()){
		return NewObjectQuery();
	}

	QString roleName = rolePtr->GetRoleName();
	if (roleName.isEmpty()){
		roleName = objectName;
	}

	QString roleDescription = rolePtr->GetRoleDescription();

	NewObjectQuery retVal;
	retVal.query = QString("INSERT INTO \"Roles\"(RoleId, ProductId, Name, Description) VALUES('%1', '%2', '%3', '%4');")
			.arg(qPrintable(roleId))
			.arg(qPrintable(productId))
			.arg(roleName)
			.arg(roleDescription)
			.toLocal8Bit();
	retVal.objectName = roleName;

	imtauth::IRole::FeatureIds permissionsIds = rolePtr->GetLocalPermissions();
	imtauth::IRole::FeatureIds prohibitionsIds = rolePtr->GetProhibitions();

	for (const QByteArray& permissionId : permissionsIds){
		retVal.query += "\n" +
				QString("INSERT INTO \"RolePermissions\"(RoleId, ProductId, PermissionId, PermissionState) VALUES('%1', '%2', '%3', '%4');")
				.arg(qPrintable(roleId))
				.arg(qPrintable(productId))
				.arg(qPrintable(permissionId))
				.arg(true).toLocal8Bit();
	}

	for (const QByteArray& permissionId : prohibitionsIds){
		retVal.query += "\n" +
				QString("INSERT INTO \"RolePermissions\"(RoleId, ProductId, PermissionId, PermissionState) VALUES('%1', '%2', '%3', '%4');")
				.arg(qPrintable(roleId))
				.arg(qPrintable(productId))
				.arg(qPrintable(permissionId))
				.arg(false).toLocal8Bit();
	}

	QByteArrayList parentRoles = rolePtr->GetIncludedRoles();

	for (const QByteArray& parentRoleId : parentRoles){

		retVal.query += "\n" +
				QString("INSERT INTO \"ParentRoles\"(RoleId, ProductId, ParentRoleId, ParentProductId) VALUES('%1', '%2', '%3', '%4');")
				.arg(qPrintable(roleId))
				.arg(qPrintable(productId))
				.arg(qPrintable(parentRoleId))
				.arg(qPrintable(productId))
				.toLocal8Bit();

	}

	qDebug() << "retVal " << retVal.query;

	return retVal;
}


QByteArray CRoleDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		const imtauth::IRole* rolePtr = dynamic_cast<const imtauth::IRole*>(objectPtr.GetPtr());
		if (rolePtr == nullptr){
			return QByteArray();
		}

		QByteArray roleId = rolePtr->GetRoleId();
		if (roleId.isEmpty()){
			return QByteArray();
		}

		QByteArray productId = rolePtr->GetProductId();
		if (productId.isEmpty()){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM \"Roles\" WHERE RoleId = '%1' AND ProductId = '%2';")
				.arg(qPrintable(roleId))
				.arg(qPrintable(productId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CRoleDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const istd::IChangeable& object) const
{
	const imtauth::IRole* newRolePtr = dynamic_cast<const imtauth::IRole*>(&object);
	if (newRolePtr == nullptr){
		return QByteArray();
	}

	const imtauth::IRole* oldRolePtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		oldRolePtr = dynamic_cast<const imtauth::IRole*>(objectPtr.GetPtr());
	}

	if (oldRolePtr == nullptr){
		return QByteArray();
	}

	QByteArray oldRoleId = oldRolePtr->GetRoleId();
	QByteArray newRoleId = newRolePtr->GetRoleId();

	QByteArray productId = oldRolePtr->GetProductId();
	QByteArray newProductId = newRolePtr->GetProductId();

	QString newRoleName = newRolePtr->GetRoleName();

	QString description = newRolePtr->GetRoleDescription();

	QByteArray retVal = QString("UPDATE \"Roles\" SET RoleId ='%1', ProductId ='%2', Name = '%3', Description = '%4' WHERE RoleId ='%5' AND ProductId = '%6';")
			.arg(qPrintable(newRoleId))
			.arg(qPrintable(newProductId))
			.arg(qPrintable(newRoleName))
			.arg(description)
			.arg(qPrintable(oldRoleId))
			.arg(qPrintable(productId)).toLocal8Bit();

	imtauth::IRole::FeatureIds newPermissionsIds = newRolePtr->GetLocalPermissions();
	imtauth::IRole::FeatureIds oldPermissionsIds = oldRolePtr->GetLocalPermissions();

	imtauth::IRole::FeatureIds addedPermissions;
	imtauth::IRole::FeatureIds removedPermissions;

	for (const QByteArray& permissionId : newPermissionsIds){
		if (!oldPermissionsIds.contains(permissionId)){
			addedPermissions << permissionId;
		}
	}

	// Calculate removed permission
	for (const QByteArray& permissionId : oldPermissionsIds){
		if (!newPermissionsIds.contains(permissionId)){
			removedPermissions << permissionId;
		}
	}

	for (const QByteArray& permissionId : addedPermissions){
		retVal += QString("INSERT INTO \"RolePermissions\" (RoleId, ProductId, PermissionId, PermissionState) VALUES('%1', '%2', '%3', '%4');")
					.arg(qPrintable(newRoleId))
					.arg(qPrintable(newProductId))
					.arg(qPrintable(permissionId))
					.arg(true).toLocal8Bit();
	}

	for (const QByteArray& permissionId : removedPermissions){
		retVal += QString("DELETE FROM \"RolePermissions\" WHERE RoleId = '%1' AND ProductId = '%2' AND PermissionId = '%3';")
				.arg(qPrintable(newRoleId))
				.arg(qPrintable(newProductId))
				.arg(qPrintable(permissionId))
				.toLocal8Bit();
	}

	imtauth::IRole::FeatureIds newProhibitionsIds = newRolePtr->GetProhibitions();
	imtauth::IRole::FeatureIds oldProhibitionsIds = oldRolePtr->GetProhibitions();

	imtauth::IRole::FeatureIds addedProhibitions;
	imtauth::IRole::FeatureIds removedProhibitions;

	for (const QByteArray& permissionId : newProhibitionsIds){
		if (!oldProhibitionsIds.contains(permissionId)){
			addedProhibitions << permissionId;
		}
	}

	// Calculate removed permission
	for (const QByteArray& permissionId : oldProhibitionsIds){
		if (!newProhibitionsIds.contains(permissionId)){
			removedProhibitions << permissionId;
		}
	}

	for (const QByteArray& permissionId : addedProhibitions){
		retVal += QString("INSERT INTO \"RolePermissions\" (RoleId, ProductId, PermissionId, PermissionState) VALUES('%1', '%2', '%3', '%4');")
					.arg(qPrintable(newRoleId))
					.arg(qPrintable(newProductId))
					.arg(qPrintable(permissionId))
					.arg(false).toLocal8Bit();
	}

	for (const QByteArray& permissionId : removedProhibitions){
		retVal += QString("DELETE FROM \"RolePermissions\" WHERE RoleId = '%1' AND ProductId = '%2' AND PermissionId = '%3';")
				.arg(qPrintable(newRoleId))
				.arg(qPrintable(newProductId))
				.arg(qPrintable(permissionId))
				.toLocal8Bit();
	}

	QByteArrayList newParentRolesIds = newRolePtr->GetIncludedRoles();
	QByteArrayList oldParentRolesIds = oldRolePtr->GetIncludedRoles();

	QByteArrayList addedParentRolesIds;
	QByteArrayList removedParentRolesIds;

	// Calculate added parents roles
	for (const QByteArray& parentRoleId : newParentRolesIds){
		if (!oldParentRolesIds.contains(parentRoleId)){
			addedParentRolesIds << parentRoleId;
		}
	}

	// Calculate removed parents roles
	for (const QByteArray& parentRoleId : oldParentRolesIds){
		if (!newParentRolesIds.contains(parentRoleId)){
			removedParentRolesIds << parentRoleId;
		}
	}

	for (const QByteArray& parentRoleId : addedParentRolesIds){
		retVal += QString("INSERT INTO \"ParentRoles\" (RoleId, ProductId, ParentRoleId, ParentProductId) VALUES('%1', '%2', '%3', '%4');")
					.arg(qPrintable(newRoleId))
					.arg(qPrintable(newProductId))
					.arg(qPrintable(parentRoleId))
					.arg(qPrintable(newProductId))
					.toLocal8Bit();
	}

	for (const QByteArray& parentRoleId : removedParentRolesIds){
		retVal += QString("DELETE FROM \"ParentRoles\" WHERE RoleId = '%1' AND ProductId = '%2' AND ParentRoleId = '%3';")
				.arg(qPrintable(newRoleId))
				.arg(qPrintable(newProductId))
				.arg(qPrintable(parentRoleId))
				.toLocal8Bit();
	}

	return retVal;
}


QByteArray CRoleDatabaseDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& newObjectName) const
{
	const imtauth::IRole* rolePtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		rolePtr = dynamic_cast<const imtauth::IRole*>(objectPtr.GetPtr());
	}

	if (rolePtr == nullptr){
		return QByteArray();
	}

	QByteArray roleId = rolePtr->GetRoleId();
	QByteArray productId = rolePtr->GetProductId();

	QByteArray retVal = QString("UPDATE \"Roles\" SET Name = '%1' WHERE RoleId ='%2' AND ProductId = '%3';")
			.arg(qPrintable(newObjectName))
			.arg(qPrintable(roleId))
			.arg(qPrintable(productId)).toLocal8Bit();

	return retVal;
}


QByteArray CRoleDatabaseDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& description) const
{
	return QByteArray();
}


QByteArray CRoleDatabaseDelegateComp::GetObjectIdFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	QByteArray objectId;

	if (record.contains("RoleId")){
		objectId = record.value("RoleId").toByteArray();
	}

	if (m_separatorObjectIdAttrPtr.IsValid()){
		objectId += *m_separatorObjectIdAttrPtr;
	}

	if (record.contains("ProductId")){
		objectId += record.value("ProductId").toByteArray();
	}

	return objectId;
}


QByteArray CRoleDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		QString objectIdStr = objectId;

		QByteArray roleId, productId;

		if (m_separatorObjectIdAttrPtr.IsValid()){
			QStringList splitData = objectIdStr.split(*m_separatorObjectIdAttrPtr);

			if (splitData.count() == 2){
				roleId = splitData[0].toUtf8();
				productId = splitData[1].toUtf8();
			}
			else{
				return QByteArray();
			}
		}

		return QString("SELECT * FROM \"%1\" WHERE RoleId = '%2' AND ProductId = '%3'")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(roleId))
					.arg(qPrintable(productId))
					.toLocal8Bit();
	}
	else{
		QString sortQuery;
		QString filterQuery;
		if (count == 0){
			return QByteArray();
		}
		if (paramsPtr != nullptr){
			if (!CreateFilterQuery(*paramsPtr, filterQuery)){
				return QByteArray();
			}

			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(paramsPtr, "Filter");
			if (collectionFilterParamPtr.IsValid()){
				if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
					return QByteArray();
				}
			}
		}

		QByteArray paginationQuery;
		if (!CreatePaginationQuery(offset, count, paginationQuery)){
			return QByteArray();
		}

		// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
		QString retVal = "(SELECT * FROM";
		retVal += QString(" \"") + qPrintable(*m_tableNameAttrPtr);
		retVal += QString("\" ") + filterQuery;
		retVal += QString(" ") + qPrintable(paginationQuery) + ")";
		retVal += QString(" ") + sortQuery;

		return retVal.toLocal8Bit();
	}

	return QByteArray();
}


// protected methods

void GenerateDifferences(
			imtauth::IRole::FeatureIds& currentPermissions,
			imtauth::IRole::FeatureIds& newPermissions,
			imtauth::IRole::FeatureIds& addedPermissions,
			imtauth::IRole::FeatureIds& removedPermissions)
{
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CRoleDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr();
}


bool CRoleDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
//	if (record.contains("Description")){
//		QString description = record.value("Description").toString();

//		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::ElementInfoType::EIT_DESCRIPTION, description);
//	}

	return true;
}


} // namespace imtauthdb


