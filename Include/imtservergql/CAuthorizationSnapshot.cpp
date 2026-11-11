// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CAuthorizationSnapshot.h>


// Qt includes
#include <QtCore/QSet>

// ImtCore includes
#include <imtbase/IObjectCollectionCacheController.h>
#include <imtbase/IObjectCollectionReadStatus.h>
#include <imtauth/CollectionSelection.h>
#include <imtauth/CRole.h>
#include <imtauth/CUserBaseInfo.h>
#include <imtauth/IUserGroupInfo.h>


namespace imtservergql
{


static const int s_maxCacheLayers = 8;


// public methods

CAuthorizationSnapshot::CAuthorizationSnapshot(
			const imtbase::IObjectCollection* userCollectionPtr,
			const imtbase::IObjectCollection* roleCollectionPtr,
			const imtbase::IObjectCollection* groupCollectionPtr)
	:m_userCollectionPtr(GetSourceCollection(userCollectionPtr)),
	m_roleCollectionPtr(GetSourceCollection(roleCollectionPtr)),
	m_groupCollectionPtr(GetSourceCollection(groupCollectionPtr))
{
}


CAuthorizationSnapshot::ReadStatus CAuthorizationSnapshot::ReadUser(
			const QByteArray& userId,
			istd::IChangeableUniquePtr& userPtr,
			QString& errorMessage) const
{
	if (m_userCollectionPtr == nullptr){
		errorMessage = QStringLiteral("The users cannot be read: no user collection is configured.");

		return RS_FAILED;
	}

	const ReadStatus status = ReadObject(*m_userCollectionPtr, userId, userPtr);
	if (status != RS_READ){
		if (status == RS_DENIED){
			errorMessage = QStringLiteral("Access to the user account was refused.");
		}
		else if (status == RS_FAILED){
			errorMessage = QStringLiteral("The user account could not be read.");
		}

		return status;
	}

	imtauth::CUserBaseInfo* userBasePtr = dynamic_cast<imtauth::CUserBaseInfo*>(userPtr.GetPtr());
	if (dynamic_cast<imtauth::IUserInfo*>(userPtr.GetPtr()) == nullptr || userBasePtr == nullptr){
		userPtr.Reset();
		errorMessage = QStringLiteral("The user account has an unexpected type.");

		return RS_FAILED;
	}

	userBasePtr->SetRoleProvider(this);
	userBasePtr->SetUserGroupProvider(this);

	return RS_READ;
}


bool CAuthorizationSnapshot::KeepPermissions(imtauth::IUserInfo& userInfo, const QByteArray& productId, QString& errorMessage) const
{
	// The products of the groups count too: a user may hold a product through a group only
	QByteArrayList productIds = userInfo.GetProducts();
	QByteArrayList pendingGroupIds = userInfo.GetGroups();
	QSet<QByteArray> visitedGroupIds;
	while (!pendingGroupIds.isEmpty()){
		const QByteArray groupId = pendingGroupIds.takeFirst();
		if (visitedGroupIds.contains(groupId)){
			continue;
		}
		visitedGroupIds.insert(groupId);

		const imtauth::IUserGroupInfoSharedPtr groupPtr = GetUserGroup(groupId);
		if (groupPtr.IsValid()){
			productIds += groupPtr->GetProducts();
			pendingGroupIds += groupPtr->GetParentGroups();
		}
	}
	productIds << productId;

	QHash<QByteArray, imtauth::IUserBaseInfo::FeatureIds> permissions;
	for (const QByteArray& permissionProductId : std::as_const(productIds)){
		if (!permissionProductId.isEmpty() && !permissions.contains(permissionProductId)){
			permissions.insert(permissionProductId, userInfo.GetPermissions(permissionProductId));
		}
	}
	if (!IsComplete(errorMessage)){
		return false;
	}

	DetachUser(userInfo);
	for (auto it = permissions.cbegin(); it != permissions.cend(); ++it){
		userInfo.SetLocalPermissions(it.key(), it.value());
	}

	return true;
}


bool CAuthorizationSnapshot::IsComplete(QString& errorMessage) const
{
	if (m_errorMessage.isEmpty()){
		return true;
	}

	errorMessage = m_errorMessage;

	return false;
}


void CAuthorizationSnapshot::DetachUser(istd::IChangeable& user)
{
	imtauth::CUserBaseInfo* userBasePtr = dynamic_cast<imtauth::CUserBaseInfo*>(&user);
	if (userBasePtr != nullptr){
		userBasePtr->SetRoleProvider(nullptr);
		userBasePtr->SetUserGroupProvider(nullptr);
	}
}


CAuthorizationSnapshot::ReadStatus CAuthorizationSnapshot::ReadObject(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			istd::IChangeableUniquePtr& objectPtr)
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	bool isAccessDenied = false;
	const imtbase::IObjectCollectionReadStatus* readStatusPtr = dynamic_cast<const imtbase::IObjectCollectionReadStatus*>(&collection);
	const bool isObjectRead = (readStatusPtr != nullptr)
				? readStatusPtr->GetObjectDataWithStatus(objectId, dataPtr, isAccessDenied)
				: collection.GetObjectData(objectId, dataPtr);
	if (isAccessDenied){
		return RS_DENIED;
	}
	if (isObjectRead && dataPtr.IsValid()){
		// A copy of its own: what the source handed out may be shared
		objectPtr = dataPtr->CloneMe();

		return objectPtr.IsValid() ? RS_READ : RS_FAILED;
	}

