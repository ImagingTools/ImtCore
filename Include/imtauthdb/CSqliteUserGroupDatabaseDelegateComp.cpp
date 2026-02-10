// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CSqliteUserGroupDatabaseDelegateComp.h>


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

imtdb::IDatabaseObjectDelegate::NewObjectQuery CSqliteUserGroupDatabaseDelegateComp::CreateNewObjectQuery(
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


QByteArray CSqliteUserGroupDatabaseDelegateComp::CreateUpdateObjectQuery(
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
			imtbase::IObjectCollection::DataPtr addedUserDataPtr;
			if (m_userCollectionCompPtr->GetObjectData(userId, addedUserDataPtr)){
				auto userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(addedUserDataPtr.GetPtr());
				if (userInfoPtr != nullptr){
					if (!userInfoPtr->GetGroups().contains(objectId)){
						userInfoPtr->AddToGroup(objectId);

						retVal += m_userDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_userCollectionCompPtr, userId, *userInfoPtr, operationContextPtr, false);
					}
				}
			}
		}

		for (const QByteArray& userId : removedUsers){
			imtbase::IObjectCollection::DataPtr removedUsserDataPtr;
			if (m_userCollectionCompPtr->GetObjectData(userId, removedUsserDataPtr)){
				auto userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(removedUsserDataPtr.GetPtr());
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


} // namespace imtauthdb


