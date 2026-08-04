// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleChangeGeneratorComp.h>


// ImtCore includes
#include <imtauth/CRole.h>


namespace imtauth
{


// protected methods

QString CRoleChangeGeneratorComp::GetRoleName(const QByteArray& roleId) const
{
	if (!IsUuid(roleId)){
		return roleId;
	}

	if (m_objectCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(roleId, dataPtr)){
			const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			if (roleInfoPtr != nullptr){
				return roleInfoPtr->GetRoleName();
			}
		}
	}

	return roleId;
}


// reimplemented (imtbase::CDocumentChangeGeneratorCompBase)

bool CRoleChangeGeneratorComp::CompareDocuments(
			const istd::IChangeable& oldDocument,
			const istd::IChangeable& newDocument,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage)
{
	const imtauth::CRole* oldRoleInfoPtr = dynamic_cast<const imtauth::CRole*>(&oldDocument);
	if (oldRoleInfoPtr == nullptr){
		errorMessage = QString("Unable to compare roles. Error: old role is invalid");
		return false;
	}

	const imtauth::CRole* newRoleInfoPtr = dynamic_cast<const imtauth::CRole*>(&newDocument);
	if (newRoleInfoPtr == nullptr){
		errorMessage = QString("Unable to compare roles. Error: New role is invalid");
		return false;
	}

	InsertChange(documentChangeCollection, "RoleId", QT_TRANSLATE_NOOP("Attribute", "Role-ID"), oldRoleInfoPtr->GetRoleId(), newRoleInfoPtr->GetRoleId());
	InsertTextChange(documentChangeCollection, "Name", QT_TRANSLATE_NOOP("Attribute", "Name"), oldRoleInfoPtr->GetRoleName(), newRoleInfoPtr->GetRoleName());
	InsertTextChange(documentChangeCollection, "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldRoleInfoPtr->GetRoleDescription(), newRoleInfoPtr->GetRoleDescription());
	InsertChange(documentChangeCollection, "ProductId", QT_TRANSLATE_NOOP("Attribute", "Product-ID"), oldRoleInfoPtr->GetProductId(), newRoleInfoPtr->GetProductId());
	InsertFlagChange(documentChangeCollection, "Default", QT_TRANSLATE_NOOP("Attribute", "Default Role"), oldRoleInfoPtr->IsDefault(), newRoleInfoPtr->IsDefault());
	InsertFlagChange(documentChangeCollection, "Guest", QT_TRANSLATE_NOOP("Attribute", "Guest Role"), oldRoleInfoPtr->IsGuest(), newRoleInfoPtr->IsGuest());

	InsertListChanges(
				documentChangeCollection,
				"AddParentRole",
				"RemoveParentRole",
				"ParentRoleId",
				QT_TRANSLATE_NOOP("Attribute", "Parent Role"),
				oldRoleInfoPtr->GetIncludedRoles(),
				newRoleInfoPtr->GetIncludedRoles());

	// Only the local permissions are an attribute of this role, the resulting ones also
	// change whenever a parent role is edited and would report changes nobody made here.
	InsertListChanges(
				documentChangeCollection,
				"AddPermission",
				"RemovePermission",
				"Permission",
				QT_TRANSLATE_NOOP("Attribute", "Permission"),
				oldRoleInfoPtr->GetLocalPermissions(),
				newRoleInfoPtr->GetLocalPermissions());

	InsertListChanges(
				documentChangeCollection,
				"AddProhibition",
				"RemoveProhibition",
				"Prohibition",
				QT_TRANSLATE_NOOP("Attribute", "Prohibition"),
				oldRoleInfoPtr->GetProhibitions(),
				newRoleInfoPtr->GetProhibitions());

	return true;
}


QString CRoleChangeGeneratorComp::CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("imtauth::CRoleChangeGeneratorComp");

	const QByteArray typeId = operationDescription.GetOperationTypeId();
	const QByteArray oldValue = operationDescription.GetOldValue();
	const QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "AddParentRole"){
		return Translate(QT_TR_NOOP("Added parent role '%1'"), languageId, translationContext).arg(GetRoleName(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveParentRole"){
		return Translate(QT_TR_NOOP("Removed parent role '%1'"), languageId, translationContext).arg(GetRoleName(oldValue).toHtmlEscaped());
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

