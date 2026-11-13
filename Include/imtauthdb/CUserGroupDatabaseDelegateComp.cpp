// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CUserGroupDatabaseDelegateComp.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/CUserGroupFilter.h>
#include <imtauthdb/imtauthdb.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserGroupDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	auto groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(valuePtr);
	if (groupInfoPtr == nullptr){
		return NewObjectQuery();
	}

	// The searchable Name column must follow the group itself, whatever name the caller passed.
	const QString groupName = groupInfoPtr->GetName().isEmpty() ? objectName : groupInfoPtr->GetName();
	NewObjectQuery retVal = BaseClass::CreateNewObjectQuery(typeId, proposedObjectId, groupName, objectDescription, valuePtr, operationContextPtr);
	
	imtauth::IUserGroupInfo::UserIds userIds = groupInfoPtr->GetUsers();
	for (const QByteArray& userId : userIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
			auto userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userInfoPtr != nullptr){
				if (!userInfoPtr->GetGroups().contains(proposedObjectId)){
					userInfoPtr->AddToGroup(proposedObjectId);

					imtbase::COperationContext userOperationContext;
					AddReferenceOperation(userOperationContext, operationContextPtr, "AddGroup", "GroupId", QT_TRANSLATE_NOOP("Attribute", "Group"), QByteArray(), proposedObjectId);
					retVal.query += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, &userOperationContext, false);
				}
			}
		}
	}

	return retVal;
}


QByteArray CUserGroupDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool useExternDelegate) const
{
	QByteArray retVal = BaseClass::CreateUpdateObjectQuery(collection, objectId, object, operationContextPtr, useExternDelegate);

	const imtauth::IUserGroupInfo* oldGroupInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (collection.GetObjectData(objectId, dataPtr)){
		oldGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
	}

	if (oldGroupInfoPtr == nullptr){
		return QByteArray();
	}

	auto newGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(&object);
	if (newGroupInfoPtr == nullptr){
		return QByteArray();
	}

	if (!newGroupInfoPtr->GetName().isEmpty()){
		retVal += CreateRenameObjectQuery(collection, objectId, newGroupInfoPtr->GetName(), operationContextPtr);
	}
	
	if (useExternDelegate){
		imtauth::IUserGroupInfo::UserIds oldUserIds = oldGroupInfoPtr->GetUsers();
		imtauth::IUserGroupInfo::UserIds newUserIds = newGroupInfoPtr->GetUsers();
		
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
				auto userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
				if (userInfoPtr != nullptr){
					if (!userInfoPtr->GetGroups().contains(objectId)){
						userInfoPtr->AddToGroup(objectId);

						imtbase::COperationContext userOperationContext;
						AddReferenceOperation(userOperationContext, operationContextPtr, "AddGroup", "GroupId", QT_TRANSLATE_NOOP("Attribute", "Group"), QByteArray(), objectId);
						retVal += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, &userOperationContext, false);
					}
				}
			}
		}
		
		for (const QByteArray& userId : removedUsers){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
				auto userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
				if (userInfoPtr != nullptr){
					bool result = userInfoPtr->RemoveFromGroup(objectId);
					if (result){
						imtbase::COperationContext userOperationContext;
						AddReferenceOperation(userOperationContext, operationContextPtr, "RemoveGroup", "GroupId", QT_TRANSLATE_NOOP("Attribute", "Group"), objectId, QByteArray());
						retVal += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, &userOperationContext, false);
					}
				}
			}
		}
	}
	
	return retVal;
}


QByteArray CUserGroupDatabaseDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray retVal = BaseClass::CreateDeleteObjectsQuery(collection, objectIds, operationContextPtr);
	if (retVal.isEmpty()){
		return retVal;
	}

	// Users and groups must not keep referencing the removed groups.
	const QByteArrayList groupIds(objectIds.cbegin(), objectIds.cend());

	iprm::CParamsSet userFilterParams;
	istd::TUniqueInterfacePtr<imtauth::CUserGroupFilter> groupFilterPtr = new imtauth::CUserGroupFilter();
	groupFilterPtr->SetGroupIds(groupIds);
	userFilterParams.SetEditableParameter(QByteArrayLiteral("GroupFilter"), groupFilterPtr.PopPtr(), true);

	retVal += CreateReferenceRemovalQuery<imtauth::IUserInfo>(
				*m_userCollectionCompPtr,
				*m_userDatabaseDelegateCompPtr,
				userFilterParams,
				imtbase::ICollectionInfo::Ids(),
				[&groupIds](imtauth::IUserInfo& userInfo){
					RemovedReferences removedReferences;
					for (const QByteArray& groupId : groupIds){
						if (userInfo.RemoveFromGroup(groupId)){
							removedReferences << RemovedReference{"RemoveGroup", "GroupId", QT_TRANSLATE_NOOP("Attribute", "Group"), groupId};
						}
					}
					return removedReferences;
				},
				operationContextPtr);

	iprm::CParamsSet groupFilterParams;
	SetFilterIds(groupFilterParams, s_parentFilterId, groupIds);

	retVal += CreateReferenceRemovalQuery<imtauth::IUserGroupInfo>(
				collection,
				*this,
				groupFilterParams,
				objectIds,
				[&groupIds](imtauth::IUserGroupInfo& groupInfo){
					RemovedReferences removedReferences;
					for (const QByteArray& groupId : groupIds){
						if (groupInfo.RemoveParentGroup(groupId)){
							removedReferences << RemovedReference{"RemoveParentGroup", "ParentGroupId", QT_TRANSLATE_NOOP("Attribute", "Parent Group"), groupId};
						}
					}
					return removedReferences;
				},
				operationContextPtr);

	return retVal;
}


QString CUserGroupDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	const bool isSqlite = IsSQLite();

	QStringList conditions;

	const QByteArrayList roleIds = GetFilterIds(filterParams, s_roleFilterId);
	if (!roleIds.isEmpty()){
		conditions << CreateProductRolesContainAnyQuery(isSqlite, roleIds);
	}

	const QByteArrayList parentGroupIds = GetFilterIds(filterParams, s_parentFilterId);
	if (!parentGroupIds.isEmpty()){
		conditions << CreateArrayContainsAnyQuery(isSqlite, QStringLiteral("ParentGroups"), parentGroupIds);
	}

	iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, "ParentListFilter");
	if (idParamPtr.IsValid()){
		QByteArray targetId = idParamPtr->GetId();
		conditions << QStringLiteral(R"(
			NOT EXISTS (
				WITH RECURSIVE descendants AS (
					SELECT g."DocumentId"
					FROM "UserGroups" g
					WHERE g."DocumentId" = '%1'
					AND g."State" = 'Active'
				
					UNION ALL
					SELECT child."DocumentId"
					FROM "UserGroups" child
					JOIN descendants d
						ON coalesce(child."Document"->'ParentGroups', '[]'::jsonb)
							? (d."DocumentId")::text
					WHERE child."State" = 'Active'
				)
				SELECT 1
				FROM descendants d
				WHERE d."DocumentId" = root."DocumentId"
			)
					)").arg(targetId);
	}

	return conditions.join(QStringLiteral(" AND "));
}



} // namespace imtauthdb


