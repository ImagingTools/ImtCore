// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserGroupChangeGeneratorComp.h>


// ImtCore includes
#include <iqt/iqt.h>
#include <imtauth/CRole.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/CUserGroupInfo.h>


namespace imtauth
{


// protected methods

QString CUserGroupChangeGeneratorComp::GetRoleName(const QByteArray& roleId) const
{
	if (!IsUuid(roleId)){
		return roleId;
	}

	if (m_roleCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_roleCollectionCompPtr->GetObjectData(roleId, dataPtr)){
			const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			if (roleInfoPtr != nullptr){
				return roleInfoPtr->GetRoleName();
			}
		}
	}

	return QString();
}


QString CUserGroupChangeGeneratorComp::GetUserName(const QByteArray& userId) const
{
	if (!IsUuid(userId)){
		return userId;
	}

	if (m_userCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userInfoPtr != nullptr){
				return userInfoPtr->GetName();
			}
		}
	}

	return QString();
}


QString CUserGroupChangeGeneratorComp::GetGroupName(const QByteArray& groupId) const
{
	if (!IsUuid(groupId)){
		return groupId;
	}

	if (m_objectCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(groupId, dataPtr)){
			const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
			if (groupInfoPtr != nullptr){
				return groupInfoPtr->GetName();
			}
		}
	}

	return QString();
}


// reimplemented (imtbase::CDocumentChangeGeneratorCompBase)

bool CUserGroupChangeGeneratorComp::CompareDocuments(
			const istd::IChangeable& oldDocument,
			const istd::IChangeable& newDocument,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage)
{
	const imtauth::CUserGroupInfo* oldUserGroupInfoPtr = dynamic_cast<const imtauth::CUserGroupInfo*>(&oldDocument);
	if (oldUserGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to compare groups. Error: Old group is invalid");
		return false;
	}

	const imtauth::CUserGroupInfo* newUserGroupInfoPtr = dynamic_cast<const imtauth::CUserGroupInfo*>(&newDocument);
	if (newUserGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to compare groups. Error: New group is invalid");
		return false;
	}

	QByteArray oldId = oldUserGroupInfoPtr->GetId();
	QByteArray newId = newUserGroupInfoPtr->GetId();
	if (oldId != newId){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Group-ID");
		InsertOperationDescription(documentChangeCollection, "", "GroupId", keyName, oldId, newId);
	}

	QString oldName = oldUserGroupInfoPtr->GetName();
	QString newName = newUserGroupInfoPtr->GetName();
	if (oldName != newName){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Name");
		InsertOperationDescription(documentChangeCollection, "", "Name", keyName, oldName.toUtf8(), newName.toUtf8());
	}

	QString oldDescription = oldUserGroupInfoPtr->GetDescription();
	QString newDescription = newUserGroupInfoPtr->GetDescription();
	if (oldDescription != newDescription){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Description");
		InsertOperationDescription(documentChangeCollection, "", "Description", keyName, oldDescription.toUtf8(), newDescription.toUtf8());
	}

	QByteArrayList oldParentGroups = oldUserGroupInfoPtr->GetParentGroups();
	QByteArrayList newParentGroups = newUserGroupInfoPtr->GetParentGroups();

	QByteArrayList addedGroups;
	QByteArrayList removedGroups;
	GenerateChanges(oldParentGroups, newParentGroups, addedGroups, removedGroups);

	for (const QByteArray& groupId : addedGroups){
		InsertOperationDescription(documentChangeCollection, "AddParentGroup", "ParentGroup", "ParentGroup", groupId, groupId);
	}

	for (const QByteArray& groupId : removedGroups){
		InsertOperationDescription(documentChangeCollection, "RemoveParentGroup", "ParentGroup", "ParentGroup", groupId, groupId);
	}

	QByteArrayList oldUsers = oldUserGroupInfoPtr->GetUsers();
	QByteArrayList newUsers = newUserGroupInfoPtr->GetUsers();

	QByteArrayList addedUsers;
	QByteArrayList removedUsers;
	GenerateChanges(oldUsers, newUsers, addedUsers, removedUsers);

	for (const QByteArray& userId : addedUsers){
		InsertOperationDescription(documentChangeCollection, "AddUser", "User", "User", userId, userId);
	}

	for (const QByteArray& userId : removedUsers){
		InsertOperationDescription(documentChangeCollection, "RemoveUser", "User", "User", userId, userId);
	}

	QByteArrayList oldProducts = oldUserGroupInfoPtr->GetProducts();
	QByteArrayList newProducts = newUserGroupInfoPtr->GetProducts();

	QByteArrayList addedProducts;
	QByteArrayList removedProducts;
	GenerateChanges(oldProducts, newProducts, addedProducts, removedProducts);

	for (const QByteArray& productId : addedProducts){
		InsertOperationDescription(documentChangeCollection, "AddProduct", "Product", "Product", productId, productId);

		QByteArrayList newRoles = newUserGroupInfoPtr->GetRoles(productId);
		for (const QByteArray& roleId : newRoles){
			InsertOperationDescription(documentChangeCollection, "AddRole", "Role", "Role", roleId, roleId);
		}
	}

	for (const QByteArray& productId : removedProducts){
		InsertOperationDescription(documentChangeCollection, "RemoveProduct", "Product", "Product", productId, productId);

		QByteArrayList oldRoles = oldUserGroupInfoPtr->GetRoles(productId);
		for (const QByteArray& roleId : oldRoles){
			InsertOperationDescription(documentChangeCollection, "RemoveRole", "Role", "Role", roleId, roleId);
		}
	}

	if (oldProducts == newProducts){
		for (const QByteArray& productId : newProducts){
			QByteArrayList newRoles = newUserGroupInfoPtr->GetRoles(productId);
			QByteArrayList oldRoles = oldUserGroupInfoPtr->GetRoles(productId);

			QByteArrayList addedRoles;
			QByteArrayList removedRoles;
			GenerateChanges(oldRoles, newRoles, addedRoles, removedRoles);

			for (const QByteArray& roleId : addedRoles){
				InsertOperationDescription(documentChangeCollection, "AddRole", "Role", "Role", roleId, roleId);
			}

			for (const QByteArray& roleId : removedRoles){
				InsertOperationDescription(documentChangeCollection, "RemoveRole", "Role", "Role", roleId, roleId);
			}
		}
	}

	return true;
}


