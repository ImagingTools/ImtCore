// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtnotify/INotification.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtnotifydb
{


/**
	SQL database delegate for imtnotify::INotification objects.

	Maps notifications to an explicitly-columned "Notifications" table so that
	filtering (by recipient, read state, category, severity), sorting and
	pagination can be pushed down to the database. Works with both PostgreSQL and
	SQLite; the table is auto-created from the bundled SQL scripts via the base
	class \c AutoCreateTable / \c CreateTableScriptPath attributes.

	The bundled CreateNotificationsTable.sql scripts contain the CREATE TABLE
	statement only (one statement per file). Indexes are created separately by
	this class, one \c ExecSqlQuery call per CREATE INDEX statement: the QSQLITE
	Qt driver executes only the first statement of a semicolon-separated batch
	passed to a single query, silently dropping the rest, so a combined
	table+indexes script is only safe on PostgreSQL, never on SQLite.
*/
class CNotificationDbDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationDbDelegateComp)
		I_ASSIGN(m_notificationFactCompPtr, "NotificationFactory", "Factory used for creation of new notification instances", true, "NotificationFactory");
	I_END_COMPONENT

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(
				const QSqlRecord& record,
				const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArrayList& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr) const override;

protected:
	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;

private:
	// Create every "Notifications" index with its own ExecSqlQuery call (see
	// class comment). Safe to call every startup: each statement is
	// "CREATE INDEX IF NOT EXISTS".
	void CreateIndexesIfNeeded() const;

private:
	I_FACT(imtnotify::INotification, m_notificationFactCompPtr);
};


} // namespace imtnotifydb
