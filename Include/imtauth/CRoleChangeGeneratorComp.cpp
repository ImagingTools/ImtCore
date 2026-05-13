// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleChangeGeneratorComp.h>


// ImtCore includes
#include <iqt/iqt.h>
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

	return QString();
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

	QByteArray oldRoleId = oldRoleInfoPtr->GetRoleId();
	QByteArray newRoleId = newRoleInfoPtr->GetRoleId();
	if (oldRoleId != newRoleId){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Role-ID");
		InsertOperationDescription(documentChangeCollection, "", "RoleId", keyName, oldRoleId, newRoleId);
	}

	QString oldRoleName = oldRoleInfoPtr->GetRoleName();
	QString newRoleName = newRoleInfoPtr->GetRoleName();
	if (oldRoleName != newRoleName){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Name");
		InsertOperationDescription(documentChangeCollection, "", "Name", keyName, oldRoleName.toUtf8(), newRoleName.toUtf8());
	}

	QString oldDescription = oldRoleInfoPtr->GetRoleDescription();
	QString newDescription = newRoleInfoPtr->GetRoleDescription();
	if (oldDescription != newDescription){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Description");
		InsertOperationDescription(documentChangeCollection, "", "Description", keyName, oldDescription.toUtf8(), newDescription.toUtf8());
	}

	QByteArrayList oldParentRoles = oldRoleInfoPtr->GetIncludedRoles();
	QByteArrayList newParentRoles = newRoleInfoPtr->GetIncludedRoles();

	QByteArrayList addedParentRoles;
	QByteArrayList removedParentRoles;
	GenerateChanges(oldParentRoles, newParentRoles, addedParentRoles, removedParentRoles);

	for (const QByteArray& parentRoleId : addedParentRoles){
		QString name = GetRoleName(parentRoleId);
		InsertOperationDescription(documentChangeCollection, "AddParentRole", "ParentRoleId", name, parentRoleId, parentRoleId);
	}

	for (const QByteArray& parentRoleId : removedParentRoles){
		QString name = GetRoleName(parentRoleId);
		InsertOperationDescription(documentChangeCollection, "RemoveParentRole", "ParentRoleId", name, parentRoleId, parentRoleId);
	}

	QByteArrayList oldPermissions = oldRoleInfoPtr->GetPermissions();
	QByteArrayList newPermissions = newRoleInfoPtr->GetPermissions();

	QByteArrayList addedPermissions;
	QByteArrayList removedPermissions;
	GenerateChanges(oldPermissions, newPermissions, addedPermissions, removedPermissions);

	for (const QByteArray& permissionId : addedPermissions){
		InsertOperationDescription(documentChangeCollection, "AddPermission", "Permission", "Permission", permissionId, permissionId);
	}

	for (const QByteArray& permissionId : removedPermissions){
		InsertOperationDescription(documentChangeCollection, "RemovePermission", "Permission", "Permission", permissionId, permissionId);
	}

	return true;
}


QString CRoleChangeGeneratorComp::CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId) const
{
	QString retVal;

	QByteArray typeId = operationDescription.GetOperationTypeId();
	QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "AddParentRole"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added parent role '%1'"), languageId, "imtauth::CRoleChangeGeneratorComp");
		change = change.arg(GetRoleName(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveParentRole"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed parent role '%1'"), languageId, "imtauth::CRoleChangeGeneratorComp");
		change = change.arg(GetRoleName(newValue));
		retVal += change;
	}
	else if (typeId == "AddPermission"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added permission '%1'"), languageId, "imtauth::CRoleChangeGeneratorComp");
		change = change.arg(qPrintable(newValue));
		retVal += change;
	}
	else if (typeId == "RemovePermission"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed permission '%1'"), languageId, "imtauth::CRoleChangeGeneratorComp");
		change = change.arg(qPrintable(newValue));
		retVal += change;
	}

	return retVal;
}


} // namespace imtauth