QString CUserGroupChangeGeneratorComp::CreateCustomOperationDescription(
			const imtbase::COperationDescription& operationDescription,
			const QByteArray& languageId) const
{
	QString retVal;

	QByteArray typeId = operationDescription.GetOperationTypeId();
	QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "AddParentGroup"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added parent group '%1'"), languageId, "imtauth::CUserGroupChangeGeneratorComp");
		change = change.arg(GetGroupName(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveParentGroup"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed parent group '%1'"), languageId, "imtauth::CUserGroupChangeGeneratorComp");
		change = change.arg(GetGroupName(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveUser"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed user '%1'"), languageId, "imtauth::CUserGroupChangeGeneratorComp");
		change = change.arg(GetUserName(newValue));
		retVal += change;
	}
	else if (typeId == "AddUser"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added user '%1'"), languageId, "imtauth::CUserGroupChangeGeneratorComp");
		change = change.arg(GetUserName(newValue));
		retVal += change;
	}
	else if (typeId == "AddRole"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added role '%1'"), languageId, "imtauth::CUserGroupChangeGeneratorComp");
		change = change.arg(GetRoleName(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveRole"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed role '%1'"), languageId, "imtauth::CUserGroupChangeGeneratorComp");
		change = change.arg(GetRoleName(newValue));
		retVal += change;
	}
	else if (typeId == "AddProduct"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added to product '%1'"), languageId, "imtauth::CUserGroupChangeGeneratorComp");
		change = change.arg(qPrintable(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveProduct"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed from product '%1'"), languageId, "imtauth::CUserGroupChangeGeneratorComp");
		change = change.arg(qPrintable(newValue));
		retVal += change;
	}

	return retVal;
}


} // namespace imtauth


