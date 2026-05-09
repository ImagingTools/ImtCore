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
#include <imtauth/IUserRecentAction.h>
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

	if (record.contains("TicketId")){
		actionPtr->SetTicketId(record.value("TicketId").toByteArray());
	}

	imtauth::IUserRecentAction::UserInfo userInfo;
	if (record.contains("UserId")){
		userInfo.id = record.value("UserId").toByteArray();
	}
	if (record.contains("UserName")){
		userInfo.name = record.value("UserName").toString();
	}
	actionPtr->SetUserInfo(userInfo);

	imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo;
	if (record.contains("ActionTypeId")){
		actionTypeInfo.id = record.value("ActionTypeId").toByteArray();
	}
	if (record.contains("ActionTypeName")){
		actionTypeInfo.name = record.value("ActionTypeName").toString();
	}
	if (record.contains("ActionTypeDescription")){
		actionTypeInfo.description = record.value("ActionTypeDescription").toString();
	}
	actionPtr->SetActionTypeInfo(actionTypeInfo);

	if (record.contains("Timestamp")){
		actionPtr->SetTimestamp(QDateTime::fromString(record.value("Timestamp").toString(), Qt::ISODate));
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

	const imtauth::IUserRecentAction::UserInfo userInfo = actionPtr->GetUserInfo();
	const imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo = actionPtr->GetActionTypeInfo();
	const QDateTime timestamp = actionPtr->GetTimestamp();

	const QString ticketIdSql = actionPtr->GetTicketId().isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(actionPtr->GetTicketId())));
	const QString userIdSql = userInfo.id.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(userInfo.id)));
	const QString userNameSql = userInfo.name.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(userInfo.name));
	const QString actionTypeIdSql = actionTypeInfo.id.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(actionTypeInfo.id)));
	const QString actionTypeNameSql = actionTypeInfo.name.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionTypeInfo.name));
	const QString actionTypeDescSql = actionTypeInfo.description.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionTypeInfo.description));
	const QString timestampSql = timestamp.isNull() ? "NULL" : QString("'%1'").arg(sqlEscape(timestamp.toString(Qt::ISODate)));
	const QString actionDataSql = "NULL";

	NewObjectQuery retVal;
	retVal.query = QString(
		"INSERT INTO \"TicketActions\" "
		"(\"Id\", \"TicketId\", \"UserId\", \"UserName\", "
		"\"ActionTypeId\", \"ActionTypeName\", \"ActionTypeDescription\", "
		"\"Timestamp\", \"ActionData\") "
		"VALUES('%1', %2, %3, %4, %5, %6, %7, %8, %9);")
		.arg(sqlEscape(QString::fromUtf8(actionId)))
		.arg(ticketIdSql)
		.arg(userIdSql)
		.arg(userNameSql)
		.arg(actionTypeIdSql)
		.arg(actionTypeNameSql)
		.arg(actionTypeDescSql)
		.arg(timestampSql)
		.arg(actionDataSql)
		.toUtf8();

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

	const imtauth::IUserRecentAction::UserInfo userInfo = actionPtr->GetUserInfo();
	const imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo = actionPtr->GetActionTypeInfo();
	const QDateTime timestamp = actionPtr->GetTimestamp();

	const QString userIdSql = userInfo.id.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(userInfo.id)));
	const QString userNameSql = userInfo.name.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(userInfo.name));
	const QString actionTypeIdSql = actionTypeInfo.id.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(QString::fromUtf8(actionTypeInfo.id)));
	const QString actionTypeNameSql = actionTypeInfo.name.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionTypeInfo.name));
	const QString actionTypeDescSql = actionTypeInfo.description.isEmpty() ? "NULL" : QString("'%1'").arg(sqlEscape(actionTypeInfo.description));
	const QString timestampSql = timestamp.isNull() ? "NULL" : QString("'%1'").arg(sqlEscape(timestamp.toString(Qt::ISODate)));
	const QString actionDataSql = "NULL";

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
		.arg(sqlEscape(QString::fromUtf8(objectId)))
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
		idsStr += QString("'%1'").arg(sqlEscape(QString::fromUtf8(objectIds[i])));
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
