#include <imtauthdb/CUserDatabaseDelegateComp.h>


// ACF includes
#include <istd/CCrcCalculator.h>
#include <istd/TOptDelPtr.h>
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& objectName,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* operationContextPtr) const
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

		if (m_userGroupCollectionCompPtr.IsValid() && m_userGroupDatabaseDelegateCompPtr.IsValid()){
			QByteArrayList groupIds = userInfoPtr->GetGroups();
			for (const QByteArray& groupId : groupIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
					imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
					if (userGroupInfoPtr != nullptr){
						userGroupInfoPtr->AddUser(objectId);
						retVal.query += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(
									*m_userGroupCollectionCompPtr,
									groupId,
									*userGroupInfoPtr,
									operationContextPtr,
									false);
					}
				}
			}
		}

		QByteArray documentContent;
		if (WriteDataToMemory("DocumentInfo", *userInfoPtr, documentContent)){
			int revisionVersion = 1;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			retVal.query += QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\"(\"DocumentId\", \"Document\", \"RevisionNumber\", \"LastModified\", \"Checksum\", \"IsActive\") VALUES('%2', '%3', '%4', '%5', '%6', true);")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectId))
					.arg(SqlEncode(documentContent))
					.arg(revisionVersion)
					.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
					.arg(checksum).toUtf8();

			retVal.query += CreateOperationDescriptionQuery(objectId, operationContextPtr);

			retVal.objectName = objectName;
		}
	}

	return retVal;
}


QByteArray CUserDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* operationContextPtr,
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

		if (m_userGroupCollectionCompPtr.IsValid() && m_userGroupDatabaseDelegateCompPtr.IsValid()){
			for (const QByteArray& groupId : addedGroups){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
					imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
					if (userGroupInfoPtr != nullptr){
						if (!userGroupInfoPtr->GetUsers().contains(objectId)){
							userGroupInfoPtr->AddUser(objectId);

							retVal += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(
										*m_userGroupCollectionCompPtr,
										groupId,
										*userGroupInfoPtr,
										operationContextPtr,
										false);
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
							retVal += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(
										*m_userGroupCollectionCompPtr,
										groupId,
										*userGroupInfoPtr,
										operationContextPtr,
										false);
						}
					}
				}
			}
		}
	}

	QDateTime oldDateTime = oldObjectPtr->GetLastConnection();
	QDateTime newDateTime = userInfoPtr->GetLastConnection();

	if (!oldDateTime.isNull() && !newDateTime.isNull() && oldDateTime != newDateTime){
		retVal += QString(R"(UPDATE "Users" SET "Document" = jsonb_set("Document", '{LastConnection}', '"%1"', true) WHERE "DocumentId" ='%2' AND "IsActive" = true;)")
				.arg(userInfoPtr->GetLastConnection().toString(Qt::ISODate))
				.arg(qPrintable(objectId)).toUtf8();
	}
	else{
		QByteArray documentContent;
		if (WriteDataToMemory("DocumentInfo", *userInfoPtr, documentContent)){
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
			retVal += QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', true, (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 );")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectId))
					.arg(SqlEncode(documentContent))
					.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
					.arg(checksum).toUtf8();

			retVal += CreateOperationDescriptionQuery(objectId, operationContextPtr);
		}
	}

	return retVal;
}


QByteArray CUserDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(objectPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		return QByteArray();
	}

	if (userInfoPtr->IsAdmin()){
		return QByteArray();
	}

	QByteArray retVal = QString("DELETE FROM \"%1\" WHERE \"%2\" = '%3';")
			.arg(qPrintable(*m_tableNameAttrPtr))
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(objectId))
			.toUtf8();

	return retVal;
}


bool CUserDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	BaseClass::SetObjectMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

		if (!jsonDocument.isNull()){
			QString userId = jsonDocument["Id"].toString();
			metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_ID, userId);

			QString username = jsonDocument["Name"].toString();
			metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_NAME, username);

			QString description = jsonDocument["Description"].toString();
			metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_DESCRIPTION, description);

			QString mail = jsonDocument["Mail"].toString();
			metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_EMAIL, mail);

			QJsonArray groupsArray = jsonDocument["Groups"].toArray();

			QStringList groups;
			for (const QJsonValue& jsonValue : groupsArray){
				groups << jsonValue.toString();
			}

			metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_GROUPS, groups.join(';'));
		}
	}

	return true;
}



bool CUserDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::TParamsPtr<iprm::ITextParam> systemIdParamPtr(&filterParams, "SystemId");
	if (systemIdParamPtr.IsValid()){
		QString systemId = systemIdParamPtr->GetText();

		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += QString(R"((('%1' = '' AND NOT root."Document" ? 'SystemInfos') OR EXISTS (SELECT 1 FROM jsonb_array_elements(root."Document"->'SystemInfos') as s WHERE s->>'SystemId' = '%1')))").arg(systemId);
	}
	else{
		return BaseClass::CreateObjectFilterQuery(filterParams, filterQuery);
	}

	return true;
}


bool CUserDatabaseDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
{
	QByteArrayList infoIds = collectionFilter.GetSortingInfoIds();
	imtbase::ICollectionFilter::SortingOrder sortingOrder = collectionFilter.GetSortingOrder();
	if (infoIds.contains("SystemName")){
		sortQuery = QString(R"(ORDER BY CASE
					WHEN (NOT root."Document" ? 'SystemInfos') THEN 0
					WHEN root."Document"->'SystemInfos'->0->>'SystemId' = '' THEN 1
					WHEN root."Document"->'SystemInfos'->0->>'SystemId' != '' THEN 2
					ELSE 3 END %1)").arg(sortingOrder == imtbase::ICollectionFilter::SO_ASC ? "ASC" : "DESC");
	}
	else{
		return BaseClass::CreateSortQuery(collectionFilter, sortQuery);
	}

	return true;
}


bool CUserDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	BaseClass::SetCollectionItemMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

		if (!jsonDocument.isNull()){
			QString description = jsonDocument["Description"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, description);

			QString name = jsonDocument["Name"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, name);
		}
	}

	return true;
}


} // namespace imtauthdb


