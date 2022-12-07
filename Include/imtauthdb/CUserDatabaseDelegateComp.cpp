#include <imtauthdb/CUserDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/CUserInfoMetaInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CUserDatabaseDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_userInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtauth::IUserInfo> userPtr = m_userInfoFactCompPtr.CreateInstance();
	if (!userPtr.IsValid()){
		return nullptr;
	}

	QByteArray userId;
	if (record.contains("UserId")){
		userId = record.value("UserId").toByteArray();
		userPtr->SetUserId(userId);
	}

	QString name;
	if (record.contains("Name")){
		name = record.value("Name").toString();
		userPtr->SetName(name);
	}

	QByteArray passwordHash;
	if (record.contains("Password")){
		passwordHash = record.value("Password").toByteArray();
		userPtr->SetPasswordHash(passwordHash);
	}

	QString mail;
	if (record.contains("Email")){
		mail = record.value("Email").toString();
		userPtr->SetMail(mail);
	}

	QByteArray selectUserPermissions = QString("SELECT * from \"UserPermissions\" WHERE UserId = '%1'").arg(qPrintable(userId)).toUtf8();

	QSqlError error;
	QSqlQuery userPermissionsQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectUserPermissions, &error);

	imtauth::IUserInfo::FeatureIds permissionsIds;

	while (userPermissionsQuery.next()){
		QSqlRecord permissionRecord = userPermissionsQuery.record();
		QByteArray permissionId;

		if (permissionRecord.contains("PermissionId")){
			permissionId = permissionRecord.value("PermissionId").toByteArray();

			permissionsIds << permissionId;
		}
	}

	userPtr->SetLocalPermissions(permissionsIds);

	QByteArray selectUserRoles = QString("SELECT * from \"UserRoles\" WHERE UserId = '%1'").arg(qPrintable(userId)).toUtf8();

	QSqlQuery userRolesQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectUserRoles, &error);

	imtauth::IUserInfo::RoleIds rolesIds;

	while (userRolesQuery.next()){
		QSqlRecord roleRecord = userRolesQuery.record();

		QByteArray roleId;
		if (roleRecord.contains("RoleId")){
			roleId = roleRecord.value("RoleId").toByteArray();
		}

		QByteArray productId;
		if (roleRecord.contains("ProductId")){
			productId = roleRecord.value("ProductId").toByteArray();
		}

		rolesIds << roleId + *m_separatorObjectIdAttrPtr + productId;
	}

	userPtr->SetRoles(rolesIds);

	return userPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& /*proposedObjectId*/,
		const QString& objectName,
		const QString& objectDescription,
		const istd::IChangeable* valuePtr) const
{
	const imtauth::IUserInfo* userPtr = dynamic_cast<const imtauth::IUserInfo*>(valuePtr);
	if (userPtr == nullptr){
		return NewObjectQuery();
	}

	QString name = userPtr->GetName();
	if (name.isEmpty()){
		name = objectName;
	}

	QByteArray userId = qPrintable(userPtr->GetUserId());
	QByteArray passwordHash = userPtr->GetPasswordHash();
	QString mail = userPtr->GetMail();

	NewObjectQuery retVal;

	retVal.query += "\n" + QString("INSERT INTO \"Users\" (UserId, Password, Name, Email, Description, Added, LastModified) VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7');")
			.arg(qPrintable(userId))
			.arg(qPrintable(passwordHash))
			.arg(name)
			.arg(mail)
			.arg("")
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.toLocal8Bit();
	retVal.objectName = name;

	imtauth::IUserInfo::FeatureIds permissionsIds = userPtr->GetPermissions();

	for (const QByteArray& permissionId : permissionsIds){
		retVal.query += "\n" +
				QString("INSERT INTO \"UserPermissions\" (UserId, PermissionId) VALUES('%1', '%2');")
				.arg(qPrintable(userId))
				.arg(qPrintable(permissionId)).toLocal8Bit();
	}

	imtauth::IUserInfo::RoleIds rolesIds = userPtr->GetRoles();

	for (const QByteArray& productRoleId : rolesIds){
		QStringList data = QString(productRoleId).split(*m_separatorObjectIdAttrPtr);

		if (data.size() == 2){
			QByteArray roleId = data[0].toUtf8();
			QByteArray productId = data[1].toUtf8();

			retVal.query += "\n" +
					QString("INSERT INTO \"UserRoles\" (UserId, RoleId, ProductId) VALUES('%1', '%2', '%3');")
					.arg(qPrintable(userId))
					.arg(qPrintable(roleId))
					.arg(qPrintable(productId)).toLocal8Bit();
		}
	}

	return retVal;
}


