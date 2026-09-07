// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/COidcClientDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtauth/IOidcClient.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtauthdb
{


static QString GetOidcSqlResourcePath(const imtdb::IDatabaseEngine& databaseEngine, const QString& fileName)
{
	const QByteArray databaseDriverId = databaseEngine.GetDatabaseDriverId();
	const bool isSqlite = databaseDriverId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString prefix = isSqlite ? QStringLiteral(":/SQL/SQLite/") : QStringLiteral(":/SQL/Postgres/");
	return prefix + fileName;
}


istd::IChangeableUniquePtr COidcClientDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_clientFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::IOidcClient> clientPtr = m_clientFactCompPtr.CreateInstance();
	if (!clientPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		clientPtr->SetClientId(record.value("Id").toByteArray());
	}

	if (record.contains("ClientSecretHash")){
		clientPtr->SetClientSecretHash(record.value("ClientSecretHash").toByteArray());
	}

	if (record.contains("ClientName")){
		clientPtr->SetClientName(record.value("ClientName").toString());
	}

	if (record.contains("RedirectUris")){
		QString urisStr = record.value("RedirectUris").toString();
		QStringList uris;
		if (!urisStr.isEmpty()){
			uris = urisStr.split(',');
		}
		clientPtr->SetRedirectUris(uris);
	}

	if (record.contains("GrantTypes")){
		QString grantTypesStr = record.value("GrantTypes").toString();
		QByteArrayList grantTypes;
		if (!grantTypesStr.isEmpty()){
			QStringList list = grantTypesStr.split(',');
			for (const QString& s : ::std::as_const(list)){
				grantTypes.append(s.toUtf8());
			}
		}
		clientPtr->SetGrantTypes(grantTypes);
	}

	if (record.contains("Scopes")){
		QString scopesStr = record.value("Scopes").toString();
		QByteArrayList scopes;
		if (!scopesStr.isEmpty()){
			QStringList list = scopesStr.split(',');
			for (const QString& s : ::std::as_const(list)){
				scopes.append(s.toUtf8());
			}
		}
		clientPtr->SetScopes(scopes);
	}

	if (record.contains("CreatedAt")){
		QDateTime createdAt = QDateTime::fromString(record.value("CreatedAt").toString(), Qt::ISODate);
		if (createdAt.isValid()){
			createdAt = createdAt.toUTC();
		}
		clientPtr->SetCreatedAt(createdAt);
	}

	return clientPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery COidcClientDbDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || (typeId != "OidcClient")){
		return NewObjectQuery();
	}

	const imtauth::IOidcClient* clientPtr = dynamic_cast<const imtauth::IOidcClient*>(valuePtr);
	if (clientPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray clientId = proposedObjectId;
	if (clientId.isEmpty()){
		clientId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString redirectUrisStr;
	QStringList uris = clientPtr->GetRedirectUris();
	redirectUrisStr = uris.join(',');

	QString grantTypesStr;
	QByteArrayList grantTypes = clientPtr->GetGrantTypes();
	for (int i = 0; i < grantTypes.size(); ++i){
		if (i > 0) grantTypesStr += ",";
		grantTypesStr += QString::fromUtf8(grantTypes[i]);
	}

	QString scopesStr;
	QByteArrayList scopes = clientPtr->GetScopes();
	for (int i = 0; i < scopes.size(); ++i){
		if (i > 0) scopesStr += ",";
		scopesStr += QString::fromUtf8(scopes[i]);
	}

	QString createdAt = clientPtr->GetCreatedAt().isValid()
		? clientPtr->GetCreatedAt().toUTC().toString(Qt::ISODate)
		: QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

	NewObjectQuery retVal;
	retVal.query = QString(
			"INSERT INTO \"OidcClients\""
			"(\"Id\", \"ClientSecretHash\", \"ClientName\", \"RedirectUris\", \"GrantTypes\", \"Scopes\", \"CreatedAt\") "
			"VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7');")
		.arg(imtdb::SqlEncode(QString::fromUtf8(clientId)))
		.arg(imtdb::SqlEncode(QString::fromUtf8(clientPtr->GetClientSecretHash())))
		.arg(imtdb::SqlEncode(clientPtr->GetClientName()))
		.arg(imtdb::SqlEncode(redirectUrisStr))
		.arg(imtdb::SqlEncode(grantTypesStr))
		.arg(imtdb::SqlEncode(scopesStr))
		.arg(createdAt)
		.toUtf8();

	return retVal;
}


QByteArray COidcClientDbDelegateComp::CreateUpdateObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& objectId,
	const istd::IChangeable& object,
	const imtbase::IOperationContext* /*operationContextPtr*/,
	bool /*useExternDelegate*/) const
{
	const imtauth::IOidcClient* clientPtr = dynamic_cast<const imtauth::IOidcClient*>(&object);
	if (clientPtr == nullptr){
		return QByteArray();
	}

	QString redirectUrisStr = clientPtr->GetRedirectUris().join(',');

	QString grantTypesStr;
	QByteArrayList grantTypes = clientPtr->GetGrantTypes();
	for (int i = 0; i < grantTypes.size(); ++i){
		if (i > 0) grantTypesStr += ",";
		grantTypesStr += QString::fromUtf8(grantTypes[i]);
	}

	QString scopesStr;
	QByteArrayList scopes = clientPtr->GetScopes();
	for (int i = 0; i < scopes.size(); ++i){
		if (i > 0) scopesStr += ",";
		scopesStr += QString::fromUtf8(scopes[i]);
	}

	return QString(
		"UPDATE \"OidcClients\" SET "
		"\"ClientSecretHash\"='%1', "
		"\"ClientName\"='%2', "
		"\"RedirectUris\"='%3', "
		"\"GrantTypes\"='%4', "
		"\"Scopes\"='%5' "
		"WHERE \"Id\"='%6';")
		.arg(imtdb::SqlEncode(QString::fromUtf8(clientPtr->GetClientSecretHash())))
		.arg(imtdb::SqlEncode(clientPtr->GetClientName()))
		.arg(imtdb::SqlEncode(redirectUrisStr))
		.arg(imtdb::SqlEncode(grantTypesStr))
		.arg(imtdb::SqlEncode(scopesStr))
		.arg(imtdb::SqlEncode(QString::fromUtf8(objectId)))
		.toUtf8();
}