	// Only what the store reports as absent is not there; a collection may ignore the filter, and the id is compared as text
	bool isRead = false;
	const QByteArrayList ids = imtauth::CollectionSelection::SelectIds(
				collection,
				imtauth::CollectionSelection::ColumnEquals("DocumentId", objectId),
				{QByteArrayLiteral("DocumentId")},
				&isRead);

	return (isRead && !ids.contains(objectId)) ? RS_ABSENT : RS_FAILED;
}


const imtbase::IObjectCollection* CAuthorizationSnapshot::GetSourceCollection(const imtbase::IObjectCollection* collectionPtr)
{
	for (int layer = 0; layer < s_maxCacheLayers; layer++){
		const imtbase::IObjectCollectionCacheController* cachePtr = dynamic_cast<const imtbase::IObjectCollectionCacheController*>(collectionPtr);
		if (cachePtr == nullptr){
			return collectionPtr;
		}

		collectionPtr = cachePtr->GetSourceCollection();
	}

	return nullptr;
}


// reimplemented (imtauth::IRoleInfoProvider)

const imtbase::ICollectionInfo& CAuthorizationSnapshot::GetRoleList() const
{
	return m_emptyList;
}


imtauth::IRoleUniquePtr CAuthorizationSnapshot::GetRole(const QByteArray& objectId, const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtbase::IObjectCollection::DataPtr dataPtr = ReadOnce(OK_ROLE, objectId);
	if (!dataPtr.IsValid()){
		return imtauth::IRoleUniquePtr();
	}

	// A copy takes the provider of its component; its parents are read here too
	imtauth::IRoleUniquePtr retVal;
	retVal.MoveCastedPtr(dataPtr->CloneMe());
	imtauth::CRole* rolePtr = dynamic_cast<imtauth::CRole*>(retVal.GetPtr());
	if (rolePtr == nullptr){
		return imtauth::IRoleUniquePtr();
	}
	rolePtr->SetParentRoleProvider(this);

	return retVal;
}


// reimplemented (imtauth::IUserGroupInfoProvider)

const imtbase::ICollectionInfo& CAuthorizationSnapshot::GetUserGroupList() const
{
	return m_emptyList;
}


imtauth::IUserGroupInfoSharedPtr CAuthorizationSnapshot::GetUserGroup(const QByteArray& groupId, const iprm::IParamsSet* /*paramsPtr*/) const
{
	imtauth::IUserGroupInfoSharedPtr retVal;
	imtbase::IObjectCollection::DataPtr dataPtr = ReadOnce(OK_GROUP, groupId);
	if (dataPtr.IsValid()){
		retVal.SetCastedPtr(dataPtr);
	}

	return retVal;
}


// private methods

imtbase::IObjectCollection::DataPtr CAuthorizationSnapshot::ReadOnce(ObjectKind kind, const QByteArray& objectId) const
{
	const bool isRole = (kind == OK_ROLE);
	const imtbase::IObjectCollection* collectionPtr = isRole ? m_roleCollectionPtr : m_groupCollectionPtr;
	const QString kindName = isRole ? QStringLiteral("role") : QStringLiteral("group");
	QHash<QByteArray, imtbase::IObjectCollection::DataPtr>& objects = isRole ? m_roles : m_groups;
	if (objects.contains(objectId)){
		return objects.value(objectId);
	}

	// Every outcome is kept: a failure stays one, and the object is read once
	objects.insert(objectId, imtbase::IObjectCollection::DataPtr());

	QString errorMessage;
	istd::IChangeableUniquePtr objectPtr;
	const ReadStatus status = (collectionPtr != nullptr) ? ReadObject(*collectionPtr, objectId, objectPtr) : RS_FAILED;
	if (status == RS_ABSENT){
		return imtbase::IObjectCollection::DataPtr();
	}

	if (collectionPtr == nullptr){
		errorMessage = QStringLiteral("The %1 '%2' cannot be read: no collection is configured.").arg(kindName, QString::fromUtf8(objectId));
	}
	else if (status != RS_READ){
		errorMessage = QStringLiteral("The %1 '%2' could not be read.").arg(kindName, QString::fromUtf8(objectId));
	}
	else{
		// What it refers to is read here too
		imtauth::CRole* rolePtr = dynamic_cast<imtauth::CRole*>(objectPtr.GetPtr());
		imtauth::CUserBaseInfo* groupPtr = dynamic_cast<imtauth::CUserBaseInfo*>(objectPtr.GetPtr());
		const bool isExpectedType = isRole
					? (rolePtr != nullptr)
					: (groupPtr != nullptr && dynamic_cast<imtauth::IUserGroupInfo*>(objectPtr.GetPtr()) != nullptr);
		if (!isExpectedType){
			errorMessage = QStringLiteral("The %1 '%2' has an unexpected type.").arg(kindName, QString::fromUtf8(objectId));
		}
		else if (rolePtr != nullptr){
			rolePtr->SetParentRoleProvider(this);
		}
		else{
			groupPtr->SetRoleProvider(this);
			groupPtr->SetUserGroupProvider(this);
		}
	}

	if (!errorMessage.isEmpty()){
		if (m_errorMessage.isEmpty()){
			m_errorMessage = errorMessage;
		}

		return imtbase::IObjectCollection::DataPtr();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	dataPtr.FromUnique(std::move(objectPtr));
	objects.insert(objectId, dataPtr);

	return dataPtr;
}


} // namespace imtservergql
