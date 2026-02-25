// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CPersonalAccessTokenDatabaseDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/CPersonalAccessTokenMetaInfoCreatorComp.h>
#include <imtauth/IPersonalAccessToken.h>
#include <imtdb/CDatabaseEngineComp.h>


namespace imtauthdb
{


istd::IChangeableUniquePtr CPersonalAccessTokenDatabaseDelegateComp::CreateObjectFromRecord(
	const QSqlRecord& record,
	const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_tokenFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::IPersonalAccessToken> tokenPtr = m_tokenFactCompPtr.CreateInstance();
	if (!tokenPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		const QByteArray tokenId = record.value("Id").toByteArray();
		tokenPtr->SetId(tokenId);
	}

	if (record.contains("UserId")){
		const QByteArray userId = record.value("UserId").toByteArray();
		tokenPtr->SetUserId(userId);
	}

	if (record.contains("Name")){
		const QString name = record.value("Name").toString();
		tokenPtr->SetName(name);
	}

	if (record.contains("Description")){
		const QString description = record.value("Description").toString();
		tokenPtr->SetDescription(description);
	}

	if (record.contains("TokenHash")){
		const QByteArray tokenHash = record.value("TokenHash").toByteArray();
		tokenPtr->SetTokenHash(tokenHash);
	}

	if (record.contains("Scopes")){
		const QString scopesStr = record.value("Scopes").toString();
		QByteArrayList scopes;
		if (!scopesStr.isEmpty()){
			const QStringList scopeList = scopesStr.split(',');
			for (const QString& scope : scopeList){
				scopes.append(scope.toUtf8());
			}
		}
		tokenPtr->SetScopes(scopes);
	}

	if (record.contains("CreatedAt")){
		QDateTime createdAt = QDateTime::fromString(record.value("CreatedAt").toString(), Qt::ISODate);
		// Ensure the datetime is properly converted to UTC when read from database
		if (createdAt.isValid()){
			createdAt = createdAt.toUTC();
		}
		tokenPtr->SetCreatedAt(createdAt);
	}

	if (record.contains("LastUsedAt")){
		QDateTime lastUsedAt = record.value("LastUsedAt").toDateTime();
		if (lastUsedAt.isValid()){
			// Ensure the datetime is properly converted to UTC when read from database
			lastUsedAt = lastUsedAt.toUTC();
			tokenPtr->SetLastUsedAt(lastUsedAt);
		}
	}

	if (record.contains("ExpiresAt")) {
		QDateTime expiresAt = record.value("ExpiresAt").toDateTime();
		if (expiresAt.isValid()){
			// Ensure the datetime is properly converted to UTC when read from database
			expiresAt = expiresAt.toUTC();
			tokenPtr->SetExpiresAt(expiresAt);
		}
	}

	if (record.contains("Revoked")){
		const bool revoked = record.value("Revoked").toBool();
		tokenPtr->SetRevoked(revoked);
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<imtauth::IPersonalAccessToken>(tokenPtr);
	return retVal;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CPersonalAccessTokenDatabaseDelegateComp::CreateNewObjectQuery(
	const QByteArray& typeId,
	const QByteArray& proposedObjectId,
	const QString& objectName,
	const QString& objectDescription,
	const istd::IChangeable* valuePtr,
	const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || (typeId != "PersonalAccessToken")){
		return NewObjectQuery();
	}

	const imtauth::IPersonalAccessToken* tokenPtr = dynamic_cast<const imtauth::IPersonalAccessToken*>(valuePtr);
	if (tokenPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray tokenId = proposedObjectId;
	if (tokenId.isEmpty()){
		tokenId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString name = tokenPtr->GetName();
	QString description = tokenPtr->GetDescription();
	QString userId = QString::fromUtf8(tokenPtr->GetUserId());
	QString tokenHash = QString::fromUtf8(tokenPtr->GetTokenHash());
	QString scopesStr;
	QString createdAt = tokenPtr->GetCreatedAt().toUTC().toString(Qt::ISODate);
	QString lastUsedAt = tokenPtr->GetLastUsedAt().isValid() ? tokenPtr->GetLastUsedAt().toUTC().toString(Qt::ISODate) : "";
	QString expiresAt = tokenPtr->GetExpiresAt().isValid() ? tokenPtr->GetExpiresAt().toUTC().toString(Qt::ISODate) : "";
	bool revoked = tokenPtr->IsRevoked();

	const QByteArrayList scopes = tokenPtr->GetScopes();
	for (int i = 0; i < scopes.size(); ++i){
		if (i > 0){
			scopesStr += ",";
		}
		scopesStr += QString::fromUtf8(scopes[i]);
	}

	if (name.isEmpty()){
		name = objectName;
	}

	if (description.isEmpty()){
		description = objectDescription;
	}

	const QString createdAtSql = createdAt.isEmpty() ? "NULL" : QString("'%1'").arg(createdAt);
	const QString lastUsedAtSql = lastUsedAt.isEmpty() ? "NULL" : QString("'%1'").arg(lastUsedAt);
	const QString expiresAtSql = expiresAt.isEmpty() ? "NULL" : QString("'%1'").arg(expiresAt);
	const QString revokedSql = revoked ? "TRUE" : "FALSE";

	NewObjectQuery retVal;

	retVal.query = QString(
			"INSERT INTO \"PersonalAccessTokens\""
			"(\"Id\", \"Name\", \"Description\", \"UserId\", \"TokenHash\", \"Scopes\", \"CreatedAt\", \"LastUsedAt\", \"ExpiresAt\", \"Revoked\") "
			"VALUES('%1', '%2', '%3', '%4', '%5', '%6', %7, %8, %9, %10);")
		.arg(QString::fromUtf8(tokenId))
		.arg(name)
		.arg(description)
		.arg(userId)
		.arg(tokenHash)
		.arg(scopesStr)
		.arg(createdAtSql)
		.arg(lastUsedAtSql)
		.arg(expiresAtSql)
		.arg(revokedSql)
		.toUtf8();

	return retVal;
}


QByteArray CPersonalAccessTokenDatabaseDelegateComp::CreateUpdateObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& objectId,
	const istd::IChangeable& object,
	const imtbase::IOperationContext* /*operationContextPtr*/,
	bool /*useExternDelegate*/) const
{
	const imtauth::IPersonalAccessToken* tokenPtr = dynamic_cast<const imtauth::IPersonalAccessToken*>(&object);
	if (tokenPtr == nullptr){
		return QByteArray();
	}

	const QString name = tokenPtr->GetName();
	const QString description = tokenPtr->GetDescription();
	const QString userId = QString::fromUtf8(tokenPtr->GetUserId());
	const QString tokenHash = QString::fromUtf8(tokenPtr->GetTokenHash());

	QString scopesStr;
	const QByteArrayList scopes = tokenPtr->GetScopes();
	for (int i = 0; i < scopes.size(); ++i){
		if (i > 0){
			scopesStr += ",";
		}
		scopesStr += QString::fromUtf8(scopes[i]);
	}

	const QString lastUsedAt = tokenPtr->GetLastUsedAt().isValid()
		? tokenPtr->GetLastUsedAt().toUTC().toString(Qt::ISODate)
		: "";

	const QString expiresAt = tokenPtr->GetExpiresAt().isValid()
		? tokenPtr->GetExpiresAt().toUTC().toString(Qt::ISODate)
		: "";

	const bool revoked = tokenPtr->IsRevoked();

	const QString lastUsedAtSql = lastUsedAt.isEmpty() ? "NULL" : QString("'%1'").arg(lastUsedAt);
	const QString expiresAtSql = expiresAt.isEmpty() ? "NULL" : QString("'%1'").arg(expiresAt);
	const QString revokedSql = revoked ? "TRUE" : "FALSE";

	const QByteArray retVal = QString(
		"UPDATE \"PersonalAccessTokens\" SET "
		"\"Name\"='%1', "
		"\"Description\"='%2', "
		"\"UserId\"='%3', "
		"\"TokenHash\"='%4', "
		"\"Scopes\"='%5', "
		"\"LastUsedAt\"=%6, "
		"\"ExpiresAt\"=%7, "
		"\"Revoked\"=%8 "
		"WHERE \"Id\"='%9';")
		.arg(name)
		.arg(description)
		.arg(userId)
		.arg(tokenHash)
		.arg(scopesStr)
		.arg(lastUsedAtSql)
		.arg(expiresAtSql)
		.arg(revokedSql)
		.arg(QString::fromUtf8(objectId))
		.toUtf8();

	return retVal;
}


QByteArray CPersonalAccessTokenDatabaseDelegateComp::CreateDeleteObjectsQuery(
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
		idsStr += QString("'%1'").arg(QString::fromUtf8(objectIds[i]));
	}

	const QByteArray retVal = QString("DELETE FROM \"PersonalAccessTokens\" WHERE \"Id\" IN (%1);")
		.arg(idsStr)
		.toUtf8();

	return retVal;
}


QByteArray CPersonalAccessTokenDatabaseDelegateComp::CreateDeleteObjectSetQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const iprm::IParamsSet* /*paramsPtr*/,
	const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CPersonalAccessTokenDatabaseDelegateComp::CreateRenameObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& objectId,
	const QString& newObjectName,
	const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectId.isEmpty() || newObjectName.isEmpty()){
		return QByteArray();
	}

