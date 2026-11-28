// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CDocumentActionJournalComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CDocumentActionJournalQueries.h>
#include <imtdb/imtdb.h>


namespace imtdb
{


namespace
{


QString TimeToText(const QDateTime& time)
{
	return time.toUTC().toString(Qt::ISODateWithMs);
}


QString Text(const QByteArray& value)
{
	return QString::fromUtf8(value);
}


} // namespace


// reimplemented (imtdb::IDocumentActionJournal)

QByteArray CDocumentActionJournalComp::BeginDocumentAction(
			const QByteArray& tenantId,
			const QByteArray& ownerId,
			const QByteArray& origin,
			const QString& summary,
			const QByteArray& referenceId)
{
	if (tenantId.isEmpty() || !EnsureTables()){
		return QByteArray();
	}

	RemoveExpired();

	const QByteArray documentActionId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":Id"), Text(documentActionId));
	bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));
	bindValues.insert(QStringLiteral(":OwnerId"), Text(ownerId));
	bindValues.insert(QStringLiteral(":Origin"), Text(origin));
	bindValues.insert(QStringLiteral(":Summary"), summary.left(MAX_SUMMARY_LENGTH));
	bindValues.insert(QStringLiteral(":ReferenceId"), Text(referenceId));
	bindValues.insert(QStringLiteral(":State"), Text(CDocumentActionJournalQueries::GetStateId(DAS_APPLYING)));
	bindValues.insert(QStringLiteral(":CreatedAt"), TimeToText(QDateTime::currentDateTimeUtc()));

	QSqlQuery sqlQuery;
	const QString query = QStringLiteral(
				R"(INSERT INTO "DocumentActions" ("Id", "TenantId", "OwnerId", "Origin", "Summary", "ReferenceId", "State", "CreatedAt") )"
				R"(VALUES (:Id, :TenantId, :OwnerId, :Origin, :Summary, :ReferenceId, :State, :CreatedAt))");
	if (!ExecQuery(query, bindValues, sqlQuery)){
		return QByteArray();
	}

	return documentActionId;
}


bool CDocumentActionJournalComp::SetDocumentActionState(const QByteArray& tenantId, const QByteArray& documentActionId, DocumentActionState state)
{
	if (tenantId.isEmpty() || documentActionId.isEmpty() || !EnsureTables()){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":Id"), Text(documentActionId));
	bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));
	bindValues.insert(QStringLiteral(":State"), Text(CDocumentActionJournalQueries::GetStateId(state)));

	QSqlQuery sqlQuery;
	if (!ExecQuery(QStringLiteral(R"(UPDATE "DocumentActions" SET "State" = :State WHERE "Id" = :Id AND "TenantId" = :TenantId)"), bindValues, sqlQuery)){
		return false;
	}

	return sqlQuery.numRowsAffected() > 0;
}


bool CDocumentActionJournalComp::GetDocumentActionInfo(const QByteArray& tenantId, const QByteArray& documentActionId, DocumentActionInfo& info) const
{
	info = DocumentActionInfo();

	if (tenantId.isEmpty() || documentActionId.isEmpty() || !const_cast<CDocumentActionJournalComp*>(this)->EnsureTables()){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":Id"), Text(documentActionId));
	bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));

	QSqlQuery sqlQuery;
	const QString query = QStringLiteral(
				R"(SELECT cs.*, (SELECT COUNT(*) FROM "DocumentActionEntries" e WHERE e."DocumentActionId" = cs."Id") AS "EntryCount" )"
				R"(FROM "DocumentActions" cs WHERE cs."Id" = :Id AND cs."TenantId" = :TenantId)");
	if (!ExecQuery(query, bindValues, sqlQuery) || !sqlQuery.next()){
		return false;
	}

	return ReadDocumentAction(sqlQuery, info);
}