QByteArray CUserDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		const imtauth::IUserInfo* userPtr = dynamic_cast<const imtauth::IUserInfo*>(objectPtr.GetPtr());
		if (userPtr == nullptr){
			return QByteArray();
		}

		if (userPtr->IsAdmin()){
			return QByteArray();
		}

		QByteArray userId = qPrintable(userPtr->GetUserId());
		if (userId.isEmpty()){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM \"Users\" WHERE UserId = '%1';").arg(qPrintable(userId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CUserDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const istd::IChangeable& object) const
{
	const imtauth::IUserInfo* newUserPtr = dynamic_cast<const imtauth::IUserInfo*>(&object);
	if (newUserPtr == nullptr){
		return QByteArray();
	}

	const imtauth::IUserInfo* oldUserPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		oldUserPtr = dynamic_cast<const imtauth::IUserInfo*>(objectPtr.GetPtr());
	}

	if (oldUserPtr == nullptr){
		return QByteArray();
	}

	QByteArray oldUserId = qPrintable(oldUserPtr->GetUserId());
	QByteArray newUserId = qPrintable(newUserPtr->GetUserId());

	QString newName = newUserPtr->GetName();
	QByteArray newUserPasswordHash = newUserPtr->GetPasswordHash();
	QString newMail = newUserPtr->GetMail();

	QByteArray retVal = QString("UPDATE \"Users\" SET UserId ='%1', Password = '%2', Name = '%3', Email = '%4', Description = '%5', LastModified = '%6' WHERE UserId ='%7';")
			.arg(qPrintable(newUserId))
			.arg(qPrintable(newUserPasswordHash))
			.arg(qPrintable(newName))
			.arg(qPrintable(newMail))
			.arg(qPrintable(""))
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.arg(qPrintable(oldUserId)).toLocal8Bit();

	imtauth::IUserInfo::FeatureIds newPermissionsIds = newUserPtr->GetLocalPermissions();
	imtauth::IUserInfo::FeatureIds oldPermissionsIds = oldUserPtr->GetLocalPermissions();

	for (const QByteArray& permissionId : newPermissionsIds){
		if (!oldPermissionsIds.contains(permissionId)){
			retVal += "\n" +
					QString("INSERT INTO \"UserPermissions\" (UserId, PermissionId) VALUES('%1', '%2');")
					.arg(qPrintable(newUserId))
					.arg(qPrintable(permissionId)).toLocal8Bit();
		}
	}

	for (const QByteArray& permissionId : oldPermissionsIds){
		if (!newPermissionsIds.contains(permissionId)){
			retVal += "\n" +
					QString("DELETE FROM \"UserPermissions\" WHERE UserId = '%1' AND PermissionId = '%2';")
					.arg(qPrintable(newUserId))
					.arg(qPrintable(permissionId)).toLocal8Bit();
		}
	}

	imtauth::IUserInfo::FeatureIds newProhibitionsIds = newUserPtr->GetProhibitions();
	imtauth::IUserInfo::FeatureIds oldProhibitionsIds = oldUserPtr->GetProhibitions();

	for (const QByteArray& prohibitionId : newProhibitionsIds){
		if (!oldProhibitionsIds.contains(prohibitionId)){
			retVal += "\n" +
					QString("INSERT INTO \"UserProhibitions\" (UserId, ProhibitionId) VALUES('%1', '%2');")
					.arg(qPrintable(newUserId))
					.arg(qPrintable(prohibitionId)).toLocal8Bit();
		}
	}

	for (const QByteArray& prohibitionId : oldProhibitionsIds){
		if (!newProhibitionsIds.contains(prohibitionId)){
			retVal += "\n" +
					QString("DELETE FROM \"UserProhibitions\" WHERE UserId = '%1' AND ProhibitionId = '%2';")
					.arg(qPrintable(newUserId))
					.arg(qPrintable(prohibitionId)).toLocal8Bit();
		}
	}

	imtauth::IUserInfo::RoleIds newRolesIds = newUserPtr->GetRoles();
	imtauth::IUserInfo::RoleIds oldRolesIds = oldUserPtr->GetRoles();

	for (const QByteArray& productRoleId : newRolesIds){
		if (!oldRolesIds.contains(productRoleId)){
			QStringList data = QString(productRoleId).split(*m_separatorObjectIdAttrPtr);

			if (data.size() == 2){
				QByteArray roleId = data[0].toUtf8();
				QByteArray productId = data[1].toUtf8();

				retVal += "\n" +
						QString("INSERT INTO \"UserRoles\" (UserId, RoleId, ProductId) VALUES('%1', '%2', '%3');")
						.arg(qPrintable(newUserId))
						.arg(qPrintable(roleId))
						.arg(qPrintable(productId)).toLocal8Bit();
			}
		}
	}

	for (const QByteArray& productRoleId : oldRolesIds){
		if (!newRolesIds.contains(productRoleId)){
			QStringList data = QString(productRoleId).split(*m_separatorObjectIdAttrPtr);

			if (data.size() == 2){
				QByteArray roleId = data[0].toUtf8();
				QByteArray productId = data[1].toUtf8();

				retVal += "\n" +
						QString("DELETE FROM \"UserRoles\" WHERE UserId = '%1' AND RoleId = '%2' AND ProductId = '%3';")
						.arg(qPrintable(newUserId))
						.arg(qPrintable(roleId))
						.arg(qPrintable(productId)).toLocal8Bit();
			}
		}
	}

	return retVal;
}


QByteArray CUserDatabaseDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& newObjectName) const
{
	const imtauth::IUserInfo* userPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		userPtr = dynamic_cast<const imtauth::IUserInfo*>(objectPtr.GetPtr());
	}

	if (userPtr == nullptr){
		return QByteArray();
	}

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray userId = qPrintable(userPtr->GetUserId());

	QByteArray retVal = QString("UPDATE \"Users\" SET Name = '%1', LastModified = '%2' WHERE UserId ='%3';")
			.arg(qPrintable(newObjectName))
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.arg(qPrintable(userId)).toLocal8Bit();

	return retVal;
}


QByteArray CUserDatabaseDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& description) const
{
	return QByteArray();
}


// protected methods
// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CUserDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new imod::TModelWrap<imtauth::CUserInfoMetaInfo>);
}


bool CUserDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	const istd::IChangeable* instancePtr = CreateObjectFromRecord(QByteArray(), record);
	if ((instancePtr != nullptr) && m_metaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr retVal;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(instancePtr, "UserInfo", retVal)){
			Q_ASSERT(retVal.IsValid());

			return metaInfo.CopyFrom(*retVal);
		}
	}

	return false;

}


} // namespace imtauthdb


