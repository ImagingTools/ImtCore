// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ImtCore includes
#include <imtdb/IDocumentActionJournal.h>
#include <imtbase/IOperationContext.h>


namespace imtdb
{


/**
	SQL of the document action journal (tables "DocumentActions" and "DocumentActionEntries", see CDocumentActionJournalComp),
	shared by the journal and the document delegates that write the entries.

	An entry statement is written before the statements that change the document, in the same transaction.
	It adds the entry only while the document action of the tenant is in the state "Applying", so an operation
	context with an unknown, finished or foreign document action writes nothing into the journal.
	The statements contain no ';' inside values, so they can be split into single statements.
*/
class CDocumentActionJournalQueries
{
public:
	static const QString s_documentActionsTable;
	static const QString s_entriesTable;

	static QByteArray GetStateId(imtdb::IDocumentActionJournal::DocumentActionState state);
	static bool GetStateFromId(const QByteArray& stateId, imtdb::IDocumentActionJournal::DocumentActionState& state);
	static QByteArray GetOperationId(imtdb::IDocumentActionJournal::EntryOperation operation);
	static bool GetOperationFromId(const QByteArray& operationId, imtdb::IDocumentActionJournal::EntryOperation& operation);

	/**
		ID of the document action of an operation; empty without a document action or if the ID is not a UUID.
	*/
	static QByteArray GetDocumentActionId(const imtbase::IOperationContext* operationContextPtr);

	/**
		Statement adding an entry for a document of a table with revisions.
		\param revisionExpression	SQL expression of the revision number of a row of the table.
		\return Empty without a document action in the operation context.
	*/
	static QString CreateDocumentEntryQuery(
				const imtbase::IOperationContext* operationContextPtr,
				const QByteArray& tableSchema,
				const QByteArray& tableName,
				const QByteArray& documentId,
				imtdb::IDocumentActionJournal::EntryOperation operation,
				const QString& revisionExpression);

	/**
		Statement adding an entry of the kind EO_UNRECORDED: the write cannot be rolled back.
		\return Empty without a document action in the operation context.
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
				const QByteArray& documentActionId,
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


