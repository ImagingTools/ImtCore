// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>


namespace imtdb
{


/**
	Journal of the document actions of the tenants.
	A document action groups the documents written by one confirmed change (for example a plan of the assistant).
	The journal holds no document data: an entry refers to the revisions of a document before and after the change,
	the data stays in the revisions of the document table.

	The entries are written by the document delegate in the same transaction as the revision, while the document action is
	in the state DAS_APPLYING and the operation context carries its ID (see imtbase::IDocumentActionContext).
	Writes to tables without revisions add an entry of the kind EO_UNRECORDED: such a document action cannot be rolled back.
	\ingroup Collection
*/
class IDocumentActionJournal: virtual public istd::IChangeable
{
public:
	enum DocumentActionState
	{
		DAS_APPLYING,
		DAS_APPLIED,
		DAS_PARTIALLY_APPLIED,
		DAS_FAILED,
		DAS_ROLLED_BACK
	};

	enum EntryOperation
	{
		EO_CREATED,
		EO_UPDATED,
		EO_REMOVED,
		EO_RESTORED,
		/**
			A write the journal could not record (a table without revisions or a write of a selection).
		*/
		EO_UNRECORDED
	};

	struct DocumentActionInfo
	{
		QByteArray id;
		QByteArray tenantId;
		QByteArray ownerId;
		QByteArray origin;
		QString summary;
		/**
			The document action a rollback reverts; empty for other document actions.
		*/
		QByteArray referenceId;
		DocumentActionState state = DAS_APPLYING;
		QDateTime createdAt;
		int entryCount = 0;
	};

	struct Entry
	{
		QByteArray tableSchema;
		QByteArray tableName;
		QByteArray documentId;
		EntryOperation operation = EO_UNRECORDED;
		/**
			Revision before the change; -1 if the document did not exist.
		*/
		qlonglong revisionBefore = -1;
		qlonglong revisionAfter = -1;
	};

	struct DocumentState
	{
		bool exists = false;
		bool isRemoved = false;
		qlonglong revision = -1;
	};

	/**
		Create a document action in the state DAS_APPLYING.
		\return ID of the document action, empty on failure.
	*/
	virtual QByteArray BeginDocumentAction(
				const QByteArray& tenantId,
				const QByteArray& ownerId,
				const QByteArray& origin,
				const QString& summary,
				const QByteArray& referenceId = QByteArray()) = 0;

	/**
		Set the state of a document action of the tenant.
	*/
	virtual bool SetDocumentActionState(const QByteArray& tenantId, const QByteArray& documentActionId, DocumentActionState state) = 0;

	/**
		Get a document action of the tenant.
	*/
	virtual bool GetDocumentActionInfo(const QByteArray& tenantId, const QByteArray& documentActionId, DocumentActionInfo& info) const = 0;

	/**
		Latest document actions of the tenant, newest first; of one owner if \c ownerId is not empty.
	*/
	virtual QList<DocumentActionInfo> GetDocumentActions(const QByteArray& tenantId, const QByteArray& ownerId, int maxCount) const = 0;

	/**
		Entries of a document action of the tenant in the order they were written.
	*/
	virtual bool GetEntries(const QByteArray& tenantId, const QByteArray& documentActionId, QList<Entry>& entries) const = 0;

	/**
		Current state of a document of a table with revisions.
	*/
	virtual bool GetDocumentState(
				const QByteArray& tableSchema,
				const QByteArray& tableName,
				const QByteArray& documentId,
				DocumentState& state) const = 0;
};


} // namespace imtdb


