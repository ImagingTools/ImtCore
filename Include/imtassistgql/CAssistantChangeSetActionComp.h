// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IChangeSetJournal.h>
#include <imtbase/IObjectCollection.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtassistgql/CGqlAssistantActionCompBase.h>


namespace imtassistgql
{


/**
	Assistant actions on the change sets of the confirmed changes (attribute \c Kind):
	- \c list: \c changes.list, the latest change sets of the user in the organization and whether they can be rolled back;
	- \c rollback: \c changes.rollback, rolls a change set back as a whole after the user confirmed the preview.

	A rollback writes the former revisions of the documents again as new revisions and removes the documents the
	change set created; it is itself a change set (origin "rollback"). Only documents of the configured collections
	are rolled back: the collections with revisions whose delegate records the documents in the journal.
	Change sets that removed or restored documents are not rolled back: removing a document also changed references
	in other documents and its bindings, which restoring the document does not bring back.

	The checks before a rollback:
	- the change set belongs to the organization of the request and to the user (or the user holds RollbackAnyPermission);
	- every document is in the state the change set left it in (else it was changed later and the rollback is refused);
	- the user holds the configured edit permission (restore a revision) or remove permission (remove a created document)
	  of the collection of every document; the collections are written directly, not through the GraphQL handlers.

	Writes of the domain logic of a delegate to other documents (for example the members of a group in the user
	documents) are not recorded; rolling back the document lets the same domain logic adjust them again.

	\ingroup imtassistgql
*/
class CAssistantChangeSetActionComp: public CGqlAssistantActionCompBase
{
public:
	typedef CGqlAssistantActionCompBase BaseClass;

	enum
	{
		MAX_CHANGE_SETS = 20,
		MAX_DOCUMENTS = 200
	};

	I_BEGIN_COMPONENT(CAssistantChangeSetActionComp);
		I_ASSIGN(m_kindAttrPtr, "Kind", "list or rollback", true, "list");
		I_ASSIGN_MULTI_0(m_collectionsCompPtr, "Collections", "Collections whose documents can be rolled back, in the order of Delegates", false);
		I_ASSIGN_MULTI_0(m_delegatesCompPtr, "Delegates", "Document delegates of the collections: table and revisions", false);
		I_ASSIGN_MULTI_0(m_editPermissionsAttrPtr, "EditPermissions", "Permission per collection needed to restore a former revision", false);
		I_ASSIGN_MULTI_0(m_removePermissionsAttrPtr, "RemovePermissions", "Permission per collection needed to remove a document the change set created", false);
		I_ASSIGN(m_rollbackAnyPermissionAttrPtr, "RollbackAnyPermission", "Permission needed to list and roll back the change sets of other users", true, "/Administration/Assistant/RollbackAny");
	I_END_COMPONENT;

	CAssistantChangeSetActionComp();

	// reimplemented (imtassist::IAssistantAction)
	virtual QByteArray GetActionId() const override;
	virtual QString GetDescription() const override;
	virtual ActionKind GetActionKind() const override;
	virtual const imtassist::CParameterSchema& GetParameterSchema() const override;
	virtual const imtassist::CParameterSchema& GetResultSchema() const override;
	virtual bool CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& parameters) const override;
	virtual ExecutionStatus Execute(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				Result& result,
				QString& userError) override;
	virtual ExecutionStatus PrepareChange(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				ChangePreview& preview,
				QString& userError) override;
	virtual ExecutionStatus ApplyChange(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				const QByteArray& stateToken,
				Result& result,
				QString& userError) override;

protected:
	/**
		What a rollback does with one document: the state before the first and after the last entry of the change set.
	*/
	struct DocumentPlan
	{
		int collectionIndex = -1;
		QByteArray documentId;
		imtbase::IChangeSetJournal::EntryOperation firstOperation = imtbase::IChangeSetJournal::EO_UNRECORDED;
		qlonglong revisionBefore = -1;
		qlonglong revisionAfter = -1;
		imtbase::IChangeSetJournal::DocumentState currentState;
		QString name;
	};

	/**
		Group the entries by document and check that the change set can be rolled back at all.
		\param reason	Why not, for the user.
	*/
	bool CreatePlan(const QList<imtbase::IChangeSetJournal::Entry>& entries, QList<DocumentPlan>& plan, QString& reason) const;

	/**
		Everything checked before a rollback: change set, owner, state, entries, current documents and permissions.
	*/
	ExecutionStatus PrepareRollback(
				const imtassist::CAssistantRequestContext& context,
				const QByteArray& changeSetId,
				imtbase::IChangeSetJournal::ChangeSetInfo& info,
				QList<DocumentPlan>& plan,
				QString& userError) const;

	/**
		Roll one document back with the operation context of the rollback.
	*/
	bool RollbackDocument(const DocumentPlan& documentPlan, const imtbase::IOperationContext& operationContext, QString& error) const;

	static bool IsChanged(const DocumentPlan& documentPlan);
	static QByteArray CreateStateToken(const QByteArray& changeSetId, const QList<DocumentPlan>& plan);
	static QString GetStateName(imtbase::IChangeSetJournal::ChangeSetState state);

private:
	enum Kind
	{
		K_LIST,
		K_ROLLBACK
	};

	Kind GetKind() const;
	int FindCollection(const QByteArray& tableSchema, const QByteArray& tableName) const;
	QByteArray GetEditPermission(int collectionIndex) const;
	QByteArray GetRemovePermission(int collectionIndex) const;

private:
	I_ATTR(QByteArray, m_kindAttrPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_collectionsCompPtr);
	I_MULTIREF(imtdb::ISqlDatabaseObjectDelegate, m_delegatesCompPtr);
	I_MULTIATTR(QByteArray, m_editPermissionsAttrPtr);
	I_MULTIATTR(QByteArray, m_removePermissionsAttrPtr);
	I_ATTR(QByteArray, m_rollbackAnyPermissionAttrPtr);

	imtassist::CParameterSchema m_listParameterSchema;
	imtassist::CParameterSchema m_listResultSchema;
	imtassist::CParameterSchema m_rollbackParameterSchema;
	imtassist::CParameterSchema m_rollbackResultSchema;
};


} // namespace imtassistgql


