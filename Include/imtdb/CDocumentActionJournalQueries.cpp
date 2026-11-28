// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CDocumentActionJournalQueries.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtbase/IDocumentActionContext.h>
#include <imtbase/imtbase.h>
#include <imtdb/imtdb.h>


namespace imtdb
{


const QString CDocumentActionJournalQueries::s_documentActionsTable = QStringLiteral("DocumentActions");
const QString CDocumentActionJournalQueries::s_entriesTable = QStringLiteral("DocumentActionEntries");


namespace
{


struct StateName
{
	imtdb::IDocumentActionJournal::DocumentActionState state;
	const char* id;
};


const StateName s_stateNames[] = {
	{imtdb::IDocumentActionJournal::DAS_APPLYING, "Applying"},
	{imtdb::IDocumentActionJournal::DAS_APPLIED, "Applied"},
	{imtdb::IDocumentActionJournal::DAS_PARTIALLY_APPLIED, "PartiallyApplied"},
	{imtdb::IDocumentActionJournal::DAS_FAILED, "Failed"},
	{imtdb::IDocumentActionJournal::DAS_ROLLED_BACK, "RolledBack"}
};


struct OperationName
{
	imtdb::IDocumentActionJournal::EntryOperation operation;
	const char* id;
};


const OperationName s_operationNames[] = {
	{imtdb::IDocumentActionJournal::EO_CREATED, "Created"},
	{imtdb::IDocumentActionJournal::EO_UPDATED, "Updated"},
	{imtdb::IDocumentActionJournal::EO_REMOVED, "Removed"},
	{imtdb::IDocumentActionJournal::EO_RESTORED, "Restored"},
	{imtdb::IDocumentActionJournal::EO_UNRECORDED, "Unrecorded"}
};


} // namespace


QByteArray CDocumentActionJournalQueries::GetStateId(imtdb::IDocumentActionJournal::DocumentActionState state)
{
	for (const StateName& stateName: s_stateNames){
		if (stateName.state == state){
			return QByteArray(stateName.id);
		}
	}

	return QByteArray();
}


bool CDocumentActionJournalQueries::GetStateFromId(const QByteArray& stateId, imtdb::IDocumentActionJournal::DocumentActionState& state)
{
	for (const StateName& stateName: s_stateNames){
		if (stateId == stateName.id){
			state = stateName.state;

			return true;
		}
	}

	return false;
}


QByteArray CDocumentActionJournalQueries::GetOperationId(imtdb::IDocumentActionJournal::EntryOperation operation)
{
	for (const OperationName& operationName: s_operationNames){
		if (operationName.operation == operation){
			return QByteArray(operationName.id);
		}
	}

	return QByteArray();
}


bool CDocumentActionJournalQueries::GetOperationFromId(const QByteArray& operationId, imtdb::IDocumentActionJournal::EntryOperation& operation)
{
	for (const OperationName& operationName: s_operationNames){
		if (operationId == operationName.id){
			operation = operationName.operation;

			return true;
		}
	}

	return false;
}


QByteArray CDocumentActionJournalQueries::GetDocumentActionId(const imtbase::IOperationContext* operationContextPtr)
{
	const imtbase::IDocumentActionContext* documentActionContextPtr = dynamic_cast<const imtbase::IDocumentActionContext*>(operationContextPtr);
	if (documentActionContextPtr == nullptr){
		return QByteArray();
	}

	const QByteArray documentActionId = documentActionContextPtr->GetDocumentActionId();
	if (documentActionId.isEmpty() || QUuid::fromString(QString::fromLatin1(documentActionId)).isNull()){
		return QByteArray();
	}

	return documentActionId;
}


QString CDocumentActionJournalQueries::CreateDocumentEntryQuery(
			const imtbase::IOperationContext* operationContextPtr,
			const QByteArray& tableSchema,
			const QByteArray& tableName,
			const QByteArray& documentId,
			imtdb::IDocumentActionJournal::EntryOperation operation,
			const QString& revisionExpression)
{
	const QByteArray documentActionId = GetDocumentActionId(operationContextPtr);
	if (documentActionId.isEmpty() || revisionExpression.isEmpty()){
		return QString();
	}

	const QString table = GetQualifiedTableName(tableSchema, tableName);
	const QString documentCondition = QStringLiteral(R"(%1."DocumentId" = %2)").arg(table, Text(documentId));
	const QString activeRevision = QStringLiteral(R"((SELECT %1 FROM %2 WHERE %3 AND %2."State" = 'Active'))")
				.arg(revisionExpression, table, documentCondition);

	QString revisionBefore;
	QString revisionAfter;
	QString condition;

	switch (operation){
	case imtdb::IDocumentActionJournal::EO_CREATED:
		// A new document starts with the revision 1
		revisionBefore = NULL_DATA_LITERAL;
		revisionAfter = QStringLiteral("1");
		break;

	case imtdb::IDocumentActionJournal::EO_UPDATED:
		// The same expression as the revision of the new row
		revisionBefore = activeRevision;
		revisionAfter = QStringLiteral(R"((SELECT MAX(%1) + 1 FROM %2 WHERE %3))").arg(revisionExpression, table, documentCondition);
		break;

	case imtdb::IDocumentActionJournal::EO_REMOVED:
		revisionBefore = activeRevision;
		revisionAfter = activeRevision;
		condition = QStringLiteral(R"(EXISTS (SELECT 1 FROM %1 WHERE %2 AND %1."State" = 'Active'))").arg(table, documentCondition);
		break;

	case imtdb::IDocumentActionJournal::EO_RESTORED:
		revisionBefore = QStringLiteral(R"((SELECT MAX(%1) FROM %2 WHERE %3 AND %2."State" = 'Disabled'))")
					.arg(revisionExpression, table, documentCondition);
		revisionAfter = revisionBefore;
		condition = QStringLiteral(R"(EXISTS (SELECT 1 FROM %1 WHERE %2 AND %1."State" = 'Disabled'))").arg(table, documentCondition);
		break;

	case imtdb::IDocumentActionJournal::EO_UNRECORDED:
		return CreateUnrecordedEntryQuery(operationContextPtr, tableSchema, tableName);
	}

	return CreateEntryQuery(
				documentActionId,
				operationContextPtr->GetTenantId(),
				tableSchema,
				tableName,
				documentId,
				GetOperationId(operation),
				revisionBefore,
				revisionAfter,
				condition);
}


QString CDocumentActionJournalQueries::CreateUnrecordedEntryQuery(
			const imtbase::IOperationContext* operationContextPtr,
			const QByteArray& tableSchema,
			const QByteArray& tableName)
{
	const QByteArray documentActionId = GetDocumentActionId(operationContextPtr);
	if (documentActionId.isEmpty()){
		return QString();
	}

	return CreateEntryQuery(
				documentActionId,
				operationContextPtr->GetTenantId(),
				tableSchema,
				tableName,
				QByteArray(),
				GetOperationId(imtdb::IDocumentActionJournal::EO_UNRECORDED),
				NULL_DATA_LITERAL,
				NULL_DATA_LITERAL,
				QString());
}


bool CDocumentActionJournalQueries::ContainsEntryQuery(const QByteArray& query)
{
	static const QByteArray entryStatement = QStringLiteral("INSERT INTO %1 ").arg(QuoteIdentifier(s_entriesTable)).toUtf8();

	return query.contains(entryStatement);
}


QString CDocumentActionJournalQueries::GetQualifiedTableName(const QByteArray& tableSchema, const QByteArray& tableName)
{
	const QString quotedTable = QuoteIdentifier(QString::fromUtf8(tableName));
	if (tableSchema.isEmpty()){
		return quotedTable;
	}

	// The schema is written as the delegates write it: without quotes
	return QStringLiteral("%1.%2").arg(QString::fromUtf8(tableSchema), quotedTable);
}


// private static methods

QString CDocumentActionJournalQueries::CreateEntryQuery(
			const QByteArray& documentActionId,
			const QByteArray& tenantId,
			const QByteArray& tableSchema,
			const QByteArray& tableName,
			const QByteArray& documentId,
			const QByteArray& operationId,
			const QString& revisionBefore,
			const QString& revisionAfter,
			const QString& condition)
{
	const QString documentActions = QuoteIdentifier(s_documentActionsTable);
	const QString entries = QuoteIdentifier(s_entriesTable);

	QString query = QStringLiteral(
				R"(INSERT INTO %1 ("Id", "DocumentActionId", "Position", "TableSchema", "TableName", "DocumentId", "Operation", "RevisionBefore", "RevisionAfter") )"
				R"(SELECT %2, %3."Id", (SELECT COUNT(*) FROM %1 WHERE %1."DocumentActionId" = %4), %5, %6, %7, %8, %9, %10 )"
				R"(FROM %3 WHERE %3."Id" = %4 AND %3."TenantId" = %11 AND %3."State" = 'Applying')")
				.arg(
							/*1*/ entries,
							/*2*/ Text(QUuid::createUuid().toByteArray(QUuid::WithoutBraces)),
							/*3*/ documentActions,
							/*4*/ Text(documentActionId),
							/*5*/ Text(tableSchema),
							/*6*/ Text(tableName),
							/*7*/ documentId.isEmpty() ? NULL_DATA_LITERAL : Text(documentId),
							/*8*/ Text(operationId),
							/*9*/ revisionBefore,
							/*10*/ revisionAfter,
							/*11*/ Text(tenantId));

	if (!condition.isEmpty()){
		query += QStringLiteral(" AND ") + condition;
	}

	return query + QLatin1Char(';');
}


QString CDocumentActionJournalQueries::Text(const QByteArray& value)
{
	return SqlEncode(QString::fromUtf8(value)).prepend('\'').append('\'');
}


} // namespace imtdb