QList<imtdb::IDocumentActionJournal::DocumentActionInfo> CDocumentActionJournalComp::GetDocumentActions(const QByteArray& tenantId, const QByteArray& ownerId, int maxCount) const
{
	QList<DocumentActionInfo> retVal;

	if (tenantId.isEmpty() || !const_cast<CDocumentActionJournalComp*>(this)->EnsureTables()){
		return retVal;
	}

	const int count = qBound(1, maxCount, int(MAX_CHANGE_SETS));

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));

	QString condition = QStringLiteral(R"(cs."TenantId" = :TenantId)");
	if (!ownerId.isEmpty()){
		condition += QStringLiteral(R"( AND cs."OwnerId" = :OwnerId)");
		bindValues.insert(QStringLiteral(":OwnerId"), Text(ownerId));
	}

	QSqlQuery sqlQuery;
	const QString query = QStringLiteral(
				R"(SELECT cs.*, (SELECT COUNT(*) FROM "DocumentActionEntries" e WHERE e."DocumentActionId" = cs."Id") AS "EntryCount" )"
				R"(FROM "DocumentActions" cs WHERE %1 ORDER BY cs."CreatedAt" DESC LIMIT %2)").arg(condition).arg(count);
	if (!ExecQuery(query, bindValues, sqlQuery)){
		return retVal;
	}

	while (sqlQuery.next()){
		DocumentActionInfo info;
		if (ReadDocumentAction(sqlQuery, info)){
			retVal.append(info);
		}
	}

	return retVal;
}


bool CDocumentActionJournalComp::GetEntries(const QByteArray& tenantId, const QByteArray& documentActionId, QList<Entry>& entries) const
{
	entries.clear();

	DocumentActionInfo info;
	if (!GetDocumentActionInfo(tenantId, documentActionId, info)){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":DocumentActionId"), Text(documentActionId));

	QSqlQuery sqlQuery;
	const QString query = QStringLiteral(
				R"(SELECT "TableSchema", "TableName", "DocumentId", "Operation", "RevisionBefore", "RevisionAfter" )"
				R"(FROM "DocumentActionEntries" WHERE "DocumentActionId" = :DocumentActionId ORDER BY "Position")");
	if (!ExecQuery(query, bindValues, sqlQuery)){
		return false;
	}

	while (sqlQuery.next()){
		Entry entry;
		entry.tableSchema = sqlQuery.value(0).toString().toUtf8();
		entry.tableName = sqlQuery.value(1).toString().toUtf8();
		entry.documentId = sqlQuery.value(2).toString().toUtf8();
		if (!CDocumentActionJournalQueries::GetOperationFromId(sqlQuery.value(3).toString().toUtf8(), entry.operation)){
			entry.operation = EO_UNRECORDED;
		}
		entry.revisionBefore = sqlQuery.value(4).isNull() ? -1 : sqlQuery.value(4).toLongLong();
		entry.revisionAfter = sqlQuery.value(5).isNull() ? -1 : sqlQuery.value(5).toLongLong();

		entries.append(entry);
	}

	return true;
}


bool CDocumentActionJournalComp::GetDocumentState(
			const QByteArray& tableSchema,
			const QByteArray& tableName,
			const QByteArray& documentId,
			DocumentState& state) const
{
	state = DocumentState();

	if (!m_jsonXPathExtractorCompPtr.IsValid() || documentId.isEmpty() || !IsIdentifier(tableName) || (!tableSchema.isEmpty() && !IsIdentifier(tableSchema))){
		return false;
	}

	const QString revisionExpression = m_jsonXPathExtractorCompPtr->ExtractXPath(QStringLiteral("RevisionInfo"), QStringLiteral("RevisionNumber"), QMetaType::Int);

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":DocumentId"), Text(documentId));

	QSqlQuery sqlQuery;
	const QString query = QStringLiteral(
				R"(SELECT %1 AS "Revision", "State" FROM %2 WHERE "DocumentId" = :DocumentId AND "State" IN ('Active', 'Disabled') )"
				R"(ORDER BY "Revision" DESC LIMIT 1)")
				.arg(revisionExpression, CDocumentActionJournalQueries::GetQualifiedTableName(tableSchema, tableName));
	if (!ExecQuery(query, bindValues, sqlQuery)){
		return false;
	}

	if (sqlQuery.next()){
		state.exists = true;
		state.revision = sqlQuery.value(0).toLongLong();
		state.isRemoved = (sqlQuery.value(1).toString() == QStringLiteral("Disabled"));
	}

	return true;
}


