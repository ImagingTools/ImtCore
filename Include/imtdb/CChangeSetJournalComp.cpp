// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CChangeSetJournalComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CChangeSetJournalQueries.h>
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


// reimplemented (imtbase::IChangeSetJournal)

QByteArray CChangeSetJournalComp::BeginChangeSet(
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

	const QByteArray changeSetId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":Id"), Text(changeSetId));
	bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));
	bindValues.insert(QStringLiteral(":OwnerId"), Text(ownerId));
	bindValues.insert(QStringLiteral(":Origin"), Text(origin));
	bindValues.insert(QStringLiteral(":Summary"), summary.left(MAX_SUMMARY_LENGTH));
	bindValues.insert(QStringLiteral(":ReferenceId"), Text(referenceId));
	bindValues.insert(QStringLiteral(":State"), Text(CChangeSetJournalQueries::GetStateId(CSS_APPLYING)));
	bindValues.insert(QStringLiteral(":CreatedAt"), TimeToText(QDateTime::currentDateTimeUtc()));

	QSqlQuery sqlQuery;
	const QString query = QStringLiteral(
				R"(INSERT INTO "ChangeSets" ("Id", "TenantId", "OwnerId", "Origin", "Summary", "ReferenceId", "State", "CreatedAt") )"
				R"(VALUES (:Id, :TenantId, :OwnerId, :Origin, :Summary, :ReferenceId, :State, :CreatedAt))");
	if (!ExecQuery(query, bindValues, sqlQuery)){
		return QByteArray();
	}

	return changeSetId;
}


bool CChangeSetJournalComp::SetChangeSetState(const QByteArray& tenantId, const QByteArray& changeSetId, ChangeSetState state)
{
	if (tenantId.isEmpty() || changeSetId.isEmpty() || !EnsureTables()){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":Id"), Text(changeSetId));
	bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));
	bindValues.insert(QStringLiteral(":State"), Text(CChangeSetJournalQueries::GetStateId(state)));

	QSqlQuery sqlQuery;
	if (!ExecQuery(QStringLiteral(R"(UPDATE "ChangeSets" SET "State" = :State WHERE "Id" = :Id AND "TenantId" = :TenantId)"), bindValues, sqlQuery)){
		return false;
	}

	return sqlQuery.numRowsAffected() > 0;
}


bool CChangeSetJournalComp::GetChangeSetInfo(const QByteArray& tenantId, const QByteArray& changeSetId, ChangeSetInfo& info) const
{
	info = ChangeSetInfo();

	if (tenantId.isEmpty() || changeSetId.isEmpty() || !const_cast<CChangeSetJournalComp*>(this)->EnsureTables()){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":Id"), Text(changeSetId));
	bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));

	QSqlQuery sqlQuery;
	const QString query = QStringLiteral(
				R"(SELECT cs.*, (SELECT COUNT(*) FROM "ChangeSetEntries" e WHERE e."ChangeSetId" = cs."Id") AS "EntryCount" )"
				R"(FROM "ChangeSets" cs WHERE cs."Id" = :Id AND cs."TenantId" = :TenantId)");
	if (!ExecQuery(query, bindValues, sqlQuery) || !sqlQuery.next()){
		return false;
	}

	return ReadChangeSet(sqlQuery, info);
}


QList<imtbase::IChangeSetJournal::ChangeSetInfo> CChangeSetJournalComp::GetChangeSets(const QByteArray& tenantId, const QByteArray& ownerId, int maxCount) const
{
	QList<ChangeSetInfo> retVal;

	if (tenantId.isEmpty() || !const_cast<CChangeSetJournalComp*>(this)->EnsureTables()){
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
				R"(SELECT cs.*, (SELECT COUNT(*) FROM "ChangeSetEntries" e WHERE e."ChangeSetId" = cs."Id") AS "EntryCount" )"
				R"(FROM "ChangeSets" cs WHERE %1 ORDER BY cs."CreatedAt" DESC LIMIT %2)").arg(condition).arg(count);
	if (!ExecQuery(query, bindValues, sqlQuery)){
		return retVal;
	}

	while (sqlQuery.next()){
		ChangeSetInfo info;
		if (ReadChangeSet(sqlQuery, info)){
			retVal.append(info);
		}
	}

	return retVal;
}


