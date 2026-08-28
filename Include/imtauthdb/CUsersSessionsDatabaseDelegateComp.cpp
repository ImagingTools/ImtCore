// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CUsersSessionsDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>

// Qt includes
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CSessionInfo.h>
#include <imtauth/CUserInfo.h>
#include <imtdb/imtdb.h>


namespace imtauthdb
{


// public methods

// reimplemented (icomp::CComponentBase)

void CUsersSessionsDatabaseDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	const QString tableName = QString::fromUtf8(GetTableName());
	if (tableName.isEmpty() || !TableExists(tableName)){
		return;
	}

	QString driverId = m_databaseEngineCompPtr->GetDatabaseDriverId();
	QString checkColumnQuery;

	if (driverId == "QPSQL"){
		checkColumnQuery = QString(
			"SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = '%1' AND column_name = 'TenantId');"
		).arg(tableName);
	}
	else if (driverId == "QSQLITE"){
		checkColumnQuery = QString(
			"SELECT COUNT(*) > 0 FROM pragma_table_info('%1') WHERE name = 'TenantId';"
		).arg(tableName);
	}
	else{
		return;
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(checkColumnQuery.toUtf8(), &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		return;
	}

	if (sqlQuery.next() && !sqlQuery.value(0).toBool()){
		QString alterQuery = QString("ALTER TABLE \"%1\" ADD COLUMN \"TenantId\" VARCHAR(1000);").arg(tableName);
		m_databaseEngineCompPtr->ExecSqlQuery(alterQuery.toUtf8(), &sqlError);
	}
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CUsersSessionsDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3'")
			.arg(*m_tableNameAttrPtr, *m_objectIdColumnAttrPtr, SqlEncode(QString::fromUtf8(objectId))).toUtf8();
	}

	QString filterQuery;
	if (paramsPtr != nullptr){
		CreateFilterQuery(*paramsPtr, filterQuery);
	}

	return QString("SELECT * FROM \"%1\" %2;")
		.arg(*m_tableNameAttrPtr, filterQuery).toUtf8();
}


istd::IChangeableUniquePtr CUsersSessionsDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	istd::IChangeableUniquePtr sessionDataPtr = CreateObject("Session");
	if (!sessionDataPtr.IsValid()){
		return nullptr;
	}

	auto sessionInfoPtr = dynamic_cast<imtauth::CSessionInfo*>(sessionDataPtr.GetPtr());
	if (sessionInfoPtr == nullptr){
		return nullptr;
	}

	if (record.contains("RefreshToken")){
		QByteArray token = record.value("RefreshToken").toByteArray();
		sessionInfoPtr->SetToken(token);
	}

	if (record.contains("UserId")){
		QByteArray userId = imtdb::VariantToByteArray(record.value("UserId"));
		sessionInfoPtr->SetUserId(userId);
	}

	if (record.contains("TenantId")){
		QByteArray tenantId = imtdb::VariantToByteArray(record.value("TenantId"));
		sessionInfoPtr->SetTenantId(tenantId);
	}

	if (record.contains("CreationDate")){
		QDateTime creationDate = record.value("CreationDate").toDateTime();
		sessionInfoPtr->SetCreationDate(creationDate);
	}

	if (record.contains("ExpirationDate")){
		QDateTime expirationDate = record.value("ExpirationDate").toDateTime();
		sessionInfoPtr->SetExpirationDate(expirationDate);
	}

	return sessionDataPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUsersSessionsDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	auto sessionPtr = dynamic_cast<const imtauth::ISession*>(valuePtr);
	if (sessionPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray token = sessionPtr->GetToken();
	QByteArray userId = sessionPtr->GetUserId();
	QByteArray tenantId = sessionPtr->GetTenantId();
	QDateTime creationDate = sessionPtr->GetCreationDate();
	QDateTime expirationDate = sessionPtr->GetExpirationDate();

	NewObjectQuery retVal;

	QString driverId = m_databaseEngineCompPtr->GetDatabaseDriverId();
	if (driverId == "QPSQL"){
		retVal.query += QString("\nDELETE FROM \"UserSessions\" WHERE \"ExpirationDate\" < NOW();").toUtf8();
	}
	else if (driverId == "QSQLITE"){
		retVal.query += QString("\nDELETE FROM \"UserSessions\" WHERE \"ExpirationDate\" < strftime('%Y-%m-%dT%H:%M:%S', 'now');").toUtf8();
	}

	retVal.query += QString("\nINSERT INTO \"UserSessions\" (\"Id\", \"RefreshToken\", \"UserId\", \"TenantId\", \"CreationDate\", \"ExpirationDate\") VALUES ('%0', '%1', '%2', '%3', '%4', '%5');")
				.arg(SqlEncode(QString::fromUtf8(proposedObjectId)), SqlEncode(QString::fromUtf8(token)), SqlEncode(QString::fromUtf8(userId)), SqlEncode(QString::fromUtf8(tenantId)), creationDate.toString(Qt::ISODate), expirationDate.toString(Qt::ISODate)).toUtf8();

	return retVal;
}


QByteArray CUsersSessionsDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	auto sessionPtr = dynamic_cast<const imtauth::ISession*>(&object);
	if (sessionPtr == nullptr){
		return QByteArray();
	}

	QByteArray token = sessionPtr->GetToken();
	QByteArray userId = sessionPtr->GetUserId();
	QByteArray tenantId = sessionPtr->GetTenantId();
	QDateTime creationDate = sessionPtr->GetCreationDate();
	QDateTime expirationDate = sessionPtr->GetExpirationDate();

	QByteArray retVal;

	retVal += QString("\nUPDATE \"%0\" SET \"RefreshToken\" = '%1', \"TenantId\" = '%2', \"CreationDate\" = '%3', \"ExpirationDate\" = '%4' WHERE \"%5\" = '%6'")
				  .arg(
					  *m_tableNameAttrPtr,
					  SqlEncode(QString::fromUtf8(token)),
					  SqlEncode(QString::fromUtf8(tenantId)),
					creationDate.toString(Qt::ISODate),
					expirationDate.toString(Qt::ISODate),
					*m_objectIdColumnAttrPtr,
					SqlEncode(QString::fromUtf8(objectId))).toUtf8();

	return retVal;
}


QByteArray CUsersSessionsDatabaseDelegateComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return QByteArray("Session");
}


QByteArray CUsersSessionsDatabaseDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}
	
	QStringList quotedIds;
	for (const QByteArray& objectId : objectIds){
		quotedIds << QString("'%1'").arg(SqlEncode(QString::fromUtf8(objectId)));
	}
	
	QString query = QString(
						"DELETE FROM \"%1\" WHERE \"%2\" IN (%3);")
						.arg(
							QString::fromUtf8(*m_tableNameAttrPtr),
							QString::fromUtf8(*m_objectIdColumnAttrPtr),
							quotedIds.join(", ")
							);
	
	return query.toUtf8();
}


bool CUsersSessionsDatabaseDelegateComp::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("RefreshToken")){
		auto textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter("RefreshToken"));
		if (textParamPtr == nullptr){
			return false;
		}

		filterQuery += QString(R"( WHERE "RefreshToken" = '%1')").arg(SqlEncode(textParamPtr->GetText()));

		return true;
	}

	return false;
}


} // namespace imtauthdb

