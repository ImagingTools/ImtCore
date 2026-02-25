// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CUsersSessionsDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CSessionInfo.h>
#include <imtauth/CUserInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CUsersSessionsDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3'")
			.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(*m_objectIdColumnAttrPtr), qPrintable(objectId)).toUtf8();
	}

	QString filterQuery;
	if (paramsPtr != nullptr){
		CreateFilterQuery(*paramsPtr, filterQuery);
	}

	return QString("SELECT * FROM \"%1\" %2;")
		.arg(qPrintable(*m_tableNameAttrPtr), filterQuery).toUtf8();
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
		QByteArray userId = record.value("UserId").toByteArray();
		sessionInfoPtr->SetUserId(userId);
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
	QDateTime creationDate = sessionPtr->GetCreationDate();
	QDateTime expirationDate = sessionPtr->GetExpirationDate();

	NewObjectQuery retVal;

	retVal.query += QString("\nINSERT INTO \"UserSessions\" (\"Id\", \"RefreshToken\", \"UserId\", \"CreationDate\", \"ExpirationDate\") VALUES ('%0', '%1', '%2', '%3', '%4');")
				.arg(qPrintable(proposedObjectId), qPrintable(token), qPrintable(userId), creationDate.toString(Qt::ISODate), expirationDate.toString(Qt::ISODate)).toUtf8();

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
	QDateTime creationDate = sessionPtr->GetCreationDate();
	QDateTime expirationDate = sessionPtr->GetExpirationDate();

	QByteArray retVal;

	retVal += QString("\nUPDATE \"%0\" SET \"RefreshToken\" = '%1', \"CreationDate\" = '%2', \"ExpirationDate\" = '%3' WHERE \"%4\" = '%5'")
				  .arg(
					  qPrintable(*m_tableNameAttrPtr),
					  qPrintable(token),
					creationDate.toString(Qt::ISODate),
					expirationDate.toString(Qt::ISODate),
					qPrintable(*m_objectIdColumnAttrPtr),
					qPrintable(objectId)).toUtf8();

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
		quotedIds << QString("'%1'").arg(qPrintable(objectId));
	}
	
	QString query = QString(
						"DELETE FROM \"%1\" WHERE \"%2\" IN (%3);")
						.arg(
							QString::fromUtf8(qPrintable(*m_tableNameAttrPtr)),
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

		filterQuery += QString(R"( WHERE "RefreshToken" = '%1')").arg(textParamPtr->GetText());

		return true;
	}

	return false;
}


} // namespace imtauthdb


