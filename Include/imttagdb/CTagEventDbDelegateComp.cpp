// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttagdb/CTagEventDbDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtdb/imtdb.h>
#include <imttag/CTagEvent.h>


namespace imttagdb
{


static QString ToSqlText(const QByteArray& value)
{
	return value.isEmpty() ? imtdb::NULL_DATA_LITERAL : QStringLiteral("'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(value)));
}


static QString ToSqlText(const QString& value)
{
	return value.isEmpty() ? imtdb::NULL_DATA_LITERAL : QStringLiteral("'%1'").arg(imtdb::EscapeSql(value));
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CTagEventDbDelegateComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_eventFactCompPtr.IsValid()){
		return nullptr;
	}

	imttag::ITagEventUniquePtr eventPtr = m_eventFactCompPtr.CreateInstance();
	if (!eventPtr.IsValid()){
		return nullptr;
	}

	eventPtr->SetAction(imttag::CTagEvent::FromActionId(imtdb::VariantToByteArray(record.value("Action"))));
	eventPtr->SetTagId(imtdb::VariantToByteArray(record.value("TagId")));
	eventPtr->SetTagName(record.value("TagName").toString());
	eventPtr->SetTagColor(record.value("TagColor").toString());
	eventPtr->SetEntityType(imtdb::VariantToByteArray(record.value("EntityType")));
	eventPtr->SetEntityId(imtdb::VariantToByteArray(record.value("EntityId")));

	const QVariant revision = record.value("EntityRevision");
	eventPtr->SetEntityRevision(revision.isNull() ? -1 : revision.toLongLong());

	eventPtr->SetActorId(imtdb::VariantToByteArray(record.value("ActorId")));
	eventPtr->SetActorName(record.value("ActorName").toString());
	eventPtr->SetTenantId(imtdb::VariantToByteArray(record.value("TenantId")));

	QDateTime timestamp = record.value("Timestamp").toDateTime();
	timestamp.setTimeSpec(Qt::UTC);
	eventPtr->SetTimestamp(timestamp);

	return eventPtr;
}


CTagEventDbDelegateComp::NewObjectQuery CTagEventDbDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imttag::ITagEvent* eventPtr = dynamic_cast<const imttag::ITagEvent*>(valuePtr);
	if (eventPtr == nullptr){
		return result;
	}

	const QString id = proposedObjectId.isEmpty()
				? QUuid::createUuid().toString(QUuid::WithoutBraces)
				: imtdb::EscapeSql(QString::fromUtf8(proposedObjectId));

	const QDateTime timestamp = eventPtr->GetTimestamp();
	const QString timestampLiteral = timestamp.isValid() ? timestamp.toUTC().toString(Qt::ISODateWithMs) : imtdb::UtcNow();

	const qlonglong revision = eventPtr->GetEntityRevision();
	const QString revisionLiteral = (revision < 0) ? imtdb::NULL_DATA_LITERAL : QString::number(revision);

	// Joined instead of chained arg() calls so user text containing "%N" is never re-substituted.
	const QStringList values = {
				QStringLiteral("'%1'").arg(id),
				QStringLiteral("'%1'").arg(QString::fromUtf8(imttag::CTagEvent::ToActionId(eventPtr->GetAction()))),
				ToSqlText(eventPtr->GetTagId()),
				ToSqlText(eventPtr->GetTagName()),
				ToSqlText(eventPtr->GetTagColor()),
				ToSqlText(eventPtr->GetEntityType()),
				ToSqlText(eventPtr->GetEntityId()),
				revisionLiteral,
				ToSqlText(eventPtr->GetActorId()),
				ToSqlText(eventPtr->GetActorName()),
				ToSqlText(eventPtr->GetTenantId()),
				QStringLiteral("'%1'").arg(timestampLiteral)
	};

	result.query = QStringLiteral(
				R"(INSERT INTO "%1" ("Id", "Action", "TagId", "TagName", "TagColor", "EntityType", "EntityId", "EntityRevision", "ActorId", "ActorName", "TenantId", "Timestamp") VALUES (%2);)")
				.arg(*m_tableNameAttrPtr, values.join(QStringLiteral(", "))).toUtf8();
	result.objectName = id;

	return result;
}


QByteArray CTagEventDbDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	// Events are append-only.
	return QByteArray();
}


QByteArray CTagEventDbDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArrayList& /*objectIds*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTagEventDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTagEventDbDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTagEventDbDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CTagEventDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
			const QSqlRecord& record,
			idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Timestamp")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("Timestamp").toDateTime());
	}

	return true;
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CTagEventDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new idoc::CStandardDocumentMetaInfo);
}


bool CTagEventDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imttagdb


