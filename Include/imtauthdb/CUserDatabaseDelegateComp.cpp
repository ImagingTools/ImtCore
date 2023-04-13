#include <imtauthdb/CUserDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <istd/CCrcCalculator.h>
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CUserInfoMetaInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CUserDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	int index = m_documentFactoriesCompPtr.FindValue("UserInfo");
	if (index >= 0){
		istd::TDelPtr<istd::IChangeable> documentPtr;
		istd::IChangeable* userInstancePtr = m_documentFactoriesCompPtr.CreateInstance(index);
		documentPtr.SetPtr(dynamic_cast<imtauth::CIdentifiableUserInfo*>(userInstancePtr));

		if (!documentPtr.IsValid()){
			return nullptr;
		}

		if (record.contains(*m_documentContentColumnIdAttrPtr)){
			QByteArray documentContent = record.value(qPrintable(*m_documentContentColumnIdAttrPtr)).toByteArray();

			if (ReadDataFromMemory("UserInfo", documentContent, *documentPtr)){
				return documentPtr.PopPtr();
			}
		}
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& objectName,
		const QString& objectDescription,
		const istd::IChangeable* valuePtr) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<istd::IChangeable> workingDocumentPtr;
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(const_cast<istd::IChangeable*>(valuePtr), false);
	}

	if (workingDocumentPtr.IsValid()){
		imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(workingDocumentPtr.GetPtr());
		Q_ASSERT(userInfoPtr != nullptr);
		if (userInfoPtr == nullptr){
			return NewObjectQuery();
		}

		QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;

		QByteArrayList groupIds = userInfoPtr->GetGroups();
		for (const QByteArray& groupId : groupIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
				imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
				if (userGroupInfoPtr != nullptr){
					userGroupInfoPtr->AddUser(objectId);

					for (const QByteArray& roleId : userGroupInfoPtr->GetRoles()){
						userInfoPtr->AddRole(roleId);
					}

					retVal.query += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userGroupCollectionCompPtr, groupId, *userGroupInfoPtr, false);
				}
			}
		}

		QByteArray documentContent;
		if (WriteDataToMemory("UserInfo", *userInfoPtr, documentContent)){
			int revisionVersion = 1;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

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


QByteArray CUserDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		bool useExternDelegate) const
{
	const imtauth::IUserInfo* oldObjectPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		oldObjectPtr = dynamic_cast<const imtauth::IUserInfo*>(objectPtr.GetPtr());
	}

	if (oldObjectPtr == nullptr){
		return QByteArray();
	}

	imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(&const_cast<istd::IChangeable&>(object));
	Q_ASSERT(userInfoPtr != nullptr);
	if (userInfoPtr == nullptr){
		return QByteArray();
	}

	QByteArray retVal;

	if (useExternDelegate){
		QByteArrayList oldGroupIds = oldObjectPtr->GetGroups();
		QByteArrayList newGroupIds = userInfoPtr->GetGroups();

		QByteArrayList addedGroups;
		QByteArrayList removedGroups;

		for (const QByteArray& groupId : newGroupIds){
			if (!oldGroupIds.contains(groupId)){
				addedGroups.append(groupId);
			}
		}

		for (const QByteArray& groupId : oldGroupIds){
			if (!newGroupIds.contains(groupId)){
				removedGroups.append(groupId);
			}
		}

		for (const QByteArray& groupId : addedGroups){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
				imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
				if (userGroupInfoPtr != nullptr){
					if (!userGroupInfoPtr->GetUsers().contains(objectId)){
						userGroupInfoPtr->AddUser(objectId);

						for (const QByteArray& roleId : userGroupInfoPtr->GetRoles()){
							userInfoPtr->AddRole(roleId);
						}

						retVal += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userGroupCollectionCompPtr, groupId, *userGroupInfoPtr, false);
					}
				}
			}
		}

		for (const QByteArray& groupId : removedGroups){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
				imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
				if (userGroupInfoPtr != nullptr){
					bool result = userGroupInfoPtr->RemoveUser(objectId);
					if (result){
						for (const QByteArray& roleId : userGroupInfoPtr->GetRoles()){
							userInfoPtr->RemoveRole(roleId);
						}

						retVal += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userGroupCollectionCompPtr, groupId, *userGroupInfoPtr, false);
					}
				}
			}
		}
	}

	QByteArray documentContent;
	if (WriteDataToMemory("UserInfo", *userInfoPtr, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		retVal += QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', true, (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 );")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(objectId))
				.arg(SqlEncode(documentContent))
				.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
				.arg(checksum).toLocal8Bit();
	}

	return retVal;
}


} // namespace imtauthdb


