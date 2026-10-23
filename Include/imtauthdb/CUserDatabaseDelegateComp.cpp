// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CUserDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilterHelper.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/IUserGroupFilter.h>
#include <imtauthdb/imtauthdb.h>


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
	auto userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(valuePtr);
	if (userInfoPtr == nullptr){
		return NewObjectQuery();
	}

	// The searchable Name column must follow the user itself, whatever name the caller passed.
	const QString userName = userInfoPtr->GetName().isEmpty() ? objectName : userInfoPtr->GetName();
	NewObjectQuery retVal = BaseClass::CreateNewObjectQuery(typeId, proposedObjectId, userName, objectDescription, valuePtr, operationContextPtr);

	if (m_userGroupCollectionCompPtr.IsValid() && m_userGroupDatabaseDelegateCompPtr.IsValid()){
		QByteArrayList groupIds = userInfoPtr->GetGroups();
		for (const QByteArray& groupId : groupIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
				auto userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
				if (userGroupInfoPtr != nullptr){
					userGroupInfoPtr->AddUser(proposedObjectId);

					imtbase::COperationContext groupOperationContext;
					AddReferenceOperation(groupOperationContext, operationContextPtr, "AddUser", "UserId", QT_TRANSLATE_NOOP("Attribute", "User"), QByteArray(), proposedObjectId);
					retVal.query += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(
						*m_userGroupCollectionCompPtr,
						groupId,
						*userGroupInfoPtr,
						&groupOperationContext,
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

	auto userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(&const_cast<istd::IChangeable&>(object));
	Q_ASSERT(userInfoPtr != nullptr);
	if (userInfoPtr == nullptr){
		return QByteArray();
	}

	const QString newName = userInfoPtr->GetName();
	if (!newName.isEmpty() && newName != oldObjectPtr->GetName()){
		retVal += CreateRenameObjectQuery(collection, objectId, newName, operationContextPtr);
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

							imtbase::COperationContext groupOperationContext;
							AddReferenceOperation(groupOperationContext, operationContextPtr, "AddUser", "UserId", QT_TRANSLATE_NOOP("Attribute", "User"), QByteArray(), objectId);
							retVal += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(
										*m_userGroupCollectionCompPtr,
										groupId,
										*userGroupInfoPtr,
										&groupOperationContext,
										false);
						}
					}
				}
			}

			for (const QByteArray& groupId : removedGroups){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
					auto userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
					if (userGroupInfoPtr != nullptr){
						bool result = userGroupInfoPtr->RemoveUser(objectId);
						if (result){
							imtbase::COperationContext groupOperationContext;
							AddReferenceOperation(groupOperationContext, operationContextPtr, "RemoveUser", "UserId", QT_TRANSLATE_NOOP("Attribute", "User"), objectId, QByteArray());
							retVal += m_userGroupDatabaseDelegateCompPtr->CreateUpdateObjectQuery(
										*m_userGroupCollectionCompPtr,
										groupId,
										*userGroupInfoPtr,
										&groupOperationContext,
										false);
						}
					}
				}
			}
		}
	}

	return retVal;
}


QByteArray CUserDatabaseDelegateComp::CreateDeleteObjectsQuery(
		const imtbase::IObjectCollection& collection,
		const imtbase::ICollectionInfo::Ids& objectIds,
		const imtbase::IOperationContext* operationContextPtr) const
{
	for (const imtbase::ICollectionInfo::Id& objectId : objectIds){
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
	}

	return BaseClass::CreateDeleteObjectsQuery(collection, objectIds, operationContextPtr);
}


bool CUserDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::TParamsPtr<iprm::ITextParam> systemIdParamPtr(&filterParams, "SystemId");
	if (systemIdParamPtr.IsValid()){
		QString systemId = systemIdParamPtr->GetText();

		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += QStringLiteral(R"((('%1' = '' AND NOT root."Document" ? 'SystemInfos') OR EXISTS (SELECT 1 FROM jsonb_array_elements(root."Document"->'SystemInfos') as s WHERE s->>'SystemId' = '%1')))").arg(SqlEncode(systemId));
	}
	else{
		return BaseClass::CreateObjectFilterQuery(filterParams, filterQuery);
	}

	return true;
}


