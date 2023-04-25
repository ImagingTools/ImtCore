#include <imtauthdb/CUserGroupDatabaseDelegateComp.h>


// ACF includes
#include <istd/CCrcCalculator.h>
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtauth/IUserGroupInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserGroupDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& objectName,
		const QString& objectDescription,
		const istd::IChangeable* valuePtr) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (WriteDataToMemory("DocumentInfo", *workingDocumentPtr, documentContent)){
			const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(workingDocumentPtr.GetPtr());
			Q_ASSERT(groupInfoPtr != nullptr);
			if (groupInfoPtr == nullptr){
				return NewObjectQuery();
			}

			imtauth::IUserGroupInfo::RoleIds groupRoleIds = groupInfoPtr->GetRoles();

			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			imtauth::IUserGroupInfo::UserIds userIds = groupInfoPtr->GetUsers();
			for (const QByteArray& userId : userIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
					imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
					if (userInfoPtr != nullptr){
						if (!userInfoPtr->GetGroups().contains(objectId)){
							userInfoPtr->AddToGroup(objectId);
							retVal.query += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, false);
						}
					}
				}
			}

			int revisionVersion = 1;
			retVal.query += QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\"(\"DocumentId\", \"Document\", \"RevisionNumber\", \"LastModified\", \"Checksum\", \"IsActive\") VALUES('%2', '%3', '%4', '%5', '%6', true);")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectId))
					.arg(SqlEncode(documentContent))
					.arg(revisionVersion)
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(checksum).toLocal8Bit();

			retVal.objectName = objectName;
		}
	}

	return retVal;
}


QByteArray CUserGroupDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		bool useExternDelegate) const
{
	const imtauth::IUserGroupInfo* oldObjectPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		oldObjectPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(objectPtr.GetPtr());
	}

	if (oldObjectPtr == nullptr){
		return QByteArray();
	}

	QByteArray retVal;
	QByteArray documentContent;
	if (WriteDataToMemory("DocumentInfo", object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(&object);
		Q_ASSERT(groupInfoPtr != nullptr);
		if (groupInfoPtr == nullptr){
			return QByteArray();
		}

		if (useExternDelegate){
			imtauth::IUserGroupInfo::RoleIds groupRoleIds = groupInfoPtr->GetRoles();

			imtauth::IUserGroupInfo::UserIds oldUserIds = oldObjectPtr->GetUsers();
			imtauth::IUserGroupInfo::UserIds newUserIds = groupInfoPtr->GetUsers();

			imtauth::IUserGroupInfo::UserIds addedUsers;
			imtauth::IUserGroupInfo::UserIds removedUsers;

			for (const QByteArray& userId : newUserIds){
				if (!oldUserIds.contains(userId)){
					addedUsers.append(userId);
				}
			}

			for (const QByteArray& userId : oldUserIds){
				if (!newUserIds.contains(userId)){
					removedUsers.append(userId);
				}
			}

			for (const QByteArray& userId : addedUsers){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
					imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
					if (userInfoPtr != nullptr){
						if (!userInfoPtr->GetGroups().contains(objectId)){
							userInfoPtr->AddToGroup(objectId);

							retVal += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, false);
						}
					}
				}
			}

			for (const QByteArray& userId : removedUsers){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
					imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
					if (userInfoPtr != nullptr){
						bool result = userInfoPtr->RemoveFromGroup(objectId);
						if (result){
							retVal += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, false);
						}
					}
				}
			}
		}

		retVal += QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', true, (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 );")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(objectId))
				.arg(SqlEncode(documentContent))
				.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
				.arg(checksum).toLocal8Bit();
	}

	return retVal;
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CUserGroupDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return QString("SELECT \"Id\", \"%1\", \"Document\", \"RevisionNumber\", \"LastModified\","
				   "(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND t2.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\" FROM \"%2\""
				   " as t2 WHERE \"IsActive\" = true")
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(*m_tableNameAttrPtr));
}



} // namespace imtauthdb


