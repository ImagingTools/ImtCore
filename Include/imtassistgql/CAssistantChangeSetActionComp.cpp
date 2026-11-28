// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAssistantChangeSetActionComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtbase/COperationContext.h>
#include <imtbase/IRevisionController.h>


namespace imtassistgql
{


namespace
{


QString GetOperationName(imtbase::IChangeSetJournal::EntryOperation operation)
{
	switch (operation){
	case imtbase::IChangeSetJournal::EO_CREATED:
		return QStringLiteral("created");
	case imtbase::IChangeSetJournal::EO_UPDATED:
		return QStringLiteral("changed");
	case imtbase::IChangeSetJournal::EO_REMOVED:
		return QStringLiteral("removed");
	case imtbase::IChangeSetJournal::EO_RESTORED:
		return QStringLiteral("restored");
	default:
		return QStringLiteral("not recorded");
	}
}


} // anonymous namespace


CAssistantChangeSetActionComp::CAssistantChangeSetActionComp()
	:m_listParameterSchema("changes.list.parameters", 1),
	m_listResultSchema("changes.list.result", 1),
	m_rollbackParameterSchema("changes.rollback.parameters", 1),
	m_rollbackResultSchema("changes.rollback.result", 1)
{
	m_listParameterSchema.AddInteger("maxCount", false, 1, MAX_CHANGE_SETS);
	m_listParameterSchema.AddBoolean("allUsers", false);

	std::shared_ptr<imtassist::CParameterSchema> changeSetSchemaPtr = std::make_shared<imtassist::CParameterSchema>("changes.list.changeSet", 1);
	changeSetSchemaPtr->AddId("changeSetId", true);
	changeSetSchemaPtr->AddString("createdAt", true, 40);
	changeSetSchemaPtr->AddString("summary", false, 600);
	changeSetSchemaPtr->AddString("origin", false, 40);
	changeSetSchemaPtr->AddEnum("state", true, QStringList() << "applying" << "applied" << "partiallyApplied" << "failed" << "rolledBack");
	changeSetSchemaPtr->AddInteger("documentCount", true, 0, std::numeric_limits<qint32>::max());
	changeSetSchemaPtr->AddBoolean("reversible", true);
	changeSetSchemaPtr->AddString("reason", false, 300);
	m_listResultSchema.AddObjectList("changeSets", true, changeSetSchemaPtr, MAX_CHANGE_SETS);

	m_rollbackParameterSchema.AddId("changeSetId", true);

	m_rollbackResultSchema.AddInteger("restoredDocuments", true, 0, MAX_DOCUMENTS);
	m_rollbackResultSchema.AddId("changeSetId", false);
}


// reimplemented (imtassist::IAssistantAction)

QByteArray CAssistantChangeSetActionComp::GetActionId() const
{
	return (GetKind() == K_ROLLBACK) ? "changes.rollback" : "changes.list";
}


QString CAssistantChangeSetActionComp::GetDescription() const
{
	if (GetKind() == K_ROLLBACK){
		return "Roll back a change set as a whole: the documents get their former state again, created documents are removed. "
			   "Takes a change set ID from changes.list or from the result of an applied change. The user confirms the rollback; "
			   "it is refused if a document was changed after the change set.";
	}

	return "List the latest confirmed change sets of the user in the current organization (newest first), with whether each can be rolled back. "
		   "Use it when the user wants to undo a change.";
}


imtassist::IAssistantAction::ActionKind CAssistantChangeSetActionComp::GetActionKind() const
{
	return (GetKind() == K_ROLLBACK) ? AK_CHANGE : AK_READ;
}


const imtassist::CParameterSchema& CAssistantChangeSetActionComp::GetParameterSchema() const
{
	return (GetKind() == K_ROLLBACK) ? m_rollbackParameterSchema : m_listParameterSchema;
}


const imtassist::CParameterSchema& CAssistantChangeSetActionComp::GetResultSchema() const
{
	return (GetKind() == K_ROLLBACK) ? m_rollbackResultSchema : m_listResultSchema;
}


bool CAssistantChangeSetActionComp::CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& parameters) const
{
	if ((GetChangeSetJournal() == nullptr) || (GetGqlContext(context) == nullptr)){
		return false;
	}

	if ((GetKind() == K_LIST) && parameters.value("allUsers").toBool()){
		return HasPermission(context, *m_rollbackAnyPermissionAttrPtr);
	}

	return true;
}


