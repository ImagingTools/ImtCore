#include <imtauthdb/CUserDatabaseDelegateComp.h>


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
		userPtr->SetId(userId);
	}

	QString username;
	if (record.contains("Username")){
		username = record.value("Username").toString();
		userPtr->SetUsername(username);
	}

	QString name;
	if (record.contains("Name")){
		name = record.value("Name").toString();
		userPtr->SetName(name);
	}

	QByteArray passwordHash;
	if (record.contains("PasswordHash")){
		passwordHash = record.value("Id").toByteArray();
		userPtr->SetPasswordHash(passwordHash);
	}

	QString mail;
	if (record.contains("Mail")){
		mail = record.value("Mail").toString();
		userPtr->SetMail(mail);
	}

    QByteArray selectUserPermissions = QString("SELECT * from UserPermissions WHERE UserId = '%1'").arg(qPrintable(userId)).toUtf8();

    QSqlError error;
    QSqlQuery userPermissionsQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectUserPermissions, &error);

    imtauth::IUserInfo::FeatureIds *permissionsIds = nullptr;

    while (userPermissionsQuery.next()){
        QSqlRecord permissionRecord = userPermissionsQuery.record();
        QByteArray permissionId;

        if (permissionRecord.contains("PermissionId")){
            permissionId = permissionRecord.value("PermissionId").toByteArray();
            permissionsIds->insert(permissionId);
        }
    }
    userPtr->SetLocalPermissions(*permissionsIds);

    QByteArray selectUserProhibitions = QString("SELECT * from UserProhibitions WHERE UserId = '%1'").arg(qPrintable(userId)).toUtf8();

    QSqlQuery userProhibitionsQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectUserProhibitions, &error);

    imtauth::IUserInfo::FeatureIds *prohibitionsIds = nullptr;

    while (userProhibitionsQuery.next()){
        QSqlRecord prohibitionRecord = userProhibitionsQuery.record();
        QByteArray prohibitionId;

        if (prohibitionRecord.contains("ProhibitionId")){
            prohibitionId = prohibitionRecord.value("ProhibitionId").toByteArray();
            prohibitionsIds->insert(prohibitionId);
        }
    }
    userPtr->SetProhibitions(*prohibitionsIds);

    QByteArray selectUserRoles = QString("SELECT * from UserRoles WHERE UserId = '%1'").arg(qPrintable(userId)).toUtf8();

    QSqlQuery userRolesQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectUserRoles, &error);

    imtauth::IUserInfo::RoleIds *rolesIds = nullptr;

    while (userRolesQuery.next()){
        QSqlRecord roleRecord = userRolesQuery.record();
        QByteArray roleId;

        if (roleRecord.contains("RoleId")){
            roleId = roleRecord.value("RoleId").toByteArray();
            rolesIds->insert(roleId);
        }
    }
    userPtr->SetRoles(*rolesIds);

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

    QByteArray userId = userPtr->GetId();
    if (userId.isEmpty()){
        return NewObjectQuery();
    }

    QString name = userPtr->GetName();
    if (name.isEmpty()){
        name = objectName;
    }

    QString username = userPtr->GetUsername();
    QByteArray passwordHash = userPtr->GetPasswordHash();
    QString mail = userPtr->GetMail();

    NewObjectQuery retVal;

    retVal.query += "\n" + QString("INSERT INTO Users(Id, Name, Username, PasswordHash, Mail, Added, LastModified) VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7');")
                .arg(qPrintable(userId))
                .arg(name)
                .arg(username)
                .arg(qPrintable(passwordHash))
                .arg(mail)
                .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
                .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
                .toLocal8Bit();
    retVal.objectName = name;

    imtauth::IUserInfo::FeatureIds permissionsIds = userPtr->GetPermissions();
    imtauth::IUserInfo::FeatureIds prohibitionsIds = userPtr->GetProhibitions();
    imtauth::IUserInfo::RoleIds rolesIds = userPtr->GetRoles();

    for (const QByteArray& permissionId : permissionsIds){
        retVal.query += "\n" +
                    QString("INSERT INTO UserPermissions(UserId, PermissionId) VALUES('%1', '%2');")
                    .arg(qPrintable(userId))
                    .arg(qPrintable(permissionId)).toLocal8Bit();

    }
    for (const QByteArray& prohibitionId : prohibitionsIds){
        retVal.query += "\n" +
                    QString("INSERT INTO UserProhibitions(UserId, ProhibitionId) VALUES('%1', '%2');")
                    .arg(qPrintable(userId))
                    .arg(qPrintable(prohibitionId)).toLocal8Bit();

    }
    for (const QByteArray& roleId : rolesIds){
        retVal.query += "\n" +
                    QString("INSERT INTO UserRoles(UserId, RoleId) VALUES('%1', '%2');")
                    .arg(qPrintable(userId))
                    .arg(qPrintable(roleId)).toLocal8Bit();

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

        QByteArray userId = userPtr->GetId();
        if (userId.isEmpty()){
            return QByteArray();
        }

        QByteArray retVal = QString("DELETE FROM Users WHERE Id = '%1';").arg(qPrintable(userId)).toLocal8Bit();

        retVal += "\n" +
                    QString("DELETE FROM UserPermissions WHERE UserId = '%1';").arg(qPrintable(userId)).toLocal8Bit();

        retVal += "\n" +
                    QString("DELETE FROM UserProhibitions WHERE UserId = '%1';").arg(qPrintable(userId)).toLocal8Bit();

        retVal += "\n" +
                    QString("DELETE FROM UserRoles WHERE UserId = '%1';").arg(qPrintable(userId)).toLocal8Bit();

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

    QByteArray oldUserId = oldUserPtr->GetId();
    QByteArray newUserId = newUserPtr->GetId();

    QString oldUsername = oldUserPtr->GetUsername();
    QString newUsername = newUserPtr->GetUsername();

    QString oldName = oldUserPtr->GetName();
    QString newName = newUserPtr->GetName();

    QByteArray oldUserPasswordHash = oldUserPtr->GetPasswordHash();
    QByteArray newUserPasswordHash = newUserPtr->GetPasswordHash();

    QString oldMail = oldUserPtr->GetMail();
    QString newMail = newUserPtr->GetMail();

    QByteArray retVal = QString("UPDATE Users SET Id ='%1', Username = '%2', Name = '%3', PasswordHash = '%4', Mail = '%5', LastModified = '%6' WHERE Id ='%7';")
            .arg(qPrintable(newUserId))
            .arg(qPrintable(newUsername))
            .arg(qPrintable(newName))
            .arg(qPrintable(newUserPasswordHash))
            .arg(qPrintable(newMail))
            .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
            .arg(qPrintable(oldUserId)).toLocal8Bit();

    imtauth::IUserInfo::FeatureIds newPermissionsIds = newUserPtr->GetPermissions();
    imtauth::IUserInfo::FeatureIds oldPermissionsIds = oldUserPtr->GetPermissions();
    imtauth::IUserInfo::FeatureIds newProhibitionsIds = newUserPtr->GetProhibitions();
    imtauth::IUserInfo::FeatureIds oldProhibitionsIds = oldUserPtr->GetProhibitions();

    for (const QByteArray& permissionId : newPermissionsIds){
        if (!oldPermissionsIds.contains(permissionId)){
            retVal += "\n" +
                        QString("INSERT INTO UserPermissions(UserId, PermissionId) VALUES('%1', '%2');")
                        .arg(qPrintable(newUserId))
                        .arg(qPrintable(permissionId)).toLocal8Bit();
        }
    }

    for (const QByteArray& permissionId : oldPermissionsIds){
        if (!newPermissionsIds.contains(permissionId)){
            retVal += "\n" +
                        QString("DELETE FROM UserPermissions WHERE UserId = '%1' AND PermissionId = '%2';")
                        .arg(qPrintable(newUserId))
                        .arg(qPrintable(permissionId)).toLocal8Bit();
        }
    }

    for (const QByteArray& prohibitionId : newProhibitionsIds){
        if (!oldProhibitionsIds.contains(prohibitionId)){
            retVal += "\n" +
                        QString("INSERT INTO UserProhibitions(UserId, ProhibitionId) VALUES('%1', '%2');")
                        .arg(qPrintable(newUserId))
                        .arg(qPrintable(prohibitionId)).toLocal8Bit();
        }
    }

    for (const QByteArray& prohibitionId : oldProhibitionsIds){
        if (!newProhibitionsIds.contains(prohibitionId)){
            retVal += "\n" +
                        QString("DELETE FROM UserProhibitions WHERE UserId = '%1' AND ProhibitionId = '%2';")
                        .arg(qPrintable(newUserId))
                        .arg(qPrintable(prohibitionId)).toLocal8Bit();
        }
    }

    imtauth::IUserInfo::RoleIds newRolesIds = newUserPtr->GetRoles();
    imtauth::IUserInfo::RoleIds oldRolesIds = oldUserPtr->GetRoles();

    for (const QByteArray& roleId : newRolesIds){
        if (!oldRolesIds.contains(roleId)){
            retVal += "\n" +
                        QString("INSERT INTO UserRoles(UserId, RoleId) VALUES('%1', '%2');")
                        .arg(qPrintable(newUserId))
                        .arg(qPrintable(roleId)).toLocal8Bit();
        }
    }

    for (const QByteArray& roleId : oldRolesIds){
        if (!newRolesIds.contains(roleId)){
            retVal += "\n" +
                        QString("DELETE FROM UserRoles WHERE UserId = '%1' AND RoleId = '%2';")
                        .arg(qPrintable(newUserId))
                        .arg(qPrintable(roleId)).toLocal8Bit();
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

    QByteArray userId = userPtr->GetId();

    QByteArray retVal = QString("UPDATE Users SET Name = '%1', LastModified = '%2' WHERE Id ='%3';")
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
	return idoc::MetaInfoPtr();
}


bool CUserDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& /*record*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return true;
}


} // namespace imtauthdb