// private methods

bool CDocumentActionJournalComp::EnsureTables()
{
	QMutexLocker locker(&m_tablesMutex);

	if (m_areTablesCreated){
		return true;
	}

	if (!m_databaseEngineCompPtr.IsValid()){
		SendCriticalMessage(0, QStringLiteral("Invalid component configuration: Database engine missing"), "DocumentActionJournal");

		return false;
	}

	QFile scriptFile(GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateDocumentActionTables.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QStringLiteral("The script '%1' could not be loaded").arg(scriptFile.fileName()), "DocumentActionJournal");

		return false;
	}

	const QList<QByteArray> statements = scriptFile.readAll().split(';');
	for (const QByteArray& statement: statements){
		const QByteArray trimmed = statement.trimmed();
		if (trimmed.isEmpty()){
			continue;
		}

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(trimmed, &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, QStringLiteral("The tables of the document action journal could not be created (error %1)").arg(sqlError.nativeErrorCode()), "DocumentActionJournal");

			return false;
		}
	}

	m_areTablesCreated = true;

	return true;
}


void CDocumentActionJournalComp::RemoveExpired()
{
	const int retentionDays = *m_retentionDaysAttrPtr;
	if (retentionDays <= 0){
		return;
	}

	const QDateTime now = QDateTime::currentDateTimeUtc();
	{
		QMutexLocker locker(&m_tablesMutex);

		if (m_lastCleanup.isValid() && (m_lastCleanup.secsTo(now) < 3600)){
			return;
		}

		m_lastCleanup = now;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":CreatedBefore"), TimeToText(now.addDays(-retentionDays)));

	QSqlQuery sqlQuery;
	ExecQuery(
				QStringLiteral(R"(DELETE FROM "DocumentActionEntries" WHERE "DocumentActionId" IN (SELECT "Id" FROM "DocumentActions" WHERE "CreatedAt" < :CreatedBefore))"),
				bindValues,
				sqlQuery);
	ExecQuery(QStringLiteral(R"(DELETE FROM "DocumentActions" WHERE "CreatedAt" < :CreatedBefore)"), bindValues, sqlQuery);
}


bool CDocumentActionJournalComp::ExecQuery(const QString& query, const QVariantMap& bindValues, QSqlQuery& sqlQuery) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	QSqlError sqlError;
	sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), bindValues, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		// Only the code of the error: the text of the database may contain values
		SendErrorMessage(0, QStringLiteral("A statement of the document action journal failed (error %1)").arg(sqlError.nativeErrorCode()), "DocumentActionJournal");

		return false;
	}

	return true;
}


bool CDocumentActionJournalComp::ReadDocumentAction(const QSqlQuery& sqlQuery, DocumentActionInfo& info) const
{
	const QSqlRecord record = sqlQuery.record();

	info.id = record.value(QStringLiteral("Id")).toString().toUtf8();
	info.tenantId = record.value(QStringLiteral("TenantId")).toString().toUtf8();
	info.ownerId = record.value(QStringLiteral("OwnerId")).toString().toUtf8();
	info.origin = record.value(QStringLiteral("Origin")).toString().toUtf8();
	info.summary = record.value(QStringLiteral("Summary")).toString();
	info.referenceId = record.value(QStringLiteral("ReferenceId")).toString().toUtf8();
	info.createdAt = QDateTime::fromString(record.value(QStringLiteral("CreatedAt")).toString(), Qt::ISODateWithMs);
	info.entryCount = record.value(QStringLiteral("EntryCount")).toInt();

	return CDocumentActionJournalQueries::GetStateFromId(record.value(QStringLiteral("State")).toString().toUtf8(), info.state);
}


bool CDocumentActionJournalComp::IsIdentifier(const QByteArray& name)
{
	static const QRegularExpression identifierExpression(QStringLiteral("^[A-Za-z_][A-Za-z0-9_]{0,62}$"));

	return identifierExpression.match(QString::fromUtf8(name)).hasMatch();
}


} // namespace imtdb