imtassist::IAssistantAction::ExecutionStatus CAssistantChangeSetActionComp::Execute(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			Result& result,
			QString& userError)
{
	imtbase::IChangeSetJournal* journalPtr = GetChangeSetJournal();
	if ((GetKind() != K_LIST) || (journalPtr == nullptr)){
		return ES_FAILED;
	}

	const QByteArray ownerId = parameters.value("allUsers").toBool() ? QByteArray() : context.GetUserId();
	const int maxCount = parameters.contains("maxCount") ? parameters.value("maxCount").toInt() : 10;

	const QList<imtbase::IChangeSetJournal::ChangeSetInfo> changeSets = journalPtr->GetChangeSets(GetDataTenantId(context), ownerId, qBound(1, maxCount, int(MAX_CHANGE_SETS)));

	QJsonArray items;
	for (const imtbase::IChangeSetJournal::ChangeSetInfo& info: changeSets){
		QString reason;
		bool isReversible = false;
		if ((info.state == imtbase::IChangeSetJournal::CSS_APPLIED) || (info.state == imtbase::IChangeSetJournal::CSS_PARTIALLY_APPLIED)){
			QList<imtbase::IChangeSetJournal::Entry> entries;
			QList<DocumentPlan> plan;
			if (!journalPtr->GetEntries(info.tenantId, info.id, entries)){
				reason = QStringLiteral("The change set could not be read.");
			}
			else{
				isReversible = CreatePlan(entries, plan, reason);
			}
		}
		else if (info.state == imtbase::IChangeSetJournal::CSS_ROLLED_BACK){
			reason = QStringLiteral("The change set was rolled back already.");
		}
		else{
			reason = QStringLiteral("Nothing to roll back.");
		}

		QJsonObject item;
		item["changeSetId"] = QString::fromUtf8(info.id);
		item["createdAt"] = info.createdAt.toString(Qt::ISODate);
		item["summary"] = info.summary;
		item["origin"] = QString::fromUtf8(info.origin);
		item["state"] = GetStateName(info.state);
		item["documentCount"] = info.entryCount;
		item["reversible"] = isReversible;
		if (!reason.isEmpty()){
			item["reason"] = reason;
		}

		items.append(item);
	}

	result.data["changeSets"] = items;
	if (items.isEmpty()){
		result.text = QStringLiteral("There are no change sets.");
	}

	userError.clear();

	return ES_SUCCEEDED;
}


imtassist::IAssistantAction::ExecutionStatus CAssistantChangeSetActionComp::PrepareChange(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			ChangePreview& preview,
			QString& userError)
{
	if (GetKind() != K_ROLLBACK){
		return ES_FAILED;
	}

	const QByteArray changeSetId = parameters.value("changeSetId").toString().toUtf8();

	imtbase::IChangeSetJournal::ChangeSetInfo info;
	QList<DocumentPlan> plan;
	const ExecutionStatus status = PrepareRollback(context, changeSetId, info, plan, userError);
	if (status != ES_SUCCEEDED){
		return status;
	}

	for (const DocumentPlan& documentPlan: plan){
		QJsonObject change;
		change["field"] = documentPlan.name.isEmpty() ? QString::fromUtf8(documentPlan.documentId) : documentPlan.name;
		change["oldValue"] = QStringLiteral("%1 (revision %2)").arg(GetOperationName(documentPlan.firstOperation)).arg(documentPlan.currentState.revision);
		change["newValue"] = (documentPlan.firstOperation == imtbase::IChangeSetJournal::EO_CREATED)
					? QStringLiteral("removed")
					: QStringLiteral("state of revision %1").arg(documentPlan.revisionBefore);
		preview.changes.append(change);
	}

	preview.summary = QStringLiteral("Roll back: %1").arg(info.summary.isEmpty() ? QString::fromUtf8(info.id) : info.summary);
	preview.targetLabel = QStringLiteral("%1 documents").arg(plan.size());
	preview.targetId = info.id;
	preview.impact = QStringLiteral("The documents get the state they had before the change set; documents the change set created are removed. "
									"Other documents that follow them (for example the memberships of the users of a group) are adjusted as when the document is edited.");
	preview.stateToken = CreateStateToken(info.id, plan);

	return ES_SUCCEEDED;
}