	// For PersonalAccessToken, "rename" maps to changing the "Name" column.
	return QString(
			"UPDATE \"PersonalAccessTokens\" SET "
			"\"Name\"='%1' "
			"WHERE \"Id\"='%2';")
		.arg(newObjectName)
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


QByteArray CPersonalAccessTokenDatabaseDelegateComp::CreateDescriptionObjectQuery(
	const imtbase::IObjectCollection& /*collection*/,
	const QByteArray& objectId,
	const QString& description,
	const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectId.isEmpty()){
		return QByteArray();
	}

	// For PersonalAccessToken, "description" maps to changing the "Description" column.
	return QString(
			"UPDATE \"PersonalAccessTokens\" SET "
			"\"Description\"='%1' "
			"WHERE \"Id\"='%2';")
		.arg(description)
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


// reimplemented (icomp::CComponentBase)

void CPersonalAccessTokenDatabaseDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_databaseEngineCompPtr.IsValid()){
		QString tableName = GetTableName();
		if (!TableExists(tableName)){
			QFile scriptFile(":/SQL/CreatePersonalAccessTokenTable.sql");
			if (!scriptFile.open(QFile::ReadOnly)){
				SendErrorMessage(0, QT_TR_NOOP(QString("Personal access tokens table creation script '%1'could not be loaded").arg(scriptFile.fileName())));
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
							<< "\n\t| Table could not be created"
							<< "\n\t| Error: " << sqlError
							<< "\n\t| Query: " << createTableQuery;
	
				SendErrorMessage(0, QT_TR_NOOP(QString("\n\t| Table could not be created"
														"\n\t| Error: %1"
														 "\n\t| Query: %2")
													.arg(sqlError.text(), qPrintable(createTableQuery))));
			}
		}
	}
}


idoc::MetaInfoPtr CPersonalAccessTokenDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CPersonalAccessTokenDatabaseDelegateComp::SetObjectMetaInfoFromRecord(
	const QSqlRecord& record,
	idoc::IDocumentMetaInfo& metaInfo) const
{
	if (!BaseClass::SetObjectMetaInfoFromRecord(record, metaInfo)){
		return false;
	}

	// No extra meta fields for PersonalAccessToken beyond BaseClass at the moment.
	Q_UNUSED(record);
	Q_UNUSED(metaInfo);

	return true;
}

} // namespace imtauthdb