bool CChangeSetJournalComp::GetEntries(const QByteArray& tenantId, const QByteArray& changeSetId, QList<Entry>& entries) const
{
	entries.clear();

	ChangeSetInfo info;
	if (!GetChangeSetInfo(tenantId, changeSetId, info)){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":ChangeSetId"), Text(changeSetId));

	QSqlQuery sqlQuery;
	const QString query = QStringLiteral(
				R"(SELECT "TableSchema", "TableName", "DocumentId", "Operation", "RevisionBefore", "RevisionAfter" )"
				R"(FROM "ChangeSetEntries" WHERE "ChangeSetId" = :ChangeSetId ORDER BY "Position")");
	if (!ExecQuery(query, bindValues, sqlQuery)){
		return false;
	}

	while (sqlQuery.next()){
		Entry entry;
		entry.tableSchema = sqlQuery.value(0).toString().toUtf8();
		entry.tableName = sqlQuery.value(1).toString().toUtf8();
		entry.documentId = sqlQuery.value(2).toString().toUtf8();
		if (!CChangeSetJournalQueries::GetOperationFromId(sqlQuery.value(3).toString().toUtf8(), entry.operation)){
			entry.operation = EO_UNRECORDED;
		}
		entry.revisionBefore = sqlQuery.value(4).isNull() ? -1 : sqlQuery.value(4).toLongLong();
		entry.revisionAfter = sqlQuery.value(5).isNull() ? -1 : sqlQuery.value(5).toLongLong();

		entries.append(entry);
	}

	return true;
}


bool CChangeSetJournalComp::GetDocumentState(
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
				.arg(revisionExpression, CChangeSetJournalQueries::GetQualifiedTableName(tableSchema, tableName));
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

bool CChangeSetJournalComp::EnsureTables()
{
	QMutexLocker locker(&m_tablesMutex);

	if (m_areTablesCreated){
		return true;
	}

	if (!m_databaseEngineCompPtr.IsValid()){
		SendCriticalMessage(0, QStringLiteral("Invalid component configuration: Database engine missing"), "ChangeSetJournal");

		return false;
	}

	QFile scriptFile(GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateChangeSetTables.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QStringLiteral("The script '%1' could not be loaded").arg(scriptFile.fileName()), "ChangeSetJournal");

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
			SendErrorMessage(0, QStringLiteral("The tables of the change set journal could not be created (error %1)").arg(sqlError.nativeErrorCode()), "ChangeSetJournal");

			return false;
		}
	}

	m_areTablesCreated = true;

	return true;
}


void CChangeSetJournalComp::RemoveExpired()
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
				QStringLiteral(R"(DELETE FROM "ChangeSetEntries" WHERE "ChangeSetId" IN (SELECT "Id" FROM "ChangeSets" WHERE "CreatedAt" < :CreatedBefore))"),
				bindValues,
				sqlQuery);
	ExecQuery(QStringLiteral(R"(DELETE FROM "ChangeSets" WHERE "CreatedAt" < :CreatedBefore)"), bindValues, sqlQuery);
}


bool CChangeSetJournalComp::ExecQuery(const QString& query, const QVariantMap& bindValues, QSqlQuery& sqlQuery) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	QSqlError sqlError;
	sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), bindValues, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		// Only the code of the error: the text of the database may contain values
		SendErrorMessage(0, QStringLiteral("A statement of the change set journal failed (error %1)").arg(sqlError.nativeErrorCode()), "ChangeSetJournal");

		return false;
	}

	return true;
}


bool CChangeSetJournalComp::ReadChangeSet(const QSqlQuery& sqlQuery, ChangeSetInfo& info) const
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

	return CChangeSetJournalQueries::GetStateFromId(record.value(QStringLiteral("State")).toString().toUtf8(), info.state);
}


bool CChangeSetJournalComp::IsIdentifier(const QByteArray& name)
{
	static const QRegularExpression identifierExpression(QStringLiteral("^[A-Za-z_][A-Za-z0-9_]{0,62}$"));

	return identifierExpression.match(QString::fromUtf8(name)).hasMatch();
}


} // namespace imtdb