imtassist::IAssistantAction::ExecutionStatus CAssistantChangeSetActionComp::ApplyChange(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			const QByteArray& stateToken,
			Result& result,
			QString& userError)
{
	imtbase::IChangeSetJournal* journalPtr = GetChangeSetJournal();
	if ((GetKind() != K_ROLLBACK) || (journalPtr == nullptr)){
		return ES_FAILED;
	}

	const QByteArray changeSetId = parameters.value("changeSetId").toString().toUtf8();

	imtbase::IChangeSetJournal::ChangeSetInfo info;
	QList<DocumentPlan> plan;
	const ExecutionStatus status = PrepareRollback(context, changeSetId, info, plan, userError);
	if (status != ES_SUCCEEDED){
		return status;
	}

	if (CreateStateToken(info.id, plan) != stateToken){
		return ES_STALE;
	}

	QByteArray rollbackId;
	if (!BeginChangeSet(context, QStringLiteral("Rollback: %1").arg(info.summary), rollbackId, QByteArrayLiteral("rollback"), info.id)){
		userError = QStringLiteral("The rollback could not be recorded; nothing was changed.");

		return ES_FAILED;
	}

	imtbase::TenantType tenantType = imtbase::TT_ORGANIZATION;
	const QByteArray tenantId = GetDataTenantId(context, &tenantType);

	imtbase::IOperationContext::IdentifableObjectInfo ownerInfo(context.GetUserId());
	const imtgql::IGqlContext* gqlContextPtr = GetGqlContext(context);
	if ((gqlContextPtr != nullptr) && (gqlContextPtr->GetUserInfo() != nullptr)){
		ownerInfo.name = gqlContextPtr->GetUserInfo()->GetName();
	}

	imtbase::COperationContext operationContext;
	operationContext.SetOperationOwnerId(ownerInfo);
	operationContext.SetTenantId(tenantId, tenantType);
	operationContext.SetChangeSetId(rollbackId);
	operationContext.SetOperationDescription(QStringLiteral("Rollback of a change set"));

	// The documents in the reverse order of their first change
	int restoredCount = 0;
	for (int i = plan.size() - 1; i >= 0; --i){
		QString error;
		if (!RollbackDocument(plan[i], operationContext, error)){
			SendWarningMessage(0, QStringLiteral("Assistant rollback: a document could not be rolled back: %1").arg(error), "AssistantChangeSets");

			FinishChangeSet(context, rollbackId, restoredCount, plan.size());
			userError = (restoredCount == 0)
						? QStringLiteral("The rollback failed; nothing was changed.")
						: QStringLiteral("%1 of %2 documents were rolled back; then the rollback failed.").arg(restoredCount).arg(plan.size());

			return ES_FAILED;
		}

		++restoredCount;
	}

	FinishChangeSet(context, rollbackId, restoredCount, plan.size());

	if (!journalPtr->SetChangeSetState(tenantId, info.id, imtbase::IChangeSetJournal::CSS_ROLLED_BACK)){
		SendErrorMessage(0, QStringLiteral("Assistant rollback: the change set could not be marked as rolled back"), "AssistantChangeSets");
	}

	result.data["restoredDocuments"] = restoredCount;
	result.text = QStringLiteral("%1 documents were rolled back.").arg(restoredCount);
	if (!rollbackId.isEmpty()){
		result.data["changeSetId"] = QString::fromUtf8(rollbackId);
	}

	return ES_SUCCEEDED;
}


