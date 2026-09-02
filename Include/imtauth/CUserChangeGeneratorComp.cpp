// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserChangeGeneratorComp.h>


// ImtCore includes
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

	// Falling back to the raw ID keeps a deleted role traceable in the history.
	return roleId;
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

	return groupId;
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
		errorMessage = QStringLiteral("Unable to compare users. Error: old user is invalid");
		return false;
	}

	const imtauth::CUserInfo* newUserInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(&newDocument);
	if (newUserInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to compare users. Error: New user is invalid");
		return false;
	}

	InsertChange(documentChangeCollection, "UserId", QT_TRANSLATE_NOOP("Attribute", "User-ID"), oldUserInfoPtr->GetId(), newUserInfoPtr->GetId());
	InsertTextChange(documentChangeCollection, "Username", QT_TRANSLATE_NOOP("Attribute", "Username"), oldUserInfoPtr->GetName(), newUserInfoPtr->GetName());
	InsertTextChange(documentChangeCollection, "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldUserInfoPtr->GetDescription(), newUserInfoPtr->GetDescription());
	InsertTextChange(documentChangeCollection, "Mail", QT_TRANSLATE_NOOP("Attribute", "Mail"), oldUserInfoPtr->GetMail(), newUserInfoPtr->GetMail());
	InsertChange(documentChangeCollection, "Sid", QT_TRANSLATE_NOOP("Attribute", "Security-ID"), oldUserInfoPtr->GetSid(), newUserInfoPtr->GetSid());
	InsertFlagChange(documentChangeCollection, "Admin", QT_TRANSLATE_NOOP("Attribute", "Administrator"), oldUserInfoPtr->IsAdmin(), newUserInfoPtr->IsAdmin());

	// The hash itself must never reach the history, only the fact that it was replaced.
	if (oldUserInfoPtr->GetPasswordHash() != newUserInfoPtr->GetPasswordHash()){
		InsertOperationDescription(documentChangeCollection, "ChangePassword", "Password", QT_TRANSLATE_NOOP("Attribute", "Password"));
	}

	InsertListChanges(
				documentChangeCollection,
				"AddGroup",
				"RemoveGroup",
				"GroupId",
				QT_TRANSLATE_NOOP("Attribute", "Group"),
				oldUserInfoPtr->GetGroups(),
				newUserInfoPtr->GetGroups());

	CompareProductRoles(*oldUserInfoPtr, *newUserInfoPtr, documentChangeCollection);

	return true;
}


void CUserChangeGeneratorComp::CompareProductRoles(
			const imtauth::IUserInfo& oldUserInfo,
			const imtauth::IUserInfo& newUserInfo,
			imtbase::CObjectCollection& documentChangeCollection)
{
	const QByteArrayList oldProducts = oldUserInfo.GetProducts();
	const QByteArrayList newProducts = newUserInfo.GetProducts();

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
					inOld ? oldUserInfo.GetRoles(productId) : QByteArrayList(),
					inNew ? newUserInfo.GetRoles(productId) : QByteArrayList());

		// Permissions granted directly to the user, independently of the roles.
		InsertListChanges(
					documentChangeCollection,
					"AddPermission",
					"RemovePermission",
					"Permission",
					QT_TRANSLATE_NOOP("Attribute", "Permission"),
					inOld ? oldUserInfo.GetLocalPermissions(productId) : QByteArrayList(),
					inNew ? newUserInfo.GetLocalPermissions(productId) : QByteArrayList());
	}

	InsertListChanges(
				documentChangeCollection,
				"AddProhibition",
				"RemoveProhibition",
				"Prohibition",
				QT_TRANSLATE_NOOP("Attribute", "Prohibition"),
				oldUserInfo.GetProhibitions(),
				newUserInfo.GetProhibitions());
}


QString CUserChangeGeneratorComp::CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("imtauth::CUserChangeGeneratorComp");

	const QByteArray typeId = operationDescription.GetOperationTypeId();
	const QByteArray oldValue = operationDescription.GetOldValue();
	const QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "AddGroup"){
		return Translate(QT_TR_NOOP("Added to group '%1'"), languageId, translationContext).arg(GetGroupName(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveGroup"){
		return Translate(QT_TR_NOOP("Removed from group '%1'"), languageId, translationContext).arg(GetGroupName(oldValue).toHtmlEscaped());
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

	if (typeId == "ChangePassword"){
		return Translate(QT_TR_NOOP("Password changed"), languageId, translationContext);
	}

	return QString();
}


} // namespace imtauth

