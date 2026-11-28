// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ImtCore includes
#include <imtbase/IChangeSetJournal.h>
#include <imtbase/IOperationContext.h>


namespace imtdb
{


/**
	SQL of the change set journal (tables "ChangeSets" and "ChangeSetEntries", see CChangeSetJournalComp),
	shared by the journal and the document delegates that write the entries.

	An entry statement is written before the statements that change the document, in the same transaction.
	It adds the entry only while the change set of the tenant is in the state "Applying", so an operation
	context with an unknown, finished or foreign change set writes nothing into the journal.
	The statements contain no ';' inside values, so they can be split into single statements.
*/
class CChangeSetJournalQueries
{
public:
	static const QString s_changeSetsTable;
	static const QString s_entriesTable;

	static QByteArray GetStateId(imtbase::IChangeSetJournal::ChangeSetState state);
	static bool GetStateFromId(const QByteArray& stateId, imtbase::IChangeSetJournal::ChangeSetState& state);
	static QByteArray GetOperationId(imtbase::IChangeSetJournal::EntryOperation operation);
	static bool GetOperationFromId(const QByteArray& operationId, imtbase::IChangeSetJournal::EntryOperation& operation);

	/**
		ID of the change set of an operation; empty without a change set or if the ID is not a UUID.
	*/
	static QByteArray GetChangeSetId(const imtbase::IOperationContext* operationContextPtr);

	/**
		Statement adding an entry for a document of a table with revisions.
		\param revisionExpression	SQL expression of the revision number of a row of the table.
		\return Empty without a change set in the operation context.
	*/
	static QString CreateDocumentEntryQuery(
				const imtbase::IOperationContext* operationContextPtr,
				const QByteArray& tableSchema,
				const QByteArray& tableName,
				const QByteArray& documentId,
				imtbase::IChangeSetJournal::EntryOperation operation,
				const QString& revisionExpression);

	/**
		Statement adding an entry of the kind EO_UNRECORDED: the write cannot be rolled back.
		\return Empty without a change set in the operation context.
	*/
	static QString CreateUnrecordedEntryQuery(
				const imtbase::IOperationContext* operationContextPtr,
				const QByteArray& tableSchema,
				const QByteArray& tableName);

	/**
		Whether the statements of a write contain an entry statement of the journal.
	*/
	static bool ContainsEntryQuery(const QByteArray& query);

	/**
		Table name, quoted, with its schema as the delegates write it.
	*/
	static QString GetQualifiedTableName(const QByteArray& tableSchema, const QByteArray& tableName);

private:
	static QString CreateEntryQuery(
				const QByteArray& changeSetId,
				const QByteArray& tenantId,
				const QByteArray& tableSchema,
				const QByteArray& tableName,
				const QByteArray& documentId,
				const QByteArray& operationId,
				const QString& revisionBefore,
				const QString& revisionAfter,
				const QString& condition);
	static QString Text(const QByteArray& value);
};


} // namespace imtdb


