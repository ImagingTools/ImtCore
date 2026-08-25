// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseBinaryDocumentDelegateComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>
#include <istd/CCrcCalculator.h>
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtdb
{


// reimplemented (imtdb::IDatabaseObjectDelegate)

IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseBinaryDocumentDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	NewObjectQuery result;
	istd::IChangeableUniquePtr documentInstancePtr;
	istd::TOptDelPtr<const istd::IChangeable> documentPtr;

	if (valuePtr != nullptr) {
		documentPtr.SetPtr(valuePtr, false);
	}
	else {
		documentInstancePtr = CreateObject(typeId);
		documentPtr.SetPtr(documentInstancePtr.GetPtr(), false);
	}

	if (!documentPtr.IsValid()) {
		return result;
	}

	const QByteArray objectId =
		proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedObjectId;
	const auto query = CreatePreparedInsertQuery(
		typeId, objectId, objectName, objectDescription, *documentPtr, operationContextPtr, 1);
	result.query = query.query;
	result.bindValues = query.bindValues;
	result.objectName = objectName;
	return result;
}


IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseBinaryDocumentDelegateComp::CreateUpdateObjectQueryWithBindings(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	const QByteArray typeId = collection.GetObjectTypeId(objectId);
	if (typeId.isEmpty()) {
		return NewObjectQuery();
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()) {
		schemaPrefix = QString::fromUtf8(*m_tableSchemaAttrPtr) + '.';
	}
	RawSqlExpression revision;
	revision.sql = QString(
						   "(SELECT MAX(%1) + 1 FROM %2\"%3\" WHERE \"%4\" = :documentId)")
						   .arg(
							   CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int),
							   schemaPrefix,
							   QString::fromUtf8(GetTableName()),
							   QString::fromUtf8(s_documentIdColumn));

	return CreatePreparedInsertQuery(
		typeId,
		objectId,
		collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString(),
		collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString(),
		object,
		operationContextPtr,
		QVariant::fromValue(revision));
}


QByteArray CSqlDatabaseBinaryDocumentDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	SendErrorMessage(
		0,
		"Binary document updates require parameter binding",
		"CSqlDatabaseBinaryDocumentDelegateComp");
	return QByteArray();
}


IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseBinaryDocumentDelegateComp::CreatePreparedInsertQuery(
			const QByteArray& typeId,
			const QByteArray& objectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			const QVariant& revisionArgument) const
{
	NewObjectQuery result;

	if (!m_databaseEngineCompPtr.IsValid() ||
		m_databaseEngineCompPtr->GetDatabaseDriverId().compare(QByteArrayLiteral("QPSQL"), Qt::CaseInsensitive) != 0) {
		SendErrorMessage(0, "Binary document persistence requires a PostgreSQL database", "CSqlDatabaseBinaryDocumentDelegateComp");
		return result;
	}

	QByteArray document;
	if (!WriteDataToMemory(typeId, object, document)) {
		return result;
	}

	const quint32 checksum = istd::CCrcCalculator::GetCrcFromData(
		reinterpret_cast<const quint8*>(document.constData()), imtbase::narrow_cast<int>(document.size()));
	const QString revisionInfo = QString::fromUtf8(CreateRevisionInfoQuery(operationContextPtr, revisionArgument, checksum));

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()) {
		schemaPrefix = QString::fromUtf8(*m_tableSchemaAttrPtr) + '.';
	}

	QByteArray metaInfoRepresentation;
	const bool useMetaData = m_useDataMetaInfoAttrPtr.IsValid() ? *m_useDataMetaInfoAttrPtr : false;
	if (useMetaData && m_metaInfoCreatorCompPtr.IsValid() && m_jsonBasedMetaInfoDelegateCompPtr.IsValid()) {
		idoc::MetaInfoPtr metaInfoPtr;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(&object, typeId, metaInfoPtr) && metaInfoPtr.IsValid() &&
			!m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr(), metaInfoRepresentation, typeId)) {
			SendWarningMessage(0, QString("Unable to create meta info representation for the object '%1' from the table '%2'")
									  .arg(QString::fromUtf8(objectId), QString::fromUtf8(GetTableName())));
		}
	}

	const QByteArray tenantId = operationContextPtr != nullptr ? operationContextPtr->GetTenantId() : QByteArray();
	const bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
	const bool addTenantBinding = useTenantBindings && !tenantId.isEmpty();
	if (addTenantBinding) {
		EnsureTenantBindingTableExists();
	}

	const QString documentTable = QString::fromUtf8(GetTableName());
	const QString documentIdColumn = QString::fromUtf8(s_documentIdColumn);
	const QString stateColumn = QString::fromUtf8(s_stateColumn);
	const QString columns = QString(
		"\"%1\", \"%2\", \"%3\", \"%4\", \"%5\", \"%6\", \"%7\", \"%8\", \"%9\", \"%10\"")
								.arg(
									QString::fromUtf8(s_idColumn),
									QString::fromUtf8(s_typeIdColumn),
									documentIdColumn,
									QString::fromUtf8(s_nameColumn),
									QString::fromUtf8(s_descriptionColumn),
									QString::fromUtf8(s_documentColumn),
									QString::fromUtf8(s_dataMetaInfoColumn),
									QString::fromUtf8(s_revisionInfoColumn),
									QString::fromUtf8(s_lastModifiedColumn),
									stateColumn);
	const QString values = QString(
		":id, :typeId, :documentId, :name, :description, :document, %1, %2, :timeStamp, 'Active'")
							   .arg(
								   metaInfoRepresentation.isEmpty() ? QStringLiteral("NULL") : QStringLiteral("CAST(:dataMetaInfo AS jsonb)"),
								   revisionInfo);
	const QString insertDocument = QString("INSERT INTO %1\"%2\" (%3) VALUES (%4) RETURNING \"%5\"")
									   .arg(schemaPrefix, documentTable, columns, values, documentIdColumn);
	const QString deactivate = QString(
		"UPDATE %1\"%2\" SET \"%3\" = 'InActive' WHERE \"%4\" = :documentId AND \"%3\" = 'Active'")
									 .arg(schemaPrefix, documentTable, stateColumn, documentIdColumn);

	if (addTenantBinding) {
		const QByteArray ownerId = operationContextPtr->GetOperationOwnerId().id;
		const QString createdBy = ownerId.isEmpty() ? QStringLiteral("NULL") : QStringLiteral(":ownerId");
		result.query = QString(
			"WITH deactivated AS (%1), inserted AS (%2) "
			"INSERT INTO %3 (\"Id\", \"TenantId\", \"EntityType\", \"EntityId\", \"CreatedAt\", \"CreatedByUserId\") "
			"SELECT :tenantBindingId, :tenantId, :entityType, \"%4\", :bindingCreatedAt, %5 FROM inserted "
			"ON CONFLICT (\"TenantId\", \"EntityType\", \"EntityId\") DO NOTHING")
						   .arg(deactivate, insertDocument, CreateTenantBindingTableName(), documentIdColumn, createdBy)
						   .toUtf8();
		result.bindValues[":tenantBindingId"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
		result.bindValues[":tenantId"] = QString::fromUtf8(tenantId);
		result.bindValues[":entityType"] = documentTable;
		result.bindValues[":bindingCreatedAt"] = QDateTime::currentDateTimeUtc();
		if (!ownerId.isEmpty()) {
			result.bindValues[":ownerId"] = QString::fromUtf8(ownerId);
		}
	}
	else {
		result.query = QString("WITH deactivated AS (%1) %2").arg(deactivate, insertDocument).toUtf8();
	}

	result.bindValues[":id"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
	result.bindValues[":typeId"] = QString::fromUtf8(typeId);
	result.bindValues[":documentId"] = QString::fromUtf8(objectId);
	result.bindValues[":name"] = objectName;
	result.bindValues[":description"] = objectDescription;
	result.bindValues[":document"] = document;
	result.bindValues[":timeStamp"] = QDateTime::currentDateTimeUtc();
	if (!metaInfoRepresentation.isEmpty()) {
		result.bindValues[":dataMetaInfo"] = QString::fromUtf8(metaInfoRepresentation);
	}
	return result;
}


bool CSqlDatabaseBinaryDocumentDelegateComp::WriteDataToMemory(const QByteArray& /*typeId*/, const istd::IChangeable& object, QByteArray& data) const
{
	auto serializableObjectPtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&object));
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CMemoryWriteArchive writeArchive(m_versionInfoCompPtr.GetPtr());
	iser::CBinaryWriteArchiveBase& archive = writeArchive;
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to write data to memory. Error: Serialization failed", "CSqlDatabaseBinaryDocumentDelegateComp");
		return false;
	}

	data = QByteArray(static_cast<const char*>(writeArchive.GetBuffer()), writeArchive.GetBufferSize());

	return true;
}


bool CSqlDatabaseBinaryDocumentDelegateComp::ReadDataFromMemory(const QByteArray& /*typeId*/, const QByteArray& data, istd::IChangeable& object) const
{
	auto serializableObjectPtr = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	const int bufferSize = imtbase::narrow_cast<int>(data.size());
	iser::CMemoryReadArchive readArchive(data.constData(), bufferSize);
	iser::CBinaryReadArchiveBase& archive = readArchive;
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to read data from memory. Error: Serialization failed", "CSqlDatabaseBinaryDocumentDelegateComp");
		return false;
	}

	return true;
}


} // namespace imtdb
