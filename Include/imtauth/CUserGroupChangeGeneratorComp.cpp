// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserGroupChangeGeneratorComp.h>


// ImtCore includes
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

	// Falling back to the raw ID keeps a deleted role traceable in the history.
	return roleId;
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

	return userId;
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

	return groupId;
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

	InsertChange(documentChangeCollection, "GroupId", QT_TRANSLATE_NOOP("Attribute", "Group-ID"), oldUserGroupInfoPtr->GetId(), newUserGroupInfoPtr->GetId());
	InsertTextChange(documentChangeCollection, "Name", QT_TRANSLATE_NOOP("Attribute", "Name"), oldUserGroupInfoPtr->GetName(), newUserGroupInfoPtr->GetName());
	InsertTextChange(documentChangeCollection, "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldUserGroupInfoPtr->GetDescription(), newUserGroupInfoPtr->GetDescription());

	InsertListChanges(
				documentChangeCollection,
				"AddParentGroup",
				"RemoveParentGroup",
				"ParentGroupId",
				QT_TRANSLATE_NOOP("Attribute", "Parent Group"),
				oldUserGroupInfoPtr->GetParentGroups(),
				newUserGroupInfoPtr->GetParentGroups());

	InsertListChanges(
				documentChangeCollection,
				"AddUser",
				"RemoveUser",
				"UserId",
				QT_TRANSLATE_NOOP("Attribute", "User"),
				oldUserGroupInfoPtr->GetUsers(),
				newUserGroupInfoPtr->GetUsers());

	CompareProductRoles(*oldUserGroupInfoPtr, *newUserGroupInfoPtr, documentChangeCollection);

	return true;
}


void CUserGroupChangeGeneratorComp::CompareProductRoles(
			const imtauth::IUserBaseInfo& oldGroupInfo,
			const imtauth::IUserBaseInfo& newGroupInfo,
			imtbase::CObjectCollection& documentChangeCollection)
{
	const QByteArrayList oldProducts = oldGroupInfo.GetProducts();
	const QByteArrayList newProducts = newGroupInfo.GetProducts();

	InsertListChanges(
				documentChangeCollection,
				"AddProduct",
				"RemoveProduct",
				"ProductId",
				QT_TRANSLATE_NOOP("Attribute", "Product"),
				oldProducts,
				newProducts);

	// Roles are assigned per product, so every product has to be diffed on its own. Products that
	// only exist on one side contribute all of their roles as added resp. removed.
	QByteArrayList allProducts = oldProducts;
	for (const QByteArray& productId : newProducts){
		if (!allProducts.contains(productId)){
			allProducts << productId;
		}
	}

	for (const QByteArray& productId : std::as_const(allProducts)){
		const bool inOld = oldProducts.contains(productId);
		const bool inNew = newProducts.contains(productId);

		InsertListChanges(
					documentChangeCollection,
					"AddRole",
					"RemoveRole",
					"RoleId",
					QT_TRANSLATE_NOOP("Attribute", "Role"),
					inOld ? oldGroupInfo.GetRoles(productId) : QByteArrayList(),
					inNew ? newGroupInfo.GetRoles(productId) : QByteArrayList());

		InsertListChanges(
					documentChangeCollection,
					"AddPermission",
					"RemovePermission",
					"Permission",
					QT_TRANSLATE_NOOP("Attribute", "Permission"),
					inOld ? oldGroupInfo.GetLocalPermissions(productId) : QByteArrayList(),
					inNew ? newGroupInfo.GetLocalPermissions(productId) : QByteArrayList());
	}

	InsertListChanges(
				documentChangeCollection,
				"AddProhibition",
				"RemoveProhibition",
				"Prohibition",
				QT_TRANSLATE_NOOP("Attribute", "Prohibition"),
				oldGroupInfo.GetProhibitions(),
				newGroupInfo.GetProhibitions());
}


QString CUserGroupChangeGeneratorComp::CreateCustomOperationDescription(
			const imtbase::COperationDescription& operationDescription,
			const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("imtauth::CUserGroupChangeGeneratorComp");

	const QByteArray typeId = operationDescription.GetOperationTypeId();
	const QByteArray oldValue = operationDescription.GetOldValue();
	const QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "AddParentGroup"){
		return Translate(QT_TR_NOOP("Added parent group '%1'"), languageId, translationContext).arg(GetGroupName(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveParentGroup"){
		return Translate(QT_TR_NOOP("Removed parent group '%1'"), languageId, translationContext).arg(GetGroupName(oldValue).toHtmlEscaped());
	}

	if (typeId == "AddUser"){
		return Translate(QT_TR_NOOP("Added user '%1'"), languageId, translationContext).arg(GetUserName(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveUser"){
		return Translate(QT_TR_NOOP("Removed user '%1'"), languageId, translationContext).arg(GetUserName(oldValue).toHtmlEscaped());
	}

	if (typeId == "AddRole"){
		return Translate(QT_TR_NOOP("Added role '%1'"), languageId, translationContext).arg(GetRoleName(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveRole"){
		return Translate(QT_TR_NOOP("Removed role '%1'"), languageId, translationContext).arg(GetRoleName(oldValue).toHtmlEscaped());
	}

	if (typeId == "AddProduct"){
		return Translate(QT_TR_NOOP("Added to product '%1'"), languageId, translationContext).arg(QString::fromUtf8(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveProduct"){
		return Translate(QT_TR_NOOP("Removed from product '%1'"), languageId, translationContext).arg(QString::fromUtf8(oldValue).toHtmlEscaped());
	}

	if (typeId == "AddPermission"){
		return Translate(QT_TR_NOOP("Added permission '%1'"), languageId, translationContext).arg(QString::fromUtf8(newValue).toHtmlEscaped());
	}

	if (typeId == "RemovePermission"){
		return Translate(QT_TR_NOOP("Removed permission '%1'"), languageId, translationContext).arg(QString::fromUtf8(oldValue).toHtmlEscaped());
	}

	if (typeId == "AddProhibition"){
		return Translate(QT_TR_NOOP("Added prohibition '%1'"), languageId, translationContext).arg(QString::fromUtf8(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveProhibition"){
		return Translate(QT_TR_NOOP("Removed prohibition '%1'"), languageId, translationContext).arg(QString::fromUtf8(oldValue).toHtmlEscaped());
	}

	return QString();
}


} // namespace imtauth

