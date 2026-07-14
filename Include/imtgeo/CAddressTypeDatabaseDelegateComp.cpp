// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressTypeDatabaseDelegateComp.h>


// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtgeo
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CAddressTypeDatabaseDelegateComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return QByteArrayLiteral("AddressType");
}


istd::IChangeableUniquePtr CAddressTypeDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_adrTypeInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	IAddressTypeInfoUniquePtr adrTypeInfoPtr = m_adrTypeInfoFactCompPtr.CreateInstance();
	if (!adrTypeInfoPtr.IsValid()){
		return nullptr;
	}

	if (record.contains(QStringLiteral("Id"))){
		QByteArray id = imtdb::VariantToByteArray(record.value(QStringLiteral("Id")));
		adrTypeInfoPtr->SetId(id);
	}

	if (record.contains(QStringLiteral("Name"))){
		QString name = record.value(QStringLiteral("Name")).toString();
		adrTypeInfoPtr->SetName(name);
	}

	if (record.contains(QStringLiteral("ShortName"))){
		QString sname = record.value(QStringLiteral("ShortName")).toString();
		adrTypeInfoPtr->SetShortName(sname);
	}

	if (record.contains(QStringLiteral("Description"))){
		QString description = record.value(QStringLiteral("Description")).toString();
		adrTypeInfoPtr->SetDescription(description);
	}

	return adrTypeInfoPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CAddressTypeDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_tableNameAttrPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Invalid component configuration: Table name is not set");

		return {};
	}

	const IAddressTypeInfo* addressTypeInfoPtr = dynamic_cast<const IAddressTypeInfo*>(valuePtr);
	if (addressTypeInfoPtr == nullptr){
		return {};
	}

	const QString name			= addressTypeInfoPtr->GetName();
	const QString sname			= addressTypeInfoPtr->GetShortName();
	const QString description	= addressTypeInfoPtr->GetDescription();

	NewObjectQuery retVal;
	retVal.query = QStringLiteral(R"(
						INSERT INTO "%5" ("Id", "Name", "ShortName", "Description")
						VALUES('%1', %2, %3, %4);
					)")
						.arg(
							/*1*/ proposedObjectId,
							/*2*/ imtdb::SqlValue(name),
							/*3*/ imtdb::SqlValue(sname),
							/*4*/ imtdb::SqlValue(description),
							/*5*/ *m_tableNameAttrPtr
						).toUtf8();

	retVal.objectName = name;

	return retVal;
}


QByteArray CAddressTypeDatabaseDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_tableNameAttrPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Invalid component configuration: Table name is not set");

		return QByteArray();
	}


	if (objectIds.isEmpty()){
		return QByteArray();
	}

	const QString quotedIds = objectIds.join(u8"','").prepend('\'').append('\'');

	QString query = QStringLiteral(R"(DELETE FROM "%1" WHERE "Id" IN (%2);)")
						.arg(*m_tableNameAttrPtr, quotedIds);

	return query.toUtf8();
}


QByteArray CAddressTypeDatabaseDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CAddressTypeDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	if (!m_tableNameAttrPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Invalid component configuration: Table name is not set");

		return QByteArray();
	}


	const IAddressTypeInfo* addressTypeInfoPtr = dynamic_cast<const IAddressTypeInfo*>(&object);
	if (addressTypeInfoPtr == nullptr || objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray proposedObjectId = addressTypeInfoPtr->GetId();
	if (proposedObjectId.isEmpty() && !objectId.isEmpty()){
		proposedObjectId = objectId;
	}

	if (proposedObjectId.isEmpty()){
		return QByteArray();
	}

	const QString name			= addressTypeInfoPtr->GetName();
	const QString shortName		= addressTypeInfoPtr->GetShortName();
	const QString description	= addressTypeInfoPtr->GetDescription();

	QByteArray retVal = QStringLiteral(R"(
							UPDATE "%6"
							SET "Id" = '%1'
								, "Name" = %2
								, "ShortName" = %3
								, "Description" = %4
							WHERE "Id" = '%5';
						)")
					.arg(
						/*1*/ proposedObjectId,
						/*2*/ imtdb::SqlValue(name),
						/*3*/ imtdb::SqlValue(shortName),
						/*4*/ imtdb::SqlValue(description),
						/*5*/ objectId,
						/*6*/ *m_tableNameAttrPtr
					).toUtf8();

	return retVal;
}


QByteArray CAddressTypeDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CAddressTypeDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


} // namespace imtgeo


