// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifydb/CNotificationDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtnotifydb
{


namespace
{

QString utcNow()
{
	return QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
}

QString nullableText(const QString& value)
{
	if (value.isEmpty()){
		return QStringLiteral("NULL");
	}
	return QString("'%1'").arg(imtdb::SqlEncode(value));
}

QString nullableId(const QByteArray& value)
{
	if (value.isEmpty()){
		return QStringLiteral("NULL");
	}
	QString escaped = QString::fromUtf8(value);
	escaped.replace('\'', "''");
	return QString("'%1'").arg(escaped);
}

} // anonymous namespace


// reimplemented (icomp::CComponentBase)

void CNotificationDbDelegateComp::OnComponentCreated()
{
	// Creates the "Notifications" table (single CREATE TABLE statement, safe on
	// both engines) via the base class AutoCreateTable / CreateTableScriptPath
	// mechanism.
	BaseClass::OnComponentCreated();

	CreateIndexesIfNeeded();
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CNotificationDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_notificationFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtnotify::INotification> notificationPtr = m_notificationFactCompPtr.CreateInstance();
	if (!notificationPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		notificationPtr->SetId(imtdb::VariantToByteArray(record.value("Id")));
	}
	if (record.contains("RecipientId")){
		notificationPtr->SetRecipientId(imtdb::VariantToByteArray(record.value("RecipientId")));
	}
	if (record.contains("Category")){
		notificationPtr->SetCategory(record.value("Category").toString());
	}
	if (record.contains("Title")){
		notificationPtr->SetTitle(record.value("Title").toString());
	}
	if (record.contains("Body")){
		notificationPtr->SetBody(record.value("Body").toString());
	}
	if (record.contains("IconName")){
		notificationPtr->SetIconName(record.value("IconName").toString());
	}
	if (record.contains("Severity")){
		notificationPtr->SetSeverity(static_cast<imtnotify::INotification::Severity>(record.value("Severity").toInt()));
	}
	if (record.contains("IsRead")){
		notificationPtr->SetRead(record.value("IsRead").toInt() != 0);
	}
	if (record.contains("SourceType")){
		notificationPtr->SetSourceType(record.value("SourceType").toString());
	}
	if (record.contains("SourceId")){
		notificationPtr->SetSourceId(imtdb::VariantToByteArray(record.value("SourceId")));
	}
	if (record.contains("TargetRoute")){
		notificationPtr->SetTargetRoute(record.value("TargetRoute").toString());
	}
	if (record.contains("Metadata")){
		notificationPtr->SetMetadata(record.value("Metadata").toString());
	}
	if (record.contains("CreatedAt")){
		QVariant val = record.value("CreatedAt");
		QDateTime dt = val.toDateTime();
		notificationPtr->SetCreatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	if (record.contains("UpdatedAt")){
		QVariant val = record.value("UpdatedAt");
		QDateTime dt = val.toDateTime();
		notificationPtr->SetUpdatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}

	return notificationPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CNotificationDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& typeId,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || typeId != "Notification"){
		return NewObjectQuery();
	}

	const imtnotify::INotification* notificationPtr = dynamic_cast<const imtnotify::INotification*>(valuePtr);
	if (notificationPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray notificationId = proposedObjectId;
	if (notificationId.isEmpty()){
		notificationId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString createdAt = notificationPtr->GetCreatedAt();
	if (createdAt.isEmpty()){
		createdAt = utcNow();
	}
	QString updatedAt = notificationPtr->GetUpdatedAt();
	if (updatedAt.isEmpty()){
		updatedAt = createdAt;
	}

	const QString query = QString(
		"INSERT INTO \"Notifications\" "
		"(\"Id\", \"RecipientId\", \"Category\", \"Title\", \"Body\", \"IconName\", \"Severity\", "
		"\"IsRead\", \"SourceType\", \"SourceId\", \"TargetRoute\", \"Metadata\", \"CreatedAt\", \"UpdatedAt\") "
		"VALUES('%1', '%2', %3, '%4', %5, %6, %7, %8, %9, %10, %11, %12, '%13', '%14');")
		.arg(QString::fromUtf8(notificationId))
		.arg(QString::fromUtf8(notificationPtr->GetRecipientId()))
		.arg(nullableText(notificationPtr->GetCategory()))
		.arg(imtdb::SqlEncode(notificationPtr->GetTitle()))
		.arg(nullableText(notificationPtr->GetBody()))
		.arg(nullableText(notificationPtr->GetIconName()))
		.arg(static_cast<int>(notificationPtr->GetSeverity()))
		.arg(notificationPtr->IsRead() ? 1 : 0)
		.arg(nullableText(notificationPtr->GetSourceType()))
		.arg(nullableId(notificationPtr->GetSourceId()))
		.arg(nullableText(notificationPtr->GetTargetRoute()))
		.arg(nullableText(notificationPtr->GetMetadata()))
		.arg(createdAt)
		.arg(updatedAt);

	NewObjectQuery retVal;
	retVal.query = query.toUtf8();

	return retVal;
}


QByteArray CNotificationDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtnotify::INotification* notificationPtr = dynamic_cast<const imtnotify::INotification*>(&object);
	if (notificationPtr == nullptr){
		return QByteArray();
	}

	QString escapedId = QString::fromUtf8(objectId);
	escapedId.replace('\'', "''");

	QString updatedAt = notificationPtr->GetUpdatedAt();
	if (updatedAt.isEmpty()){
		updatedAt = utcNow();
	}

	return QString(
		"UPDATE \"Notifications\" SET "
		"\"Category\"=%1, "
		"\"Title\"='%2', "
		"\"Body\"=%3, "
		"\"IconName\"=%4, "
		"\"Severity\"=%5, "
		"\"IsRead\"=%6, "
		"\"SourceType\"=%7, "
		"\"SourceId\"=%8, "
		"\"TargetRoute\"=%9, "
		"\"Metadata\"=%10, "
		"\"UpdatedAt\"='%11' "
		"WHERE \"Id\"='%12';")
		.arg(nullableText(notificationPtr->GetCategory()))
		.arg(imtdb::SqlEncode(notificationPtr->GetTitle()))
		.arg(nullableText(notificationPtr->GetBody()))
		.arg(nullableText(notificationPtr->GetIconName()))
		.arg(static_cast<int>(notificationPtr->GetSeverity()))
		.arg(notificationPtr->IsRead() ? 1 : 0)
		.arg(nullableText(notificationPtr->GetSourceType()))
		.arg(nullableId(notificationPtr->GetSourceId()))
		.arg(nullableText(notificationPtr->GetTargetRoute()))
		.arg(nullableText(notificationPtr->GetMetadata()))
		.arg(updatedAt)
		.arg(escapedId)
		.toUtf8();
}


QByteArray CNotificationDbDelegateComp::CreateDeleteObjectsQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArrayList& objectIds,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QString idsStr;
	for (int i = 0; i < objectIds.size(); ++i){
		if (i > 0){
			idsStr += ", ";
		}
		QString escaped = QString::fromUtf8(objectIds[i]);
		escaped.replace('\'', "''");
		idsStr += QString("'%1'").arg(escaped);
	}

	return QString("DELETE FROM \"Notifications\" WHERE \"Id\" IN (%1);").arg(idsStr).toUtf8();
}


QByteArray CNotificationDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CNotificationDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CNotificationDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QString CNotificationDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QStringList clauses;

	const iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	// Recipient scoping — the most important server-side filter, keeps every
	// query restricted to a single user's notifications.
	if (paramIds.contains("RecipientId")){
		iprm::TParamsPtr<iprm::IIdParam> recipientParamPtr(&filterParams, "RecipientId");
		if (recipientParamPtr.IsValid()){
			const QByteArray recipientId = recipientParamPtr->GetId();
			if (!recipientId.isEmpty()){
				QString escaped = QString::fromUtf8(recipientId);
				escaped.replace('\'', "''");
				clauses << QString("\"RecipientId\"='%1'").arg(escaped);
			}
		}
	}

	// Category filter (exact match on the producer category).
	if (paramIds.contains("Category")){
		iprm::TParamsPtr<iprm::IIdParam> categoryParamPtr(&filterParams, "Category");
		if (categoryParamPtr.IsValid()){
			const QByteArray category = categoryParamPtr->GetId();
			if (!category.isEmpty()){
				clauses << QString("\"Category\"='%1'").arg(imtdb::SqlEncode(QString::fromUtf8(category)));
			}
		}
	}

	return clauses.join(QStringLiteral(" AND "));
}