QString CUserDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	const bool isSqlite = IsSQLite();

	QStringList conditions;

	iprm::TParamsPtr<imtauth::IUserGroupFilter> groupFilterParamPtr(&filterParams, QByteArrayLiteral("GroupFilter"));
	if (groupFilterParamPtr.IsValid()){
		QByteArrayList groupIds = groupFilterParamPtr->GetGroupIds();
		if (groupIds.isEmpty()){
			// An empty group list must not widen the selection to the whole collection, so it falls
			// back to the filter's own user, like CUserActionDatabaseDelegateComp does. An empty user
			// id then matches nothing.
			const QString userId = SqlEncode(QString::fromUtf8(groupFilterParamPtr->GetUserId()));

			conditions << (isSqlite
						? QStringLiteral(R"(json_extract(root."Document", '$.Id') = '%1')").arg(userId)
						: QStringLiteral(R"(root."Document"->>'Id' = '%1')").arg(userId));
		}
		else{
			conditions << CreateArrayContainsAnyQuery(isSqlite, QStringLiteral("Groups"), groupIds);
		}
	}

	const QByteArrayList roleIds = GetFilterIds(filterParams, s_roleFilterId);
	if (!roleIds.isEmpty()){
		conditions << CreateProductRolesContainAnyQuery(isSqlite, roleIds);
	}

	return conditions.join(QStringLiteral(" AND "));
}


bool CUserDatabaseDelegateComp::CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const
{
	QByteArrayList infoIds = imtbase::CComplexCollectionFilterHelper::GetSortingFieldIds(collectionFilter).values();
	imtbase::IComplexCollectionFilter::SortingOrder sortingOrder = imtbase::CComplexCollectionFilterHelper::GetSortingOrder(collectionFilter, infoIds);
	if (infoIds.contains("LastConnection")){
		sortQuery = QStringLiteral(R"(ORDER BY "LastConnection" %1)").arg(sortingOrder == imtbase::IComplexCollectionFilter::SO_ASC ? "ASC" : "DESC");
	}
	else{
		return BaseClass::CreateSortQuery(collectionFilter, sortQuery);
	}

	return true;
}


QByteArray CUserDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	if (IsSQLite()){
		return QByteArray();
	}

	return QByteArray(R"(
			LEFT JOIN "UserConnections" AS conn
				ON conn."DocumentId" = root."DocumentId"
				AND conn."State" = 'Active'
	)");
}


QByteArray CUserDatabaseDelegateComp::GetCustomColumnsQuery() const
{
	if (IsSQLite()){
		return QByteArray();
	}

	return QByteArray(R"(
			conn."Document"->>'LastConnection' as "LastConnection"
	)");
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateCompBase)

QString CUserDatabaseDelegateComp::CreateTenantBindingFilterQuery(const QByteArray& tenantId, imtauth::TenantFilterMode filterMode) const
{
	// Users belong to no organization; in an organization only its active members are visible
	if (tenantId.isEmpty()){
		return QString();
	}

	const QString membershipsTableName = (m_tableSchemaAttrPtr.IsValid() && !(*m_tableSchemaAttrPtr).isEmpty())
				? QStringLiteral(R"(%1."TenantMemberships")").arg(QString::fromUtf8(*m_tableSchemaAttrPtr))
				: QStringLiteral(R"("TenantMemberships")");

	const QString membershipLookup = IsSQLite()
				? QStringLiteral(R"(SELECT 1 FROM %1 memberships WHERE memberships."UserId" = %2 AND memberships."TenantId" = '%3' AND memberships."IsActive" = 1)")
				: QStringLiteral(R"(SELECT 1 FROM %1 memberships WHERE memberships."UserId"::text = %2 AND memberships."TenantId"::text = '%3' AND memberships."IsActive")");

	const QString query = membershipLookup.arg(membershipsTableName, GetDocumentIdTextExpression(), SqlEncode(QString::fromUtf8(tenantId)));

	return filterMode == imtauth::TFM_EXCLUDE
				? QStringLiteral("NOT EXISTS (%1)").arg(query)
				: QStringLiteral("EXISTS (%1)").arg(query);
}


} // namespace imtauthdb


