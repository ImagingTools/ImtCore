#include <imtauthdb/CUserDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& typeId,
		const QByteArray& proposedObjectId,
		const QString& objectName,
		const QString& objectDescription,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* operationContextPtr) const
{
	NewObjectQuery retVal = BaseClass::CreateNewObjectQuery(typeId, proposedObjectId, objectName, objectDescription, valuePtr, operationContextPtr);

	const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(valuePtr);
	if (userInfoPtr == nullptr){
		return NewObjectQuery();
	}
	
	if (m_userGroupCollectionCompPtr.IsValid() && m_userGroupDatabaseDelegateCompPtr.IsValid()){
		QByteArrayList groupIds = userInfoPtr->GetGroups();
		for (const QByteArray& groupId : groupIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
				imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
				if (userGroupInfoPtr != nullptr){
					userGroupInfoPtr->AddUser(proposedObjectId);
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

	return retVal;
}


QByteArray CUserDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* operationContextPtr,
		bool useExternDelegate) const
{
	QByteArray retVal = BaseClass::CreateUpdateObjectQuery(collection, objectId, object, operationContextPtr, useExternDelegate);

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
		retVal += QString(R"(UPDATE "Users" SET "Document" = jsonb_set("Document", '{LastConnection}', '"%1"', true) WHERE "DocumentId" ='%2' AND "State" = 'Active';)")
				.arg(userInfoPtr->GetLastConnection().toString(Qt::ISODate), qPrintable(objectId)).toUtf8();
	}

	return retVal;
}


QByteArray CUserDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const imtbase::IOperationContext* operationContextPtr) const
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

	return BaseClass::CreateDeleteObjectQuery(collection, objectId, operationContextPtr);
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


} // namespace imtauthdb


