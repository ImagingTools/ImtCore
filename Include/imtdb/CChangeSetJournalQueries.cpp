// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CChangeSetJournalQueries.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtbase/IChangeSetContext.h>
#include <imtbase/imtbase.h>
#include <imtdb/imtdb.h>


namespace imtdb
{


const QString CChangeSetJournalQueries::s_changeSetsTable = QStringLiteral("ChangeSets");
const QString CChangeSetJournalQueries::s_entriesTable = QStringLiteral("ChangeSetEntries");


namespace
{


struct StateName
{
	imtbase::IChangeSetJournal::ChangeSetState state;
	const char* id;
};


const StateName s_stateNames[] = {
	{imtbase::IChangeSetJournal::CSS_APPLYING, "Applying"},
	{imtbase::IChangeSetJournal::CSS_APPLIED, "Applied"},
	{imtbase::IChangeSetJournal::CSS_PARTIALLY_APPLIED, "PartiallyApplied"},
	{imtbase::IChangeSetJournal::CSS_FAILED, "Failed"},
	{imtbase::IChangeSetJournal::CSS_ROLLED_BACK, "RolledBack"}
};


struct OperationName
{
	imtbase::IChangeSetJournal::EntryOperation operation;
	const char* id;
};


const OperationName s_operationNames[] = {
	{imtbase::IChangeSetJournal::EO_CREATED, "Created"},
	{imtbase::IChangeSetJournal::EO_UPDATED, "Updated"},
	{imtbase::IChangeSetJournal::EO_REMOVED, "Removed"},
	{imtbase::IChangeSetJournal::EO_RESTORED, "Restored"},
	{imtbase::IChangeSetJournal::EO_UNRECORDED, "Unrecorded"}
};


} // namespace


QByteArray CChangeSetJournalQueries::GetStateId(imtbase::IChangeSetJournal::ChangeSetState state)
{
	for (const StateName& stateName: s_stateNames){
		if (stateName.state == state){
			return QByteArray(stateName.id);
		}
	}

	return QByteArray();
}


bool CChangeSetJournalQueries::GetStateFromId(const QByteArray& stateId, imtbase::IChangeSetJournal::ChangeSetState& state)
{
	for (const StateName& stateName: s_stateNames){
		if (stateId == stateName.id){
			state = stateName.state;

			return true;
		}
	}

	return false;
}


QByteArray CChangeSetJournalQueries::GetOperationId(imtbase::IChangeSetJournal::EntryOperation operation)
{
	for (const OperationName& operationName: s_operationNames){
		if (operationName.operation == operation){
			return QByteArray(operationName.id);
		}
	}

	return QByteArray();
}


bool CChangeSetJournalQueries::GetOperationFromId(const QByteArray& operationId, imtbase::IChangeSetJournal::EntryOperation& operation)
{
	for (const OperationName& operationName: s_operationNames){
		if (operationId == operationName.id){
			operation = operationName.operation;

			return true;
		}
	}

	return false;
}


QByteArray CChangeSetJournalQueries::GetChangeSetId(const imtbase::IOperationContext* operationContextPtr)
{
	const imtbase::IChangeSetContext* changeSetContextPtr = dynamic_cast<const imtbase::IChangeSetContext*>(operationContextPtr);
	if (changeSetContextPtr == nullptr){
		return QByteArray();
	}

	const QByteArray changeSetId = changeSetContextPtr->GetChangeSetId();
	if (changeSetId.isEmpty() || QUuid::fromString(QString::fromLatin1(changeSetId)).isNull()){
		return QByteArray();
	}

	return changeSetId;
}


QString CChangeSetJournalQueries::CreateDocumentEntryQuery(
			const imtbase::IOperationContext* operationContextPtr,
			const QByteArray& tableSchema,
			const QByteArray& tableName,
			const QByteArray& documentId,
			imtbase::IChangeSetJournal::EntryOperation operation,
			const QString& revisionExpression)
{
	const QByteArray changeSetId = GetChangeSetId(operationContextPtr);
	if (changeSetId.isEmpty() || revisionExpression.isEmpty()){
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
	case imtbase::IChangeSetJournal::EO_CREATED:
		// A new document starts with the revision 1
		revisionBefore = NULL_DATA_LITERAL;
		revisionAfter = QStringLiteral("1");
		break;

	case imtbase::IChangeSetJournal::EO_UPDATED:
		// The same expression as the revision of the new row
		revisionBefore = activeRevision;
		revisionAfter = QStringLiteral(R"((SELECT MAX(%1) + 1 FROM %2 WHERE %3))").arg(revisionExpression, table, documentCondition);
		break;

	case imtbase::IChangeSetJournal::EO_REMOVED:
		revisionBefore = activeRevision;
		revisionAfter = activeRevision;
		condition = QStringLiteral(R"(EXISTS (SELECT 1 FROM %1 WHERE %2 AND %1."State" = 'Active'))").arg(table, documentCondition);
		break;

	case imtbase::IChangeSetJournal::EO_RESTORED:
		revisionBefore = QStringLiteral(R"((SELECT MAX(%1) FROM %2 WHERE %3 AND %2."State" = 'Disabled'))")
					.arg(revisionExpression, table, documentCondition);
		revisionAfter = revisionBefore;
		condition = QStringLiteral(R"(EXISTS (SELECT 1 FROM %1 WHERE %2 AND %1."State" = 'Disabled'))").arg(table, documentCondition);
		break;

	case imtbase::IChangeSetJournal::EO_UNRECORDED:
		return CreateUnrecordedEntryQuery(operationContextPtr, tableSchema, tableName);
	}

	return CreateEntryQuery(
				changeSetId,
				operationContextPtr->GetTenantId(),
				tableSchema,
				tableName,
				documentId,
				GetOperationId(operation),
				revisionBefore,
				revisionAfter,
				condition);
}


QString CChangeSetJournalQueries::CreateUnrecordedEntryQuery(
			const imtbase::IOperationContext* operationContextPtr,
			const QByteArray& tableSchema,
			const QByteArray& tableName)
{
	const QByteArray changeSetId = GetChangeSetId(operationContextPtr);
	if (changeSetId.isEmpty()){
		return QString();
	}

	return CreateEntryQuery(
				changeSetId,
				operationContextPtr->GetTenantId(),
				tableSchema,
				tableName,
				QByteArray(),
				GetOperationId(imtbase::IChangeSetJournal::EO_UNRECORDED),
				NULL_DATA_LITERAL,
				NULL_DATA_LITERAL,
				QString());
}


bool CChangeSetJournalQueries::ContainsEntryQuery(const QByteArray& query)
{
	static const QByteArray entryStatement = QStringLiteral("INSERT INTO %1 ").arg(QuoteIdentifier(s_entriesTable)).toUtf8();

	return query.contains(entryStatement);
}


QString CChangeSetJournalQueries::GetQualifiedTableName(const QByteArray& tableSchema, const QByteArray& tableName)
{
	const QString quotedTable = QuoteIdentifier(QString::fromUtf8(tableName));
	if (tableSchema.isEmpty()){
		return quotedTable;
	}

	// The schema is written as the delegates write it: without quotes
	return QStringLiteral("%1.%2").arg(QString::fromUtf8(tableSchema), quotedTable);
}


// private static methods

QString CChangeSetJournalQueries::CreateEntryQuery(
			const QByteArray& changeSetId,
			const QByteArray& tenantId,
			const QByteArray& tableSchema,
			const QByteArray& tableName,
			const QByteArray& documentId,
			const QByteArray& operationId,
			const QString& revisionBefore,
			const QString& revisionAfter,
			const QString& condition)
{
	const QString changeSets = QuoteIdentifier(s_changeSetsTable);
	const QString entries = QuoteIdentifier(s_entriesTable);

	QString query = QStringLiteral(
				R"(INSERT INTO %1 ("Id", "ChangeSetId", "Position", "TableSchema", "TableName", "DocumentId", "Operation", "RevisionBefore", "RevisionAfter") )"
				R"(SELECT %2, %3."Id", (SELECT COUNT(*) FROM %1 WHERE %1."ChangeSetId" = %4), %5, %6, %7, %8, %9, %10 )"
				R"(FROM %3 WHERE %3."Id" = %4 AND %3."TenantId" = %11 AND %3."State" = 'Applying')")
				.arg(
							/*1*/ entries,
							/*2*/ Text(QUuid::createUuid().toByteArray(QUuid::WithoutBraces)),
							/*3*/ changeSets,
							/*4*/ Text(changeSetId),
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


QString CChangeSetJournalQueries::Text(const QByteArray& value)
{
	return SqlEncode(QString::fromUtf8(value)).prepend('\'').append('\'');
}


} // namespace imtdb


