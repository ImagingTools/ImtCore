// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskdb/CTicketActionDbDelegateComp.h>

// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtdesk/ITicketAction.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtdeskdb
{


namespace
{

// Helper: escape single quotes for SQL string literals
QString sqlEscape(const QString& s)
{
	QString escaped = s;
	escaped.replace('\'', "''");
	return escaped;
}

} // anonymous namespace


istd::IChangeableUniquePtr CTicketActionDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_ticketActionFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtdesk::ITicketAction> actionPtr = m_ticketActionFactCompPtr.CreateInstance();
	if (!actionPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		actionPtr->SetId(record.value("Id").toByteArray());
	}
	if (record.contains("TicketId")){
		actionPtr->SetTicketId(record.value("TicketId").toByteArray());
	}
	if (record.contains("UserId")){
		actionPtr->SetUserId(record.value("UserId").toByteArray());
	}
	if (record.contains("UserName")){
		actionPtr->SetUserName(record.value("UserName").toString());
	}
	if (record.contains("ActionTypeId")){
		actionPtr->SetActionTypeId(record.value("ActionTypeId").toByteArray());
	}
	if (record.contains("ActionTypeName")){
		actionPtr->SetActionTypeName(record.value("ActionTypeName").toString());
	}
	if (record.contains("ActionTypeDescription")){
		actionPtr->SetActionTypeDescription(record.value("ActionTypeDescription").toString());
	}
	if (record.contains("Timestamp")){
		actionPtr->SetTimestamp(record.value("Timestamp").toString());
	}
	if (record.contains("ActionData")){
		actionPtr->SetActionData(record.value("ActionData").toString());
	}

	return actionPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CTicketActionDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& typeId,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || typeId != "TicketAction"){
		return NewObjectQuery();
	}

	const imtdesk::ITicketAction* actionPtr = dynamic_cast<const imtdesk::ITicketAction*>(valuePtr);
	if (actionPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray actionId = proposedObjectId;
	if (actionId.isEmpty()){
		actionId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	const QString ticketIdSql = actionPtr->GetTicketId().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(actionPtr->GetTicketId())));
	const QString userIdSql = actionPtr->GetUserId().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(actionPtr->GetUserId())));
	const QString userNameSql = actionPtr->GetUserName().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetUserName()));
	const QString actionTypeIdSql = actionPtr->GetActionTypeId().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(actionPtr->GetActionTypeId())));
	const QString actionTypeNameSql = actionPtr->GetActionTypeName().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetActionTypeName()));
	const QString actionTypeDescSql = actionPtr->GetActionTypeDescription().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetActionTypeDescription()));
	const QString timestampSql = actionPtr->GetTimestamp().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetTimestamp()));
	const QString actionDataSql = actionPtr->GetActionData().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetActionData()));

	NewObjectQuery retVal;
	retVal.query = QString(
		"INSERT INTO \"TicketActions\" "
		"(\"Id\", \"TicketId\", \"UserId\", \"UserName\", "
		"\"ActionTypeId\", \"ActionTypeName\", \"ActionTypeDescription\", "
		"\"Timestamp\", \"ActionData\") "
		"VALUES('%1', %2, %3, %4, %5, %6, %7, %8, %9);")
		.arg(QString::fromUtf8(actionId))
		.arg(ticketIdSql)
		.arg(userIdSql)
		.arg(userNameSql)
		.arg(actionTypeIdSql)
		.arg(actionTypeNameSql)
		.arg(actionTypeDescSql)
		.arg(timestampSql)
		.arg(actionDataSql)
		.toUtf8();

	retVal.objectId = actionId;

	return retVal;
}


QByteArray CTicketActionDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtdesk::ITicketAction* actionPtr = dynamic_cast<const imtdesk::ITicketAction*>(&object);
	if (actionPtr == nullptr){
		return QByteArray();
	}

	const QString userIdSql = actionPtr->GetUserId().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(actionPtr->GetUserId())));
	const QString userNameSql = actionPtr->GetUserName().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetUserName()));
	const QString actionTypeIdSql = actionPtr->GetActionTypeId().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(actionPtr->GetActionTypeId())));
	const QString actionTypeNameSql = actionPtr->GetActionTypeName().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetActionTypeName()));
	const QString actionTypeDescSql = actionPtr->GetActionTypeDescription().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetActionTypeDescription()));
	const QString timestampSql = actionPtr->GetTimestamp().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetTimestamp()));
	const QString actionDataSql = actionPtr->GetActionData().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionPtr->GetActionData()));

	return QString(
		"UPDATE \"TicketActions\" SET "
		"\"UserId\"=%1, "
		"\"UserName\"=%2, "
		"\"ActionTypeId\"=%3, "
		"\"ActionTypeName\"=%4, "
		"\"ActionTypeDescription\"=%5, "
		"\"Timestamp\"=%6, "
		"\"ActionData\"=%7 "
		"WHERE \"Id\"='%8';")
		.arg(userIdSql)
		.arg(userNameSql)
		.arg(actionTypeIdSql)
		.arg(actionTypeNameSql)
		.arg(actionTypeDescSql)
		.arg(timestampSql)
		.arg(actionDataSql)
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


QByteArray CTicketActionDbDelegateComp::CreateDeleteObjectsQuery(
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

	return QString("DELETE FROM \"TicketActions\" WHERE \"Id\" IN (%1);")
		.arg(idsStr)
		.toUtf8();
}


QByteArray CTicketActionDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTicketActionDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTicketActionDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


void CTicketActionDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	const QString tableName = GetTableName();
	if (TableExists(tableName)){
		return;
	}

	QFile scriptFile(imtdb::GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateTicketActionsTable.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QString("TicketActions table creation script '%1' could not be loaded").arg(scriptFile.fileName()));
		return;
	}

	QByteArray query = scriptFile.readAll();
	scriptFile.close();
	query.replace("${TableScheme}", "public");

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| TicketActions table could not be created"
					<< "\n\t| Error:" << sqlError
					<< "\n\t| Query:" << query;
		SendErrorMessage(0, QString("TicketActions table could not be created: %1").arg(sqlError.text()));
	}
}


QString CTicketActionDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (paramIds.contains("TicketId")){
		iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, "TicketId");
		if (idParamPtr.IsValid()){
			QByteArray ticketId = idParamPtr->GetId();
			if (!ticketId.isEmpty()){
				return QString("\"TicketId\"='%1'").arg(sqlEscape(QString::fromUtf8(ticketId)));
			}
		}
	}

	return QString();
}


idoc::MetaInfoPtr CTicketActionDbDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CTicketActionDbDelegateComp::SetObjectMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	if (!BaseClass::SetObjectMetaInfoFromRecord(record, metaInfo)){
		return false;
	}
	Q_UNUSED(record);
	Q_UNUSED(metaInfo);
	return true;
}


} // namespace imtdeskdb