// protected methods

bool CAssistantChangeSetActionComp::CreatePlan(const QList<imtbase::IChangeSetJournal::Entry>& entries, QList<DocumentPlan>& plan, QString& reason) const
{
	plan.clear();

	if (entries.isEmpty()){
		reason = QStringLiteral("The change set changed no documents.");

		return false;
	}

	QMap<QByteArray, int> documentIndices;
	for (const imtbase::IChangeSetJournal::Entry& entry: entries){
		switch (entry.operation){
		case imtbase::IChangeSetJournal::EO_CREATED:
		case imtbase::IChangeSetJournal::EO_UPDATED:
			break;

		case imtbase::IChangeSetJournal::EO_REMOVED:
		case imtbase::IChangeSetJournal::EO_RESTORED:
			reason = QStringLiteral("The change set removed or restored documents; such change sets cannot be rolled back.");
			plan.clear();

			return false;

		default:
			reason = QStringLiteral("The change set also changed data without revisions; it cannot be rolled back.");
			plan.clear();

			return false;
		}

		const int collectionIndex = FindCollection(entry.tableSchema, entry.tableName);
		if (collectionIndex < 0){
			reason = QStringLiteral("The change set changed documents that the assistant cannot roll back.");
			plan.clear();

			return false;
		}

		if ((entry.operation == imtbase::IChangeSetJournal::EO_UPDATED) && (entry.revisionBefore <= 0)){
			reason = QStringLiteral("The former state of a document is not known.");
			plan.clear();

			return false;
		}

		const QByteArray key = entry.tableSchema + '.' + entry.tableName + '/' + entry.documentId;
		QMap<QByteArray, int>::const_iterator iter = documentIndices.constFind(key);
		if (iter == documentIndices.constEnd()){
			DocumentPlan documentPlan;
			documentPlan.collectionIndex = collectionIndex;
			documentPlan.documentId = entry.documentId;
			documentPlan.firstOperation = entry.operation;
			documentPlan.revisionBefore = entry.revisionBefore;
			documentPlan.revisionAfter = entry.revisionAfter;

			documentIndices.insert(key, plan.size());
			plan.append(documentPlan);
		}
		else{
			plan[iter.value()].revisionAfter = entry.revisionAfter;
		}
	}

	if (plan.size() > MAX_DOCUMENTS){
		reason = QStringLiteral("The change set changed too many documents to roll it back here.");
		plan.clear();

		return false;
	}

	return true;
}


