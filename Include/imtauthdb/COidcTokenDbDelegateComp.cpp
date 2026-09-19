// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/COidcTokenDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtauth/IOidcTokenInfo.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtauthdb
{


static QString GetOidcTokenSqlResourcePath(const imtdb::IDatabaseEngine& databaseEngine, const QString& fileName)
{
	const QByteArray databaseDriverId = databaseEngine.GetDatabaseDriverId();
	const bool isSqlite = databaseDriverId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString prefix = isSqlite ? QStringLiteral(":/SQL/SQLite/") : QStringLiteral(":/SQL/Postgres/");
	return prefix + fileName;
}


istd::IChangeableUniquePtr COidcTokenDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_tokenFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::IOidcTokenInfo> tokenPtr = m_tokenFactCompPtr.CreateInstance();
	if (!tokenPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		tokenPtr->SetTokenId(record.value("Id").toByteArray());
	}

	if (record.contains("ClientId")){
		tokenPtr->SetClientId(record.value("ClientId").toByteArray());
	}

	if (record.contains("UserId")){
		tokenPtr->SetUserId(record.value("UserId").toByteArray());
	}

	if (record.contains("Scope")){
		tokenPtr->SetScope(record.value("Scope").toString());
	}

	if (record.contains("TokenType")){
		QString typeStr = record.value("TokenType").toString();
		if (typeStr == "refresh"){
			tokenPtr->SetTokenType(imtauth::IOidcTokenInfo::OTT_REFRESH);
		}
		else{
			tokenPtr->SetTokenType(imtauth::IOidcTokenInfo::OTT_ACCESS);
		}
	}

	if (record.contains("ExpiresAt")){
		QDateTime expiresAt = QDateTime::fromString(record.value("ExpiresAt").toString(), Qt::ISODate);
		if (expiresAt.isValid()){
			expiresAt = expiresAt.toUTC();
		}
		tokenPtr->SetExpirationDate(expiresAt);
	}

	if (record.contains("IsRevoked")){
		tokenPtr->SetRevoked(record.value("IsRevoked").toBool());
	}

	if (record.contains("CreatedAt")){
		QDateTime createdAt = QDateTime::fromString(record.value("CreatedAt").toString(), Qt::ISODate);
		if (createdAt.isValid()){
			createdAt = createdAt.toUTC();
		}
		tokenPtr->SetCreatedAt(createdAt);
	}

	return tokenPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery COidcTokenDbDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || (typeId != "OidcToken")){
		return NewObjectQuery();
	}

	const imtauth::IOidcTokenInfo* tokenPtr = dynamic_cast<const imtauth::IOidcTokenInfo*>(valuePtr);
	if (tokenPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray tokenId = proposedObjectId;
	if (tokenId.isEmpty()){
		tokenId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString tokenTypeStr = (tokenPtr->GetTokenType() == imtauth::IOidcTokenInfo::OTT_REFRESH) ? "refresh" : "access";
	QString expiresAt = tokenPtr->GetExpirationDate().toUTC().toString(Qt::ISODate);
	QString createdAt = tokenPtr->GetCreatedAt().isValid()
		? tokenPtr->GetCreatedAt().toUTC().toString(Qt::ISODate)
		: QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
	QString revokedSql = tokenPtr->IsRevoked() ? "TRUE" : "FALSE";
	QString userIdSql = tokenPtr->GetUserId().isEmpty() ? "NULL" : QString("'%1'").arg(imtdb::SqlEncode(QString::fromUtf8(tokenPtr->GetUserId())));

	NewObjectQuery retVal;
	retVal.query = QString(
			"INSERT INTO \"OidcTokens\""
			"(\"Id\", \"ClientId\", \"UserId\", \"Scope\", \"TokenType\", \"ExpiresAt\", \"IsRevoked\", \"CreatedAt\") "
			"VALUES('%1', '%2', %3, '%4', '%5', '%6', %7, '%8');")
		.arg(imtdb::SqlEncode(QString::fromUtf8(tokenId)))
		.arg(imtdb::SqlEncode(QString::fromUtf8(tokenPtr->GetClientId())))
		.arg(userIdSql)
		.arg(imtdb::SqlEncode(tokenPtr->GetScope()))
		.arg(tokenTypeStr)
		.arg(expiresAt)
		.arg(revokedSql)
		.arg(createdAt)
		.toUtf8();

	return retVal;
}


QByteArray COidcTokenDbDelegateComp::CreateUpdateObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& objectId,
	const istd::IChangeable& object,
	const imtbase::IOperationContext* /*operationContextPtr*/,
	bool /*useExternDelegate*/) const
{
	const imtauth::IOidcTokenInfo* tokenPtr = dynamic_cast<const imtauth::IOidcTokenInfo*>(&object);
	if (tokenPtr == nullptr){
		return QByteArray();
	}

	QString revokedSql = tokenPtr->IsRevoked() ? "TRUE" : "FALSE";

	return QString(
		"UPDATE \"OidcTokens\" SET "
		"\"IsRevoked\"=%1 "
		"WHERE \"Id\"='%2';")
		.arg(revokedSql)
		.arg(imtdb::SqlEncode(QString::fromUtf8(objectId)))
		.toUtf8();
}


QByteArray COidcTokenDbDelegateComp::CreateDeleteObjectsQuery(
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

	return QString("DELETE FROM \"OidcTokens\" WHERE \"Id\" IN (%1);").arg(idsStr).toUtf8();
}


QByteArray COidcTokenDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray COidcTokenDbDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray COidcTokenDbDelegateComp::CreateDescriptionObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& /*objectId*/,
	const QString& /*description*/,
	const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


// reimplemented (icomp::CComponentBase)

void COidcTokenDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_databaseEngineCompPtr.IsValid()){
		QString tableName = GetTableName();
		if (!TableExists(tableName)){
			QFile scriptFile(GetOidcTokenSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateOidcTokensTable.sql")));
			if (!scriptFile.open(QFile::ReadOnly)){
				SendErrorMessage(0, QObject::tr("OIDC tokens table creation script '%1' could not be loaded").arg(scriptFile.fileName()));
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
							<< "\n\t| OIDC Tokens table could not be created"
							<< "\n\t| Error: " << sqlError
							<< "\n\t| Query: " << createTableQuery;

				SendErrorMessage(0, QObject::tr("\n\t| OIDC Tokens table could not be created"
														"\n\t| Error: %1"
														"\n\t| Query: %2")
													.arg(sqlError.text(), qPrintable(createTableQuery)));
			}
		}
	}
}


} // namespace imtauthdb
