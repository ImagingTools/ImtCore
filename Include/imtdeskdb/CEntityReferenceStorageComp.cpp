// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskdb/CEntityReferenceStorageComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtdeskdb
{


// reimplemented (imtdesk::IEntityReferenceStorage)

QByteArray CEntityReferenceStorageComp::CreateEntityReference(
		const QString& entityType,
		const QByteArray& entityId,
		const QString& displayName,
		const QString& entityUrl)
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return QByteArray();
	}

	QString refId = QUuid::createUuid().toString(QUuid::WithoutBraces);
	QString nowUtc = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	auto sqlEscape = [](const QString& s) -> QString {
		QString escaped = s;
		escaped.replace('\'', "''");
		return escaped;
	};

	QByteArray insertQuery = QString(
		"INSERT INTO \"EntityReferences\" "
		"(\"Id\", \"EntityType\", \"EntityId\", \"DisplayName\", \"EntityUrl\", \"CreatedAt\") "
		"VALUES('%1', '%2', '%3', '%4', '%5', '%6');")
		.arg(refId,
			 sqlEscape(entityType),
			 sqlEscape(QString::fromUtf8(entityId)),
			 sqlEscape(displayName),
			 sqlEscape(entityUrl),
			 nowUtc).toUtf8();

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(insertQuery, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qWarning() << "CEntityReferenceStorageComp: failed to create entity reference:" << sqlError.text();
		return QByteArray();
	}

	return refId.toUtf8();
}


bool CEntityReferenceStorageComp::GetEntityReference(
		const QByteArray& refId,
		QString& entityType,
		QByteArray& entityId,
		QString& displayName,
		QString& entityUrl) const
{
	if (!m_databaseEngineCompPtr.IsValid() || refId.isEmpty()){
		return false;
	}

	QString escaped = QString::fromUtf8(refId);
	escaped.replace('\'', "''");
	QByteArray selectQuery = QString(
		"SELECT \"EntityType\", \"EntityId\", \"DisplayName\", \"EntityUrl\" "
		"FROM \"EntityReferences\" WHERE \"Id\"='%1';")
		.arg(escaped).toUtf8();

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectQuery, &sqlError);

	if (sqlError.type() != QSqlError::NoError || !sqlQuery.next()){
		return false;
	}

	QSqlRecord r = sqlQuery.record();
	entityType = r.value("EntityType").toString();
	entityId = imtdb::VariantToByteArray(r.value("EntityId"));
	displayName = r.value("DisplayName").toString();
	entityUrl = r.value("EntityUrl").toString();

	return true;
}


// reimplemented (icomp::CComponentBase)

void CEntityReferenceStorageComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	// Create EntityReferences table if it does not exist
	QFile scriptFile(imtdb::GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateEntityReferencesTable.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		qWarning() << "CEntityReferenceStorageComp: EntityReferences table creation script not found:" << scriptFile.fileName();
		return;
	}

	QByteArray query = scriptFile.readAll();
	scriptFile.close();
	query.replace("${TableScheme}", "public");

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qCritical() << "CEntityReferenceStorageComp: EntityReferences table could not be created:"
					<< sqlError.text() << "\n\tQuery:" << query;
	}
}


} // namespace imtdeskdb