imtassist::IAssistantAction::ExecutionStatus CAssistantChangeSetActionComp::PrepareRollback(
			const imtassist::CAssistantRequestContext& context,
			const QByteArray& changeSetId,
			imtbase::IChangeSetJournal::ChangeSetInfo& info,
			QList<DocumentPlan>& plan,
			QString& userError) const
{
	imtbase::IChangeSetJournal* journalPtr = GetChangeSetJournal();
	if ((journalPtr == nullptr) || (GetGqlContext(context) == nullptr)){
		return ES_ACCESS_DENIED;
	}

	const QByteArray tenantId = GetDataTenantId(context);
	if (changeSetId.isEmpty() || !journalPtr->GetChangeSetInfo(tenantId, changeSetId, info)){
		userError = QStringLiteral("The change set was not found.");

		return ES_FAILED;
	}

	if ((info.ownerId != context.GetUserId()) && !HasPermission(context, *m_rollbackAnyPermissionAttrPtr)){
		// The same answer as for an unknown change set: the change sets of others are not disclosed
		userError = QStringLiteral("The change set was not found.");

		return ES_FAILED;
	}

	switch (info.state){
	case imtbase::IChangeSetJournal::CSS_APPLIED:
	case imtbase::IChangeSetJournal::CSS_PARTIALLY_APPLIED:
		break;
	case imtbase::IChangeSetJournal::CSS_ROLLED_BACK:
		userError = QStringLiteral("The change set was rolled back already.");
		return ES_FAILED;
	case imtbase::IChangeSetJournal::CSS_APPLYING:
		userError = QStringLiteral("The change set is still being applied.");
		return ES_FAILED;
	default:
		userError = QStringLiteral("The change set changed nothing.");
		return ES_FAILED;
	}

	QList<imtbase::IChangeSetJournal::Entry> entries;
	if (!journalPtr->GetEntries(tenantId, changeSetId, entries)){
		userError = QStringLiteral("The change set could not be read.");

		return ES_FAILED;
	}

	if (!CreatePlan(entries, plan, userError)){
		return ES_FAILED;
	}

	QStringList changedNames;
	for (DocumentPlan& documentPlan: plan){
		const imtdb::ISqlDatabaseObjectDelegate* delegatePtr = m_delegatesCompPtr[documentPlan.collectionIndex];
		const imtbase::IObjectCollection* collectionPtr = m_collectionsCompPtr[documentPlan.collectionIndex];

		const QByteArray permission = (documentPlan.firstOperation == imtbase::IChangeSetJournal::EO_CREATED)
					? GetRemovePermission(documentPlan.collectionIndex)
					: GetEditPermission(documentPlan.collectionIndex);
		if (permission.isEmpty() || !HasPermission(context, permission)){
			userError = QStringLiteral("You are not allowed to roll back all documents of this change set.");

			return ES_ACCESS_DENIED;
		}

		if (!journalPtr->GetDocumentState(delegatePtr->GetTableScheme(), delegatePtr->GetTableName(), documentPlan.documentId, documentPlan.currentState)){
			userError = QStringLiteral("The current state of the documents could not be read.");

			return ES_FAILED;
		}

		documentPlan.name = collectionPtr->GetElementInfo(documentPlan.documentId, imtbase::ICollectionInfo::EIT_NAME).toString();

		if (IsChanged(documentPlan)){
			changedNames.append(documentPlan.name.isEmpty() ? QString::fromUtf8(documentPlan.documentId) : documentPlan.name);
		}
	}

	if (!changedNames.isEmpty()){
		userError = QStringLiteral("The change set cannot be rolled back: these documents were changed or removed after it: %1.").arg(changedNames.join(QStringLiteral(", ")));

		return ES_FAILED;
	}

	return ES_SUCCEEDED;
}


bool CAssistantChangeSetActionComp::RollbackDocument(const DocumentPlan& documentPlan, const imtbase::IOperationContext& operationContext, QString& error) const
{
	imtbase::IObjectCollection* collectionPtr = m_collectionsCompPtr[documentPlan.collectionIndex];
	if (collectionPtr == nullptr){
		error = QStringLiteral("collection missing");

		return false;
	}

	if (documentPlan.firstOperation == imtbase::IChangeSetJournal::EO_CREATED){
		if (!collectionPtr->RemoveElements(imtbase::ICollectionInfo::Ids() << documentPlan.documentId, &operationContext)){
			error = QStringLiteral("the created document could not be removed");

			return false;
		}

		return true;
	}

	const imtbase::IRevisionController* revisionControllerPtr = dynamic_cast<const imtbase::IRevisionController*>(m_delegatesCompPtr[documentPlan.collectionIndex]);
	if (revisionControllerPtr == nullptr){
		error = QStringLiteral("the delegate keeps no revisions");

		return false;
	}

	istd::IChangeableSharedPtr dataPtr;
	idoc::MetaInfoPtr metaInfoPtr;
	if (!revisionControllerPtr->GetRevisionData(*collectionPtr, documentPlan.documentId, int(documentPlan.revisionBefore), dataPtr, metaInfoPtr) || !dataPtr.IsValid()){
		error = QStringLiteral("the former revision could not be read");

		return false;
	}

	if (!collectionPtr->SetObjectData(documentPlan.documentId, *dataPtr, istd::IChangeable::CM_WITHOUT_REFS, &operationContext)){
		error = QStringLiteral("the former revision could not be written");

		return false;
	}

	return true;
}


