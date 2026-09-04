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
#include <imtdb/imtdb.h>


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

	const QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedObjectId;
	const NewObjectQuery query = CreatePreparedInsertQuery(typeId, objectId, objectName, objectDescription, *documentPtr, operationContextPtr, 1);

	result.query = query.query;
	result.bindValues = query.bindValues;
	result.objectName = objectName;
	
	return result;
}


IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseBinaryDocumentDelegateComp::CreateUpdateObjectQueryWithParameters(
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
	revision.sql = QStringLiteral(R"((SELECT MAX(%1) + 1 FROM %2"%3" WHERE "%4" = :documentId))")
						.arg(
							/*1*/ CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int),
							/*2*/ schemaPrefix,
							/*3*/ GetTableName(),
							/*4*/ s_documentIdColumn
						);

	return CreatePreparedInsertQuery(
		typeId,
		objectId,
		collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString(),
		collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString(),
		object,
		operationContextPtr,
		QVariant::fromValue(revision)
	);
}


QByteArray CSqlDatabaseBinaryDocumentDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	SendErrorMessage(0, QStringLiteral("Binary document updates require parameter binding"), QStringLiteral("CSqlDatabaseBinaryDocumentDelegateComp"));
		
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
		SendErrorMessage(0, QStringLiteral("Binary document persistence requires a PostgreSQL database"), QStringLiteral("CSqlDatabaseBinaryDocumentDelegateComp"));

		return result;
	}

	QByteArray document;
	if (!WriteDataToMemory(typeId, object, document)) {
		return result;
	}

	const quint32 checksum = istd::CCrcCalculator::GetCrcFromData(reinterpret_cast<const quint8*>(document.constData()), imtbase::narrow_cast<int>(document.size()));
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
			SendWarningMessage(0, QStringLiteral("Unable to create meta info representation for the object '%1' from the table '%2'")
									  .arg(objectId, GetTableName()));
		}
	}

	const QByteArray tenantId = operationContextPtr != nullptr ? operationContextPtr->GetTenantId() : QByteArray();
	const bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
	const bool addTenantBinding = useTenantBindings && !tenantId.isEmpty();
	if (addTenantBinding) {
		EnsureTenantBindingTableExists();
	}

	const QString documentTable		= GetTableName();
	const QString documentIdColumn	= s_documentIdColumn;
	const QString stateColumn		= s_stateColumn;

	const QString columns = QStringLiteral(R"("%1", "%2", "%3", "%4", "%5", "%6", "%7", "%8", "%9", "%10")")
								.arg(
									/*1*/ s_idColumn,
									/*2*/ s_typeIdColumn,
									/*3*/ documentIdColumn,
									/*4*/ s_nameColumn,
									/*5*/ s_descriptionColumn,
									/*6*/ s_documentColumn,
									/*7*/ s_dataMetaInfoColumn,
									/*8*/ s_revisionInfoColumn,
									/*9*/ s_lastModifiedColumn,
									/*10*/ stateColumn
								);

	const QString values = QStringLiteral(":id, :typeId, :documentId, :name, :description, :document, %1, %2, :timeStamp, 'Active'")
								.arg(
									metaInfoRepresentation.isEmpty() ? imtdb::NULL_DATA_LITERAL : QStringLiteral("CAST(:dataMetaInfo AS jsonb)"),
									revisionInfo
								);

	const QString insertDocument = QStringLiteral(R"(INSERT INTO %1"%2" (%3) VALUES (%4) RETURNING "%5")")
										.arg(
											/*1*/ schemaPrefix,
											/*2*/ documentTable,
											/*3*/ columns,
											/*4*/ values,
											/*5*/ documentIdColumn
										);

	const QString deactivate = QStringLiteral(R"(UPDATE %1"%2" SET "%3" = 'InActive' WHERE "%4" = :documentId AND "%3" = 'Active')")
									.arg(
										/*1*/ schemaPrefix,
										/*2*/ documentTable,
										/*3*/ stateColumn,
										/*4*/ documentIdColumn
									);

	if (!addTenantBinding) {
		result.query = QStringLiteral("WITH deactivated AS (%1) %2").arg(deactivate, insertDocument).toUtf8();
	}
	else{
		const QByteArray ownerId = operationContextPtr->GetOperationOwnerId().id;
		const QString createdBy = ownerId.isEmpty() ? imtdb::NULL_DATA_LITERAL : QStringLiteral(":ownerId");
		result.query = QStringLiteral(R"(WITH deactivated AS (%1), inserted AS (%2) INSERT INTO %3 ("Id", "TenantId", "EntityType", "EntityId", "CreatedAt", "CreatedByUserId") SELECT :tenantBindingId, :tenantId, :entityType, "%4", :bindingCreatedAt, %5 FROM inserted ON CONFLICT ("TenantId", "EntityType", "EntityId") DO NOTHING)")
							.arg(
								/*1*/ deactivate,
								/*2*/ insertDocument,
								/*3*/ CreateTenantBindingTableName(),
								/*4*/ documentIdColumn,
								/*5*/ createdBy
							).toUtf8();

		result.bindValues[QStringLiteral(":tenantBindingId")] = QUuid::createUuid().toString(QUuid::WithoutBraces);
		result.bindValues[QStringLiteral(":tenantId")] = QString::fromUtf8(tenantId);
		result.bindValues[QStringLiteral(":entityType")] = documentTable;
		result.bindValues[QStringLiteral(":bindingCreatedAt")] = QDateTime::currentDateTimeUtc();
		if (!ownerId.isEmpty()) {
			result.bindValues[QStringLiteral(":ownerId")] = QString::fromUtf8(ownerId);
		}
	}

	result.bindValues[QStringLiteral(":id")] = QUuid::createUuid().toString(QUuid::WithoutBraces);
	result.bindValues[QStringLiteral(":typeId")] = QString::fromUtf8(typeId);
	result.bindValues[QStringLiteral(":documentId")] = QString::fromUtf8(objectId);
	result.bindValues[QStringLiteral(":name")] = objectName;
	result.bindValues[QStringLiteral(":description")] = objectDescription;
	result.bindValues[QStringLiteral(":document")] = document;
	result.bindValues[QStringLiteral(":timeStamp")] = QDateTime::currentDateTimeUtc();

	if (!metaInfoRepresentation.isEmpty()) {
		result.bindValues[QStringLiteral(":dataMetaInfo")] = QString::fromUtf8(metaInfoRepresentation);
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
		SendErrorMessage(0, QStringLiteral("Unable to write data to memory. Error: Serialization failed"), QStringLiteral("CSqlDatabaseBinaryDocumentDelegateComp"));

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
		SendErrorMessage(0, QStringLiteral("Unable to read data from memory. Error: Serialization failed"), QStringLiteral("CSqlDatabaseBinaryDocumentDelegateComp"));

		return false;
	}

	return true;
}


} // namespace imtdb

