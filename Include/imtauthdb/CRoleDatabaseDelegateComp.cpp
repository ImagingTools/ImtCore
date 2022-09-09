#include <imtauthdb/CRoleDatabaseDelegateComp.h>


// ImtCore includes


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

	QString roleName;
	if (record.contains("Name")){
		roleName = record.value("Name").toString();
		rolePtr->SetRoleName(roleName);
	}

	QByteArray selectRolePermissions = QString("SELECT * from RolePermissions WHERE RoleId = '%1'").arg(qPrintable(roleId)).toUtf8();

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

//	QByteArray parentRoles = QString("SELECT * from ParentRoles WHERE RoleId = '%1'").arg(qPrintable(roleId)).toUtf8();
//	QSqlQuery parentRolesQuery = m_databaseEngineCompPtr->ExecSqlQuery(parentRoles, &error);

//	const QList<const imtauth::IRole*> parents;

//	while (parentRolesQuery.next()){
//		QSqlRecord parentRolesRecord = parentRolesQuery.record();

//		if (parentRolesRecord.contains("ParentRoleId")){
//			QByteArray parentRoleId = parentRolesRecord.value("ParentRoleId").toByteArray();


//		}
//	}

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

	QString roleName = rolePtr->GetRoleName();
	if (roleName.isEmpty()){
		roleName = objectName;
	}

	NewObjectQuery retVal;
	retVal.query = QString("INSERT INTO Roles(RoleId, Name, Description) VALUES('%1', '%2', '%3');")
			.arg(qPrintable(roleId))
			.arg(roleName)
			.arg("")
			.toLocal8Bit();
	retVal.objectName = roleName;

	imtauth::IRole::FeatureIds permissionsIds = rolePtr->GetPermissions();
	imtauth::IRole::FeatureIds prohibitionsIds = rolePtr->GetProhibitions();

	for (const QByteArray& permissionId : permissionsIds){
		retVal.query += "\n" +
				QString("INSERT INTO RolePermissions(RoleId, PermissionId, PermissionState) VALUES('%1', '%2', '%3');")
				.arg(qPrintable(roleId))
				.arg(qPrintable(permissionId))
				.arg(true).toLocal8Bit();
	}

	for (const QByteArray& permissionId : prohibitionsIds){
		retVal.query += "\n" +
				QString("INSERT INTO RolePermissions(RoleId, PermissionId, PermissionState) VALUES('%1', '%2', '%3');")
				.arg(qPrintable(roleId))
				.arg(qPrintable(permissionId))
				.arg(false).toLocal8Bit();
	}

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

		QByteArray retVal = QString("DELETE FROM Roles WHERE RoleId = '%1';").arg(qPrintable(roleId)).toLocal8Bit();

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

	QString newRoleName = newRolePtr->GetRoleName();

	QByteArray retVal = QString("UPDATE Roles SET RoleId ='%1', Name = '%2' WHERE RoleId ='%3';")
			.arg(qPrintable(newRoleId))
			.arg(qPrintable(newRoleName))
			.arg(qPrintable(oldRoleId)).toLocal8Bit();

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
		retVal += QString("INSERT INTO RolePermissions (RoleId, PermissionId, PermissionState) VALUES('%1', '%2', '%3');")
					.arg(qPrintable(newRoleId))
					.arg(qPrintable(permissionId))
					.arg(true).toLocal8Bit();
	}

	for (const QByteArray& permissionId : removedPermissions){
		retVal += QString("DELETE FROM RolePermissions WHERE RoleId = '%1' AND PermissionId = '%2';")
				.arg(qPrintable(newRoleId))
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
		retVal += QString("INSERT INTO RolePermissions (RoleId, PermissionId, PermissionState) VALUES('%1', '%2', '%3');")
					.arg(qPrintable(newRoleId))
					.arg(qPrintable(permissionId))
					.arg(false).toLocal8Bit();
	}

	for (const QByteArray& permissionId : removedProhibitions){
		retVal += QString("DELETE FROM RolePermissions WHERE RoleId = '%1' AND PermissionId = '%2';")
				.arg(qPrintable(newRoleId))
				.arg(qPrintable(permissionId))
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

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray roleId = rolePtr->GetRoleId();

	QByteArray retVal = QString("UPDATE Roles SET Name = '%1' WHERE RoleId ='%2';")
			.arg(qPrintable(newObjectName))
			.arg(qPrintable(roleId)).toLocal8Bit();

	return retVal;
}


QByteArray CRoleDatabaseDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& description) const
{
	return QByteArray();
}


// protected methods

void GenerateDifferences(
			imtauth::IRole::FeatureIds& currentPermissions,
			imtauth::IRole::FeatureIds& newPermissions,
			imtauth::IRole::FeatureIds& addedPermissions,
			imtauth::IRole::FeatureIds& removedPermissions)
{
	// Calculate added permission
//	for (const QByteArray& permissionId : newPermissions){
//		if (!currentPermissions.contains(permissionId)){
//			addedPermissions << permissionId;
//		}
//	}

//	// Calculate removed permission
//	for (const QByteArray& permissionId : currentPermissions){
//		if (!newPermissions.contains(permissionId)){
//			removedPermissions << permissionId;
//		}
//	}
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CRoleDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr();
}


bool CRoleDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& /*record*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return true;
}


} // namespace imtauthdb


