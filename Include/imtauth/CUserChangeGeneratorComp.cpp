// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserChangeGeneratorComp.h>


// ImtCore includes
#include <iqt/iqt.h>
#include <imtauth/CRole.h>
#include <imtauth/CUserInfo.h>


namespace imtauth
{


// protected methods

QString CUserChangeGeneratorComp::GetRoleName(const QByteArray& roleId) const
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


QString CUserChangeGeneratorComp::GetGroupName(const QByteArray& groupId) const
{
	if (!IsUuid(groupId)){
		return groupId;
	}

	if (m_userGroupCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userGroupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
			const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
			if (groupInfoPtr != nullptr){
				return groupInfoPtr->GetName();
			}
		}
	}

	return QString();
}


// reimplemented (imtbase::CDocumentChangeGeneratorCompBase)

bool CUserChangeGeneratorComp::CompareDocuments(
			const istd::IChangeable& oldDocument,
			const istd::IChangeable& newDocument,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage)
{
	const imtauth::CUserInfo* oldUserInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(&oldDocument);
	if (oldUserInfoPtr == nullptr){
		errorMessage = QString("Unable to compare users. Error: old user is invalid");
		return false;
	}

	const imtauth::CUserInfo* newUserInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(&newDocument);
	if (newUserInfoPtr == nullptr){
		errorMessage = QString("Unable to compare users. Error: New user is invalid");
		return false;
	}

	QByteArray oldUserId = oldUserInfoPtr->GetId();
	QByteArray newUserId = newUserInfoPtr->GetId();
	if (oldUserId != newUserId){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "User-ID");
		InsertOperationDescription(documentChangeCollection, "", "UserId", keyName, oldUserId, newUserId);
	}

	QString oldUserName = oldUserInfoPtr->GetName();
	QString newUserName = newUserInfoPtr->GetName();
	if (oldUserName != newUserName){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Username");
		InsertOperationDescription(documentChangeCollection, "", "Username", keyName, oldUserName.toUtf8(), newUserName.toUtf8());
	}

	QString oldDescription = oldUserInfoPtr->GetDescription();
	QString newDescription = newUserInfoPtr->GetDescription();
	if (oldDescription != newDescription){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Description");
		InsertOperationDescription(documentChangeCollection, "", "Description", keyName, oldDescription.toUtf8(), newDescription.toUtf8());
	}

	QString oldMail = oldUserInfoPtr->GetMail();
	QString newMail = newUserInfoPtr->GetMail();
	if (oldMail != newMail){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Mail");
		InsertOperationDescription(documentChangeCollection, "", "Mail", keyName, oldMail.toUtf8(), newMail.toUtf8());
	}

	QByteArrayList oldProducts = oldUserInfoPtr->GetProducts();
	QByteArrayList newProducts = newUserInfoPtr->GetProducts();

	QByteArrayList addedProducts;
	QByteArrayList removedProducts;
	GenerateChanges(oldProducts, newProducts, addedProducts, removedProducts);

	for (const QByteArray& productId : addedProducts){
		InsertOperationDescription(documentChangeCollection, "AddProduct", "Product", "Product", productId, productId);

		QByteArrayList newRoles = newUserInfoPtr->GetRoles(productId);
		for (const QByteArray& roleId : newRoles){
			InsertOperationDescription(documentChangeCollection, "AddRole", "Role", "Role", roleId, roleId);
		}
	}

	for (const QByteArray& productId : removedProducts){
		InsertOperationDescription(documentChangeCollection, "RemoveProduct", "Product", "Product", productId, productId);

		QByteArrayList oldRoles = oldUserInfoPtr->GetRoles(productId);
		for (const QByteArray& roleId : oldRoles){
			InsertOperationDescription(documentChangeCollection, "RemoveRole", "Role", "Role", roleId, roleId);
		}
	}

	if (oldProducts == newProducts){
		for (const QByteArray& productId : newProducts){
			QByteArrayList newRoles = newUserInfoPtr->GetRoles(productId);
			QByteArrayList oldRoles = oldUserInfoPtr->GetRoles(productId);

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

	QByteArrayList oldGroups = oldUserInfoPtr->GetGroups();
	QByteArrayList newGroups = newUserInfoPtr->GetGroups();

	QByteArrayList addedGroups;
	QByteArrayList removedGroups;
	GenerateChanges(oldGroups, newGroups, addedGroups, removedGroups);

	for (const QByteArray& groupId : addedGroups){
		InsertOperationDescription(documentChangeCollection, "AddGroup", "Group", "Group", groupId, groupId);
	}

	for (const QByteArray& groupId : removedGroups){
		InsertOperationDescription(documentChangeCollection, "RemoveGroup", "Group", "Group", groupId, groupId);
	}

	return true;
}


QString CUserChangeGeneratorComp::CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId) const
{
	QString retVal;

	QByteArray typeId = operationDescription.GetOperationTypeId();
	QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "AddGroup"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added to group '%1'"), languageId, "imtauth::CUserChangeGeneratorComp");
		change = change.arg(GetGroupName(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveGroup"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed from group '%1'"), languageId, "imtauth::CUserChangeGeneratorComp");
		change = change.arg(GetGroupName(newValue));
		retVal += change;
	}
	else if (typeId == "AddRole"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added role '%1'"), languageId, "imtauth::CUserChangeGeneratorComp");
		change = change.arg(GetRoleName(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveRole"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed role '%1'"), languageId, "imtauth::CUserChangeGeneratorComp");
		change = change.arg(GetRoleName(newValue));
		retVal += change;
	}
	else if (typeId == "AddProduct"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added to product '%1'"), languageId, "imtauth::CUserChangeGeneratorComp");
		change = change.arg(qPrintable(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveProduct"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed from product '%1'"), languageId, "imtauth::CUserChangeGeneratorComp");
		change = change.arg(qPrintable(newValue));
		retVal += change;
	}
	else if (typeId == "ChangePassword"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Password changed"), languageId, "imtauth::CUserChangeGeneratorComp");
		retVal += change;
	}

	return retVal;
}


} // namespace imtauth


