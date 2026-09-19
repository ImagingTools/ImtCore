// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CExternalIdentityDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtauth/IExternalIdentity.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtauthdb
{


static QString GetExternalIdentitySqlResourcePath(const imtdb::IDatabaseEngine& databaseEngine, const QString& fileName)
{
	const QByteArray databaseDriverId = databaseEngine.GetDatabaseDriverId();
	const bool isSqlite = databaseDriverId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString prefix = isSqlite ? QStringLiteral(":/SQL/SQLite/") : QStringLiteral(":/SQL/Postgres/");
	return prefix + fileName;
}


istd::IChangeableUniquePtr CExternalIdentityDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_identityFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::IExternalIdentity> identityPtr = m_identityFactCompPtr.CreateInstance();
	if (!identityPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		identityPtr->SetId(record.value("Id").toByteArray());
	}

	if (record.contains("UserId")){
		identityPtr->SetUserId(record.value("UserId").toByteArray());
	}

	if (record.contains("Provider")){
		identityPtr->SetProvider(record.value("Provider").toByteArray());
	}

	if (record.contains("ExternalSubject")){
		identityPtr->SetExternalSubject(record.value("ExternalSubject").toString());
	}

	if (record.contains("ExternalEmail")){
		identityPtr->SetExternalEmail(record.value("ExternalEmail").toString());
	}

	if (record.contains("LinkedAt")){
		QDateTime linkedAt = QDateTime::fromString(record.value("LinkedAt").toString(), Qt::ISODate);
		if (linkedAt.isValid()){
			linkedAt = linkedAt.toUTC();
		}
		identityPtr->SetLinkedAt(linkedAt);
	}

	if (record.contains("LastAuthAt")){
		QDateTime lastAuthAt = QDateTime::fromString(record.value("LastAuthAt").toString(), Qt::ISODate);
		if (lastAuthAt.isValid()){
			lastAuthAt = lastAuthAt.toUTC();
		}
		identityPtr->SetLastAuthAt(lastAuthAt);
	}

	return identityPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CExternalIdentityDbDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || (typeId != "ExternalIdentity")){
		return NewObjectQuery();
	}

	const imtauth::IExternalIdentity* identityPtr = dynamic_cast<const imtauth::IExternalIdentity*>(valuePtr);
	if (identityPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray identityId = proposedObjectId;
	if (identityId.isEmpty()){
		identityId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString linkedAt = identityPtr->GetLinkedAt().isValid()
		? identityPtr->GetLinkedAt().toUTC().toString(Qt::ISODate)
		: QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

	QString lastAuthAt;
	if (identityPtr->GetLastAuthAt().isValid()){
		lastAuthAt = QString("'%1'").arg(identityPtr->GetLastAuthAt().toUTC().toString(Qt::ISODate));
	}
	else{
		lastAuthAt = QStringLiteral("NULL");
	}

	NewObjectQuery retVal;
	retVal.query = QString(
			"INSERT INTO \"ExternalIdentities\""
			"(\"Id\", \"UserId\", \"Provider\", \"ExternalSubject\", \"ExternalEmail\", \"LinkedAt\", \"LastAuthAt\") "
			"VALUES('%1', '%2', '%3', '%4', '%5', '%6', %7);")
		.arg(imtdb::SqlEncode(QString::fromUtf8(identityId)))
		.arg(imtdb::SqlEncode(QString::fromUtf8(identityPtr->GetUserId())))
		.arg(imtdb::SqlEncode(QString::fromUtf8(identityPtr->GetProvider())))
		.arg(imtdb::SqlEncode(identityPtr->GetExternalSubject()))
		.arg(imtdb::SqlEncode(identityPtr->GetExternalEmail()))
		.arg(linkedAt)
		.arg(lastAuthAt)
		.toUtf8();

	return retVal;
}


QByteArray CExternalIdentityDbDelegateComp::CreateUpdateObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& objectId,
	const istd::IChangeable& object,
	const imtbase::IOperationContext* /*operationContextPtr*/,
	bool /*useExternDelegate*/) const
{
	const imtauth::IExternalIdentity* identityPtr = dynamic_cast<const imtauth::IExternalIdentity*>(&object);
	if (identityPtr == nullptr){
		return QByteArray();
	}

	QString lastAuthAt;
	if (identityPtr->GetLastAuthAt().isValid()){
		lastAuthAt = QString("'%1'").arg(identityPtr->GetLastAuthAt().toUTC().toString(Qt::ISODate));
	}
	else{
		lastAuthAt = QStringLiteral("NULL");
	}

	return QString(
		"UPDATE \"ExternalIdentities\" SET "
		"\"UserId\"='%1', "
		"\"Provider\"='%2', "
		"\"ExternalSubject\"='%3', "
		"\"ExternalEmail\"='%4', "
		"\"LastAuthAt\"=%5 "
		"WHERE \"Id\"='%6';")
		.arg(imtdb::SqlEncode(QString::fromUtf8(identityPtr->GetUserId())))
		.arg(imtdb::SqlEncode(QString::fromUtf8(identityPtr->GetProvider())))
		.arg(imtdb::SqlEncode(identityPtr->GetExternalSubject()))
		.arg(imtdb::SqlEncode(identityPtr->GetExternalEmail()))
		.arg(lastAuthAt)
		.arg(imtdb::SqlEncode(QString::fromUtf8(objectId)))
		.toUtf8();
}


QByteArray CExternalIdentityDbDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArrayList& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QString idsStr;
	for (int i = 0; i < objectIds.size(); ++i){
		if (i > 0) idsStr += ", ";
		idsStr += QString("'%1'").arg(imtdb::SqlEncode(QString::fromUtf8(objectIds[i])));
	}

	return QString("DELETE FROM \"ExternalIdentities\" WHERE \"Id\" IN (%1);").arg(idsStr).toUtf8();
}


QByteArray CExternalIdentityDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CExternalIdentityDbDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CExternalIdentityDbDelegateComp::CreateDescriptionObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& /*objectId*/,
	const QString& /*description*/,
	const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


// reimplemented (icomp::CComponentBase)

void CExternalIdentityDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_databaseEngineCompPtr.IsValid()){
		QString tableName = GetTableName();
		if (!TableExists(tableName)){
			QFile scriptFile(GetExternalIdentitySqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateExternalIdentitiesTable.sql")));
			if (!scriptFile.open(QFile::ReadOnly)){
				SendErrorMessage(0, QObject::tr("External identities table creation script '%1' could not be loaded").arg(scriptFile.fileName()));
				return;
			}

			QByteArray createTableQuery = scriptFile.readAll();
			scriptFile.close();

			createTableQuery.replace("${TableName}", tableName.toUtf8());
			createTableQuery.replace("${TableScheme}", "public");

			QSqlError sqlError;
			m_databaseEngineCompPtr->ExecSqlQuery(createTableQuery, &sqlError);

			if (sqlError.type() != QSqlError::NoError){
				qCritical() << __FILE__ << __LINE__
							<< "\n\t| External Identities table could not be created"
							<< "\n\t| Error: " << sqlError
							<< "\n\t| Query: " << createTableQuery;

				SendErrorMessage(0, QObject::tr("\n\t| External Identities table could not be created"
														"\n\t| Error: %1"
														"\n\t| Query: %2")
													.arg(sqlError.text(), qPrintable(createTableQuery)));
			}
		}
	}
}


} // namespace imtauthdb
