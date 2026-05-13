// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CUserGroupDatabaseDelegateComp.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtauth/IUserGroupInfo.h>


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
	NewObjectQuery retVal = BaseClass::CreateNewObjectQuery(typeId, proposedObjectId, objectName, objectDescription, valuePtr, operationContextPtr);

	auto groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(valuePtr);
	if (groupInfoPtr == nullptr){
		return NewObjectQuery();
	}
	
	imtauth::IUserGroupInfo::UserIds userIds = groupInfoPtr->GetUsers();
	for (const QByteArray& userId : userIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
			auto userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userInfoPtr != nullptr){
				if (!userInfoPtr->GetGroups().contains(proposedObjectId)){
					userInfoPtr->AddToGroup(proposedObjectId);
					
					retVal.query += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, operationContextPtr, false);
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
						
						retVal += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, operationContextPtr, false);
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
						retVal += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, operationContextPtr, false);
					}
				}
			}
		}
	}
	
	return retVal;
}


QString CUserGroupDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, "ParentListFilter");
	if (idParamPtr.IsValid()){
		QByteArray targetId = idParamPtr->GetId();
		return QString(R"(
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
					)").arg(QString::fromUtf8(targetId));
					}

	return QString();
}



} // namespace imtauthdb