// private methods

void CNotificationDbDelegateComp::CreateIndexesIfNeeded() const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	// Mirrors the base class' own AutoCreateTable gate: if schema management is
	// disabled here, indexes are the deployment's responsibility too.
	if (!m_autoCreateTableAttrPtr.IsValid() || !*m_autoCreateTableAttrPtr){
		return;
	}

	const bool isSqlite = m_databaseEngineCompPtr->GetDatabaseDriverId().compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;

	QByteArray tableScheme = GetTableScheme();
	if (tableScheme.isEmpty() && !isSqlite){
		tableScheme = QByteArrayLiteral("public");
	}

	const QString qualifiedTable = isSqlite
				? QString("\"%1\"").arg(QString::fromUtf8(GetTableName()))
				: QString("\"%1\".\"%2\"").arg(QString::fromUtf8(tableScheme), QString::fromUtf8(GetTableName()));

	// One CREATE INDEX per statement, each its own ExecSqlQuery call: the
	// QSQLITE driver only runs the first statement of a ';'-joined batch, so
	// batching these would silently create the table without any index on
	// SQLite. Executing them individually is correct on both engines.
	const QStringList indexStatements = {
		QString("CREATE INDEX IF NOT EXISTS \"IX_Notifications_RecipientId\" ON %1 (\"RecipientId\")").arg(qualifiedTable),
		QString("CREATE INDEX IF NOT EXISTS \"IX_Notifications_CreatedAt\" ON %1 (\"CreatedAt\")").arg(qualifiedTable),
		QString("CREATE INDEX IF NOT EXISTS \"IX_Notifications_RecipientId_IsRead\" ON %1 (\"RecipientId\", \"IsRead\")").arg(qualifiedTable),
		QString("CREATE INDEX IF NOT EXISTS \"IX_Notifications_RecipientId_Category\" ON %1 (\"RecipientId\", \"Category\")").arg(qualifiedTable),
	};

	for (const QString& statement: indexStatements){
		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(statement.toUtf8(), &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, QString("Notification index could not be created: %1 (query: %2)")
										.arg(sqlError.text(), statement));
		}
	}
}


} // namespace imtnotifydb
