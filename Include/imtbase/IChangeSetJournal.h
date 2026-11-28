// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Journal of the change sets of the tenants.
	A change set groups the documents written by one confirmed change (for example a plan of the assistant).
	The journal holds no document data: an entry refers to the revisions of a document before and after the change,
	the data stays in the revisions of the document table.

	The entries are written by the document delegate in the same transaction as the revision, while the change set is
	in the state CSS_APPLYING and the operation context carries its ID (see IChangeSetContext).
	Writes to tables without revisions add an entry of the kind EO_UNRECORDED: such a change set cannot be rolled back.
	\ingroup Collection
*/
class IChangeSetJournal: virtual public istd::IPolymorphic
{
public:
	enum ChangeSetState
	{
		CSS_APPLYING,
		CSS_APPLIED,
		CSS_PARTIALLY_APPLIED,
		CSS_FAILED,
		CSS_ROLLED_BACK
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

	struct ChangeSetInfo
	{
		QByteArray id;
		QByteArray tenantId;
		QByteArray ownerId;
		QByteArray origin;
		QString summary;
		/**
			The change set a rollback reverts; empty for other change sets.
		*/
		QByteArray referenceId;
		ChangeSetState state = CSS_APPLYING;
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
		Create a change set in the state CSS_APPLYING.
		\return ID of the change set, empty on failure.
	*/
	virtual QByteArray BeginChangeSet(
				const QByteArray& tenantId,
				const QByteArray& ownerId,
				const QByteArray& origin,
				const QString& summary,
				const QByteArray& referenceId = QByteArray()) = 0;

	/**
		Set the state of a change set of the tenant.
	*/
	virtual bool SetChangeSetState(const QByteArray& tenantId, const QByteArray& changeSetId, ChangeSetState state) = 0;

	/**
		Get a change set of the tenant.
	*/
	virtual bool GetChangeSetInfo(const QByteArray& tenantId, const QByteArray& changeSetId, ChangeSetInfo& info) const = 0;

	/**
		Latest change sets of the tenant, newest first; of one owner if \c ownerId is not empty.
	*/
	virtual QList<ChangeSetInfo> GetChangeSets(const QByteArray& tenantId, const QByteArray& ownerId, int maxCount) const = 0;

	/**
		Entries of a change set of the tenant in the order they were written.
	*/
	virtual bool GetEntries(const QByteArray& tenantId, const QByteArray& changeSetId, QList<Entry>& entries) const = 0;

	/**
		Current state of a document of a table with revisions.
	*/
	virtual bool GetDocumentState(
				const QByteArray& tableSchema,
				const QByteArray& tableName,
				const QByteArray& documentId,
				DocumentState& state) const = 0;
};


} // namespace imtbase