bool CAssistantChangeSetActionComp::IsChanged(const DocumentPlan& documentPlan)
{
	return !documentPlan.currentState.exists ||
				documentPlan.currentState.isRemoved ||
				(documentPlan.currentState.revision != documentPlan.revisionAfter);
}


QByteArray CAssistantChangeSetActionComp::CreateStateToken(const QByteArray& changeSetId, const QList<DocumentPlan>& plan)
{
	QCryptographicHash hash(QCryptographicHash::Sha256);
	hash.addData(changeSetId);
	for (const DocumentPlan& documentPlan: plan){
		hash.addData(QByteArrayLiteral("|"));
		hash.addData(documentPlan.documentId);
		hash.addData(QByteArray::number(documentPlan.currentState.revision));
		hash.addData(documentPlan.currentState.isRemoved ? QByteArrayLiteral("R") : QByteArrayLiteral("A"));
	}

	return hash.result().toHex();
}


QString CAssistantChangeSetActionComp::GetStateName(imtbase::IChangeSetJournal::ChangeSetState state)
{
	switch (state){
	case imtbase::IChangeSetJournal::CSS_APPLIED:
		return QStringLiteral("applied");
	case imtbase::IChangeSetJournal::CSS_PARTIALLY_APPLIED:
		return QStringLiteral("partiallyApplied");
	case imtbase::IChangeSetJournal::CSS_FAILED:
		return QStringLiteral("failed");
	case imtbase::IChangeSetJournal::CSS_ROLLED_BACK:
		return QStringLiteral("rolledBack");
	default:
		return QStringLiteral("applying");
	}
}


// private methods

CAssistantChangeSetActionComp::Kind CAssistantChangeSetActionComp::GetKind() const
{
	const QByteArray kind = m_kindAttrPtr.IsValid() ? *m_kindAttrPtr : QByteArray();

	return (kind == "rollback") ? K_ROLLBACK : K_LIST;
}


int CAssistantChangeSetActionComp::FindCollection(const QByteArray& tableSchema, const QByteArray& tableName) const
{
	const int count = qMin(m_collectionsCompPtr.GetCount(), m_delegatesCompPtr.GetCount());
	for (int i = 0; i < count; ++i){
		const imtdb::ISqlDatabaseObjectDelegate* delegatePtr = m_delegatesCompPtr[i];
		if ((delegatePtr == nullptr) || (m_collectionsCompPtr[i] == nullptr)){
			continue;
		}

		if ((delegatePtr->GetTableName() == tableName) && (delegatePtr->GetTableScheme() == tableSchema)){
			return i;
		}
	}

	return -1;
}


QByteArray CAssistantChangeSetActionComp::GetEditPermission(int collectionIndex) const
{
	return (collectionIndex >= 0) && (collectionIndex < m_editPermissionsAttrPtr.GetCount()) ? m_editPermissionsAttrPtr[collectionIndex] : QByteArray();
}


QByteArray CAssistantChangeSetActionComp::GetRemovePermission(int collectionIndex) const
{
	return (collectionIndex >= 0) && (collectionIndex < m_removePermissionsAttrPtr.GetCount()) ? m_removePermissionsAttrPtr[collectionIndex] : QByteArray();
}


} // namespace imtassistgql


