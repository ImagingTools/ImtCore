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
    if (record.contains("RoleName")){
        roleName = record.value("RoleName").toString();
		rolePtr->SetRoleName(roleName);
	}

	QByteArray selectRolePermissions = QString("SELECT * from RolePermissions WHERE RoleId = '%1'").arg(qPrintable(roleId)).toUtf8();

	QSqlError error;
	QSqlQuery rolePermissionsQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectRolePermissions, &error);

	imtauth::IRole::FeatureIds *permissionsIds = nullptr;

	while (rolePermissionsQuery.next()){
		QSqlRecord permissionRecord = rolePermissionsQuery.record();
		QByteArray permissionId;

        if (permissionRecord.contains("PermissionId")){
            permissionId = permissionRecord.value("PermissionId").toByteArray();
			permissionsIds->insert(permissionId);
		}
	}
	rolePtr->SetLocalPermissions(*permissionsIds);

	QByteArray selectRoleProhibitions = QString("SELECT * from RoleProhibitions WHERE RoleId = '%1'").arg(qPrintable(roleId)).toUtf8();

	QSqlQuery roleProhibitionsQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectRoleProhibitions, &error);

	imtauth::IRole::FeatureIds *prohibitionsIds = nullptr;

	while (roleProhibitionsQuery.next()){
		QSqlRecord prohibitionRecord = roleProhibitionsQuery.record();
		QByteArray prohibitionId;

        if (prohibitionRecord.contains("ProhibitionId")){
            prohibitionId = prohibitionRecord.value("ProhibitionId").toByteArray();
			prohibitionsIds->insert(prohibitionId);
		}
	}
	rolePtr->SetProhibitions(*prohibitionsIds);

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
	retVal.query = QString("INSERT INTO Roles(Id, Name, Description, Added, LastModified) VALUES('%1', '%2', '%3', '%4', '%5');")
				.arg(qPrintable(roleId))
				.arg(roleName)
				.arg(objectDescription)
				.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
				.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
				.toLocal8Bit();
	retVal.objectName = roleName;

	imtauth::IRole::FeatureIds permissionsIds = rolePtr->GetPermissions();
	imtauth::IRole::FeatureIds prohibitionsIds = rolePtr->GetProhibitions();

	for (const QByteArray& permissionId : permissionsIds){
		retVal.query += "\n" +
                    QString("INSERT INTO RolePermissions(RoleId, PermissionId) VALUES('%1', '%2');")
					.arg(qPrintable(roleId))
					.arg(qPrintable(permissionId)).toLocal8Bit();

	}
	for (const QByteArray& prohibitionId : prohibitionsIds){
		retVal.query += "\n" +
                    QString("INSERT INTO RoleProhibitions(RoleId, ProhibitionId) VALUES('%1', '%2');")
					.arg(qPrintable(roleId))
					.arg(qPrintable(prohibitionId)).toLocal8Bit();

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

		QByteArray retVal = QString("DELETE FROM Roles WHERE Id = '%1';").arg(qPrintable(roleId)).toLocal8Bit();

        retVal += "\n" +
                    QString("DELETE FROM RolePermissions WHERE RoleId = '%1';").arg(qPrintable(roleId)).toLocal8Bit();

        retVal += "\n" +
                    QString("DELETE FROM RoleProhibitions WHERE RoleId = '%1';").arg(qPrintable(roleId)).toLocal8Bit();


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

	QString oldRoleName = oldRolePtr->GetRoleName();
	QString newRoleName = newRolePtr->GetRoleName();

	QByteArray retVal = QString("UPDATE Roles SET Id ='%1', Name = '%2', LastModified = '%3' WHERE Id ='%4';")
			.arg(qPrintable(newRoleId))
			.arg(qPrintable(newRoleName))
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.arg(qPrintable(oldRoleId)).toLocal8Bit();

	imtauth::IRole::FeatureIds newPermissionsIds = newRolePtr->GetPermissions();
	imtauth::IRole::FeatureIds oldPermissionsIds = oldRolePtr->GetPermissions();
	imtauth::IRole::FeatureIds newProhibitionsIds = newRolePtr->GetProhibitions();
	imtauth::IRole::FeatureIds oldProhibitionsIds = oldRolePtr->GetProhibitions();

	for (const QByteArray& permissionId : newPermissionsIds){
		if (!oldPermissionsIds.contains(permissionId)){
			retVal += "\n" +
                        QString("INSERT INTO RolePermissions(RoleId, PermissionId) VALUES('%1', '%2');")
						.arg(qPrintable(newRoleId))
						.arg(qPrintable(permissionId)).toLocal8Bit();
		}
	}

	for (const QByteArray& permissionId : oldPermissionsIds){
		if (!newPermissionsIds.contains(permissionId)){
			retVal += "\n" +
                        QString("DELETE FROM RolePermissions WHERE RoleId = '%1' AND PermissionId = '%2';")
						.arg(qPrintable(newRoleId))
						.arg(qPrintable(permissionId)).toLocal8Bit();
		}
	}

	for (const QByteArray& prohibitionId : newProhibitionsIds){
		if (!oldProhibitionsIds.contains(prohibitionId)){
			retVal += "\n" +
                        QString("INSERT INTO RoleProhibitions(RoleId, ProhibitionId) VALUES('%1', '%2');")
						.arg(qPrintable(newRoleId))
						.arg(qPrintable(prohibitionId)).toLocal8Bit();
		}
	}

	for (const QByteArray& prohibitionId : oldProhibitionsIds){
		if (!newProhibitionsIds.contains(prohibitionId)){
			retVal += "\n" +
                        QString("DELETE FROM RoleProhibitions WHERE RoleId = '%1' AND ProhibitionId = '%2';")
						.arg(qPrintable(newRoleId))
						.arg(qPrintable(prohibitionId)).toLocal8Bit();
		}
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

	QByteArray retVal = QString("UPDATE Roles SET Name = '%1', LastModified = '%2' WHERE Id ='%3';")
			.arg(qPrintable(newObjectName))
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
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


