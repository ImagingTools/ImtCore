// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressTypeDatabaseDelegateComp.h>


namespace imtgeo
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CAddressTypeDatabaseDelegateComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return "AddressType";
}


istd::IChangeableUniquePtr CAddressTypeDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_adrTypeInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	IAddressTypeInfoUniquePtr adrTypeInfoPtr = m_adrTypeInfoFactCompPtr.CreateInstance();
	if (!adrTypeInfoPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		QByteArray id = record.value("Id").toByteArray();
		adrTypeInfoPtr->SetId(id);
	}

	if (record.contains("Name")){
		QString name = record.value("Name").toString();
		adrTypeInfoPtr->SetName(name);
	}

	if (record.contains("ShortName")){
		QString sname = record.value("ShortName").toString();
		adrTypeInfoPtr->SetShortName(sname);
	}

	if (record.contains("Description")){
		QString description = record.value("Description").toString();
		adrTypeInfoPtr->SetDescription(description);
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<IAddressTypeInfo>(adrTypeInfoPtr);

	return retVal;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CAddressTypeDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const IAddressTypeInfo* adrInfoPtr = dynamic_cast<const IAddressTypeInfo*>(valuePtr);
	if (adrInfoPtr == nullptr){
		return NewObjectQuery();
	}

	QString name = adrInfoPtr->GetName();
	QString sname = adrInfoPtr->GetShortName();
	QString description = adrInfoPtr->GetDescription();

	NewObjectQuery retVal;
	retVal.query = QString("INSERT INTO \"AddressTypes\"(\"Id\", \"Name\", \"ShortName\", \"Description\")  VALUES('%1', '%2', '%3', '%4');")
				.arg(qPrintable(proposedObjectId))
				.arg(name)
				.arg(sname)
				.arg(description)
				.toUtf8();
	retVal.objectName = name;

	return retVal;
}


QByteArray CAddressTypeDatabaseDelegateComp::CreateDeleteObjectsQuery(
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
						"DELETE FROM \"AddressTypes\" WHERE \"Id\" IN (%1);")
						.arg(
							quotedIds.join(", ")
							);

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
	const IAddressTypeInfo* adrInfoPtr = dynamic_cast<const IAddressTypeInfo*>(&object);
	if (adrInfoPtr == nullptr || objectId.isEmpty()){
		return QByteArray();
	}
	QByteArray adrId = adrInfoPtr->GetId();
	QString name = adrInfoPtr->GetName();
	QString shortName = adrInfoPtr->GetShortName();
	QString description = adrInfoPtr->GetDescription();

	if (adrId.isEmpty() && !objectId.isEmpty()){
		adrId = objectId;
	}
	if (adrId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"AddressTypes\" SET \"Id\" = '%1', \"Name\" = '%2', \"ShortName\" = '%3', \"Description\" = '%4' WHERE \"Id\" ='%5';")
				.arg(qPrintable(adrId))
				.arg(name)
				.arg(shortName)
				.arg(description)
				.arg(qPrintable(objectId))
				.toUtf8();

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


