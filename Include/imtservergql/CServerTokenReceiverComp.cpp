// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CServerTokenReceiverComp.h>


// Qt includes
#include <QtCore/QSet>


namespace imtservergql
{


namespace
{


// Commands of the generic collection protocol: only they are resolved by the collection they name
const QSet<QByteArray> s_objectReadCommandIds = {"GetObjectData", "GetElementInfo", "GetElementMetaInfo", "GetDataMetaInfo", "GetObjectTypeId"};
const QSet<QByteArray> s_collectionReadCommandIds = {
			"GetObjectVisualStatus", "GetElementsCount", "GetElementIds", "GetCollectionHeaders", "ExportObject",
			"GetDocumentAccess", "GetDocumentShares", "GetSelectableItems", "GetRevisionInfoList", "ExportRevision",
			"GetOpenedDocumentList", "GetDocumentName", "GetUndoInfo", "OpenDocument", "CloseDocument",
			"OnDocumentManagerChanged", "OnDocumentChanged", "OnUndoRedoChanged"};
const QSet<QByteArray> s_collectionEditCommandIds = {
			"DuplicateElements", "RemoveElements", "RemoveElementSet", "RestoreObjects", "RestoreObjectSet",
			"SetObjectName", "SetObjectDescription", "ImportObject", "InsertNewObject", "SetObjectData",
			"CreateSubCollection", "ShareDocuments", "RevokeDocumentShares",
			"BackupRevision", "RestoreRevision", "DeleteRevision",
			"BeginCollectionImportSession",
			"CreateNewDocument", "SetDocumentName", "SaveDocument", "DoUndo", "DoRedo", "ResetUndo"};


} // namespace


// public methods

// reimplemented (IServerTokenReceiver)

QByteArray CServerTokenReceiverComp::GetServerProductId() const
{
	return m_permissionTreeCompPtr->GetProductId();
}


imtauth::IPersonalAccessTokenManager* CServerTokenReceiverComp::GetTokenManager() const
{
	return m_tokenManagerCompPtr.GetPtr();
}


bool CServerTokenReceiverComp::FindRequiredPermission(const imtgql::CGqlRequest& gqlRequest, QByteArray& permission) const
{
	permission.clear();

	const QByteArray commandId = gqlRequest.GetCommandId();
	const auto commandIter = m_permissionsOfCommands.constFind(commandId);
	if (commandIter != m_permissionsOfCommands.constEnd()){
		permission = *commandIter;

		return true;
	}

	// Any other command naming a collection would pass as a request of that collection
	const bool isObjectReading = s_objectReadCommandIds.contains(commandId);
	const bool isReading = isObjectReading || s_collectionReadCommandIds.contains(commandId);
	if (!isReading && !s_collectionEditCommandIds.contains(commandId)){
		return false;
	}

	const imtgql::CGqlParamObject* inputPtr = gqlRequest.GetParamObject("input");
	const QByteArray collectionId = inputPtr != nullptr ? inputPtr->GetParamArgumentValue("collectionId").toByteArray() : QByteArray();
	const auto collectionIter = m_permissionsOfCollections.constFind(collectionId);
	if (collectionIter == m_permissionsOfCollections.constEnd()){
		return false;
	}

	if (isObjectReading && !collectionIter->objectRead.isEmpty()){
		permission = collectionIter->objectRead;
	}
	else{
		permission = isReading ? collectionIter->view : collectionIter->edit;
	}

	return !permission.isEmpty();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CServerTokenReceiverComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// A malformed entry declares nothing: its request stays closed
	for (int index = 0; index < m_commandPermissionsAttrPtr.GetCount(); index++){
		const QByteArray entry = m_commandPermissionsAttrPtr[index];
		const qsizetype separatorIndex = entry.indexOf('=');
		const QByteArray commandId = entry.left(separatorIndex).trimmed();
		if (separatorIndex <= 0 || commandId.isEmpty() || m_permissionsOfCommands.contains(commandId)){
			SendErrorMessage(0, QStringLiteral("The entry '%1' of CommandPermissions is not a new 'Command=Permission'").arg(QString::fromUtf8(entry)), "CServerTokenReceiverComp");

			continue;
		}

		m_permissionsOfCommands.insert(commandId, entry.mid(separatorIndex + 1).trimmed());
	}

	for (int index = 0; index < m_collectionPermissionsAttrPtr.GetCount(); index++){
		const QByteArray entry = m_collectionPermissionsAttrPtr[index];
		const qsizetype separatorIndex = entry.indexOf('=');
		const QByteArray collectionId = entry.left(separatorIndex).trimmed();
		const QByteArrayList permissions = entry.mid(separatorIndex + 1).split(',');
		if (separatorIndex <= 0 || collectionId.isEmpty() || permissions.size() > 3 || m_permissionsOfCollections.contains(collectionId)){
			SendErrorMessage(0, QStringLiteral("The entry '%1' of CollectionPermissions is not a new 'Collection=View[,Edit[,ObjectRead]]'").arg(QString::fromUtf8(entry)), "CServerTokenReceiverComp");

			continue;
		}

		CollectionPermissions collectionPermissions;
		collectionPermissions.view = permissions[0].trimmed();
		collectionPermissions.edit = permissions.size() > 1 ? permissions[1].trimmed() : QByteArray();
		collectionPermissions.objectRead = permissions.size() > 2 ? permissions[2].trimmed() : QByteArray();
		m_permissionsOfCollections.insert(collectionId, collectionPermissions);
	}
}


} // namespace imtservergql
