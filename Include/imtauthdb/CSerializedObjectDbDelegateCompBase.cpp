// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CSerializedObjectDbDelegateCompBase.h>


// Qt includes
#include <QtSql/QSqlRecord>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <iser/CJsonMemReadArchive.h>
#include <iser/CJsonMemWriteArchive.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtauthdb
{


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CSerializedObjectDbDelegateCompBase::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!record.contains("Data")){
		return nullptr;
	}

	istd::IChangeableUniquePtr objectPtr = CreateDataObject();
	iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(objectPtr.GetPtr());
	if (serializablePtr == nullptr){
		return nullptr;
	}

	iser::CJsonMemReadArchive archive(record.value("Data").toString().toUtf8(), false);
	if (!serializablePtr->Serialize(archive)){
		SendErrorMessage(0, QStringLiteral("An object of the table '%1' could not be read").arg(QString::fromUtf8(*m_tableNameAttrPtr)), "CSerializedObjectDbDelegateCompBase");

		return nullptr;
	}

	return objectPtr;
}


CSerializedObjectDbDelegateCompBase::NewObjectQuery CSerializedObjectDbDelegateCompBase::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery retVal;

	Columns columns;
	if (valuePtr == nullptr || proposedObjectId.isEmpty() || !GetColumns(*valuePtr, columns)){
		return retVal;
	}

	QStringList names = {QStringLiteral(R"("Id")"), QStringLiteral(R"("Data")")};
	QStringList values = {IdLiteral(proposedObjectId), CreateDataLiteral(*valuePtr)};
	for (const QPair<QString, QString>& column : std::as_const(columns)){
		names << QStringLiteral(R"("%1")").arg(column.first);
		values << column.second;
	}

	retVal.query = (QStringLiteral("INSERT INTO %1 (%2) VALUES (%3);")
				.arg(GetQuotedTableName(), names.join(QStringLiteral(", ")), values.join(QStringLiteral(", ")))
				+ CreateDependentStatements(proposedObjectId, valuePtr)).toUtf8();

	return retVal;
}


QByteArray CSerializedObjectDbDelegateCompBase::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	Columns columns;
	if (objectId.isEmpty() || !GetColumns(object, columns)){
		return QByteArray();
	}

	QStringList assignments = {QStringLiteral(R"("Data" = %1)").arg(CreateDataLiteral(object))};
	for (const QPair<QString, QString>& column : std::as_const(columns)){
		assignments << QStringLiteral(R"("%1" = %2)").arg(column.first, column.second);
	}

	return (QStringLiteral(R"(UPDATE %1 SET %2 WHERE "Id" = %3;)")
				.arg(GetQuotedTableName(), assignments.join(QStringLiteral(", ")), IdLiteral(objectId))
				+ CreateDependentStatements(objectId, &object)).toUtf8();
}


QByteArray CSerializedObjectDbDelegateCompBase::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArrayList& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QString retVal;
	QStringList ids;
	for (const QByteArray& objectId : objectIds){
		retVal += CreateDependentStatements(objectId, nullptr);
		ids << IdLiteral(objectId);
	}

	retVal += QStringLiteral(R"(DELETE FROM %1 WHERE "Id" IN (%2);)").arg(GetQuotedTableName(), ids.join(QStringLiteral(", ")));

	return retVal.toUtf8();
}


QByteArray CSerializedObjectDbDelegateCompBase::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CSerializedObjectDbDelegateCompBase::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CSerializedObjectDbDelegateCompBase::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


// protected methods

QString CSerializedObjectDbDelegateCompBase::CreateDependentStatements(const QByteArray& /*objectId*/, const istd::IChangeable* /*objectPtr*/) const
{
	return QString();
}


QString CSerializedObjectDbDelegateCompBase::GetQuotedTableName() const
{
	return QStringLiteral(R"("%1")").arg(QString::fromUtf8(*m_tableNameAttrPtr));
}


QString CSerializedObjectDbDelegateCompBase::TextLiteral(const QString& text) const
{
	return QStringLiteral("'%1'").arg(SqlEncode(text));
}


QString CSerializedObjectDbDelegateCompBase::IdLiteral(const QByteArray& id) const
{
	return TextLiteral(QString::fromUtf8(id));
}


QString CSerializedObjectDbDelegateCompBase::TimeLiteral(const QDateTime& time) const
{
	if (!time.isValid()){
		return imtdb::NULL_DATA_LITERAL;
	}

	return TextLiteral(time.toUTC().toString(Qt::ISODateWithMs));
}


QString CSerializedObjectDbDelegateCompBase::NumberLiteral(int value)
{
	return QString::number(value);
}


QString CSerializedObjectDbDelegateCompBase::ContainsCondition(const QString& columnName, const QString& text) const
{
	QString pattern = text;
	pattern.replace('!', QStringLiteral("!!"));
	pattern.replace('%', QStringLiteral("!%"));
	pattern.replace('_', QStringLiteral("!_"));

	return QStringLiteral(R"("%1" LIKE '%%2%' ESCAPE '!')").arg(columnName, SqlEncode(pattern));
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

bool CSerializedObjectDbDelegateCompBase::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& /*record*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return true;
}


idoc::MetaInfoPtr CSerializedObjectDbDelegateCompBase::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new idoc::CStandardDocumentMetaInfo);
}


bool CSerializedObjectDbDelegateCompBase::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


// private methods

QString CSerializedObjectDbDelegateCompBase::CreateDataLiteral(const istd::IChangeable& object) const
{
	iser::ISerializable* serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&object));
	if (serializablePtr == nullptr){
		return imtdb::NULL_DATA_LITERAL;
	}

	iser::CJsonMemWriteArchive archive(nullptr, false);
	if (!serializablePtr->Serialize(archive)){
		return imtdb::NULL_DATA_LITERAL;
	}

	return TextLiteral(QString::fromUtf8(archive.GetData()));
}


} // namespace imtauthdb