QByteArray COidcClientDbDelegateComp::CreateDeleteObjectsQuery(
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

	return QString("DELETE FROM \"OidcClients\" WHERE \"Id\" IN (%1);").arg(idsStr).toUtf8();
}


QByteArray COidcClientDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray COidcClientDbDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& newObjectName,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectId.isEmpty() || newObjectName.isEmpty()){
		return QByteArray();
	}

	return QString(
			"UPDATE \"OidcClients\" SET "
			"\"ClientName\"='%1' "
			"WHERE \"Id\"='%2';")
		.arg(imtdb::SqlEncode(newObjectName))
		.arg(imtdb::SqlEncode(QString::fromUtf8(objectId)))
		.toUtf8();
}


QByteArray COidcClientDbDelegateComp::CreateDescriptionObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& /*objectId*/,
	const QString& /*description*/,
	const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


// reimplemented (icomp::CComponentBase)

void COidcClientDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_databaseEngineCompPtr.IsValid()){
		QString tableName = GetTableName();
		if (!TableExists(tableName)){
			QFile scriptFile(GetOidcSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateOidcClientsTable.sql")));
			if (!scriptFile.open(QFile::ReadOnly)){
				SendErrorMessage(0, QObject::tr("OIDC clients table creation script '%1' could not be loaded").arg(scriptFile.fileName()));
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
							<< "\n\t| OIDC Clients table could not be created"
							<< "\n\t| Error: " << sqlError
							<< "\n\t| Query: " << createTableQuery;

				SendErrorMessage(0, QObject::tr("\n\t| OIDC Clients table could not be created"
														"\n\t| Error: %1"
														"\n\t| Query: %2")
													.arg(sqlError.text(), qPrintable(createTableQuery)));
			}
		}
	}
}


} // namespace imtauthdb
