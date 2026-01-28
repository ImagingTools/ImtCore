#include <imtauthdb/CPersonalAccessTokenDatabaseDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDateTime>
#include <QtSql/QSqlRecord>

// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>
#include <imtauth/CPersonalAccessTokenMetaInfoCreatorComp.h>
#include <imtdb/CDatabaseEngineComp.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CPersonalAccessTokenDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
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
		QByteArray tokenId = record.value("Id").toByteArray();
		tokenPtr->SetId(tokenId);
	}

	if (record.contains("UserId")){
		QByteArray userId = record.value("UserId").toByteArray();
		tokenPtr->SetUserId(userId);
	}

	if (record.contains("Name")){
		QString name = record.value("Name").toString();
		tokenPtr->SetName(name);
	}

	if (record.contains("Description")){
		QString description = record.value("Description").toString();
		tokenPtr->SetDescription(description);
	}

	if (record.contains("TokenHash")){
		QByteArray tokenHash = record.value("TokenHash").toByteArray();
		tokenPtr->SetTokenHash(tokenHash);
	}

	if (record.contains("Scopes")){
		QString scopesStr = record.value("Scopes").toString();
		QByteArrayList scopes;
		if (!scopesStr.isEmpty())
		{
			QStringList scopeList = scopesStr.split(',');
			for (const QString& scope : scopeList)
			{
				scopes.append(scope.toUtf8());
			}
		}
		tokenPtr->SetScopes(scopes);
	}

	if (record.contains("CreatedAt")){
		QDateTime createdAt = QDateTime::fromString(record.value("CreatedAt").toString(), Qt::ISODate);
		tokenPtr->SetCreatedAt(createdAt);
	}

	if (record.contains("LastUsedAt")){
		QString lastUsedStr = record.value("LastUsedAt").toString();
		if (!lastUsedStr.isEmpty())
		{
			QDateTime lastUsedAt = QDateTime::fromString(lastUsedStr, Qt::ISODate);
			tokenPtr->SetLastUsedAt(lastUsedAt);
		}
	}

	if (record.contains("ExpiresAt")){
		QString expiresAtStr = record.value("ExpiresAt").toString();
		if (!expiresAtStr.isEmpty())
		{
			QDateTime expiresAt = QDateTime::fromString(expiresAtStr, Qt::ISODate);
			tokenPtr->SetExpiresAt(expiresAt);
		}
	}

	if (record.contains("Revoked")){
		bool revoked = record.value("Revoked").toBool();
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
	if (typeId.isEmpty() || typeId != "PersonalAccessToken"){
		return NewObjectQuery();
	}

	QByteArray tokenId = proposedObjectId;
	if (tokenId.isEmpty()){
		tokenId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString name;
	QString description;
	QString userId;
	QString tokenHash;
	QString scopesStr;
	QString createdAt;
	QString lastUsedAt;
	QString expiresAt;
	bool revoked = false;

	const imtauth::IPersonalAccessToken* tokenPtr = dynamic_cast<const imtauth::IPersonalAccessToken*>(valuePtr);
	if (tokenPtr != nullptr){
		name = tokenPtr->GetName();
		description = tokenPtr->GetDescription();
		userId = QString::fromUtf8(tokenPtr->GetUserId());
		tokenHash = QString::fromUtf8(tokenPtr->GetTokenHash());
		
		QByteArrayList scopes = tokenPtr->GetScopes();
		for (int i = 0; i < scopes.size(); ++i)
		{
			if (i > 0) scopesStr += ",";
			scopesStr += QString::fromUtf8(scopes[i]);
		}
		
		createdAt = tokenPtr->GetCreatedAt().toString(Qt::ISODate);
		lastUsedAt = tokenPtr->GetLastUsedAt().isValid() ? tokenPtr->GetLastUsedAt().toString(Qt::ISODate) : "";
		expiresAt = tokenPtr->GetExpiresAt().isValid() ? tokenPtr->GetExpiresAt().toString(Qt::ISODate) : "";
		revoked = tokenPtr->IsRevoked();
	}

	if (name.isEmpty()){
		name = objectName;
	}

	if (description.isEmpty()){
		description = objectDescription;
	}

	NewObjectQuery retVal;

	retVal.query = QString("INSERT INTO \"PersonalAccessTokens\"(\"Id\", \"Name\", \"Description\", \"UserId\", \"TokenHash\", \"Scopes\", \"CreatedAt\", \"LastUsedAt\", \"ExpiresAt\", \"Revoked\") "
						   "VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', %10);")
			.arg(QString::fromUtf8(tokenId))
			.arg(name)
			.arg(description)
			.arg(userId)
			.arg(tokenHash)
			.arg(scopesStr)
			.arg(createdAt)
			.arg(lastUsedAt)
			.arg(expiresAt)
			.arg(revoked ? 1 : 0).toUtf8();

	return retVal;
}


QByteArray CPersonalAccessTokenDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	const imtauth::IPersonalAccessToken* tokenPtr = dynamic_cast<const imtauth::IPersonalAccessToken*>(&object);
	if (tokenPtr == nullptr){
		return QByteArray();
	}

	QString name = tokenPtr->GetName();
	QString description = tokenPtr->GetDescription();
	QString userId = QString::fromUtf8(tokenPtr->GetUserId());
	QString tokenHash = QString::fromUtf8(tokenPtr->GetTokenHash());
	
	QString scopesStr;
	QByteArrayList scopes = tokenPtr->GetScopes();
	for (int i = 0; i < scopes.size(); ++i){
		if (i > 0) scopesStr += ",";
		scopesStr += QString::fromUtf8(scopes[i]);
	}
	
	QString lastUsedAt = tokenPtr->GetLastUsedAt().isValid() ? tokenPtr->GetLastUsedAt().toString(Qt::ISODate) : "";
	QString expiresAt = tokenPtr->GetExpiresAt().isValid() ? tokenPtr->GetExpiresAt().toString(Qt::ISODate) : "";
	bool revoked = tokenPtr->IsRevoked();

	QByteArray retVal = QString(
		"UPDATE \"PersonalAccessTokens\" SET "
		"\"Name\"='%1', \"Description\"='%2', \"UserId\"='%3', \"TokenHash\"='%4', \"Scopes\"='%5', \"LastUsedAt\"='%6', \"ExpiresAt\"='%7', \"Revoked\"=%8 "
		"WHERE \"Id\"='%9';")
		.arg(name)
		.arg(description)
		.arg(userId)
		.arg(tokenHash)
		.arg(scopesStr)
		.arg(lastUsedAt)
		.arg(expiresAt)
		.arg(revoked ? 1 : 0)
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
		if (i > 0) idsStr += ", ";
		idsStr += QString("'%1'").arg(QString::fromUtf8(objectIds[i]));
	}

	QByteArray retVal = QString("DELETE FROM \"PersonalAccessTokens\" WHERE \"Id\" IN (%1);")
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
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CPersonalAccessTokenDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CPersonalAccessTokenDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CPersonalAccessTokenDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (!BaseClass::SetObjectMetaInfoFromRecord(record, metaInfo)){
		return false;
	}

	// Set additional metadata from record if needed
	return true;
}


} // namespace imtauthdb
