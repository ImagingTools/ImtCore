#include <imtlicdb/CProductInstanceDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/ILicenseInstance.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::IDatabaseObjectDelegate)

istd::IChangeable* CProductInstanceDatabaseDelegateComp::CreateObjectFromRecord(
			const QByteArray& /*typeId*/,
			const QSqlRecord& record,
			QByteArray& objectId,
			QString& objectName,
			QString& objectDescription,
			QDateTime& lastModified,
			QDateTime& added) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_productInstanceFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtlic::IProductInstanceInfo> productInstancePtr = m_productInstanceFactCompPtr.CreateInstance();
	if (!productInstancePtr.IsValid()){
		return nullptr;
	}

	QByteArray productInstanceId;
	if (record.contains("InstanceId")){
		productInstanceId = record.value("InstanceId").toByteArray();
	}

	QByteArray productId;
	if (record.contains("ProductId")){
		productId = record.value("ProductId").toByteArray();
	}

	QByteArray accountId;
	if (record.contains("AccountId")){
		accountId = record.value("AccountId").toByteArray();
	}

	productInstancePtr->SetupProductInstance(productId, productInstanceId, accountId);

	QString productName;
	if (record.contains("Name")){
		productName = record.value("Name").toString();

		objectName = productName;
	}

	if (record.contains("Description")){
		objectDescription = record.value("Description").toString();
	}

	if (record.contains("Added")){
		added = record.value("Added").toDateTime();
	}

	if (record.contains("LastModified")){
		lastModified = record.value("LastModified").toDateTime();
	}

	// Query for getting licenses inside of the product instance:
	QByteArray productLicenses = QString("SELECT * from ProductInstanceLicenses WHERE InstanceId = '%1'").arg(qPrintable(productInstanceId)).toUtf8();

	QSqlError error;
	QSqlQuery productLicensesQuery = m_databaseEngineCompPtr->ExecSqlQuery(productLicenses, &error);

	while (productLicensesQuery.next()){
		QSqlRecord licenseRecord = productLicensesQuery.record();
		QByteArray licenseId;

		if (licenseRecord.contains("LicenseId")){
			licenseId = licenseRecord.value("LicenseId").toByteArray();
		}

		QDateTime expirationDate;
		if (licenseRecord.contains("ExpirationDate")){
			expirationDate = licenseRecord.value("ExpirationDate").toDateTime();
		}

		productInstancePtr->AddLicense(licenseId, expirationDate);
	}

	return productInstancePtr.PopPtr();
}


QByteArray CProductInstanceDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	const imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(valuePtr);
	if (productInstancePtr == nullptr){
		return QByteArray();
	}

	QByteArray productInstanceId = productInstancePtr->GetProductInstanceId();
	if (productInstanceId.isEmpty()){
		return QByteArray();
	}

	QByteArray productId = productInstancePtr->GetProductId();
	if (productId.isEmpty()){
		return QByteArray();
	}

	QByteArray accountId = productInstancePtr->GetCustomerId();
	if (accountId.isEmpty()){
		return QByteArray();
	}

	QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

	QByteArray retVal = QString("INSERT INTO ProductInstances(InstanceId, ProductId, AccountId, Name, Description, Added, LastModified) VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7');")
				.arg(qPrintable(productInstanceId))
				.arg(qPrintable(productId))
				.arg(qPrintable(accountId))
				.arg(objectName)
				.arg(objectDescription)
				.arg(timestamp)
				.arg(timestamp)
				.toLocal8Bit();

	const imtbase::ICollectionInfo& licenseList = productInstancePtr->GetLicenseInstances();

	imtbase::ICollectionInfo::Ids licenseIds = licenseList.GetElementIds();

	for (const QByteArray& licenseCollectionId : licenseIds){
		const imtlic::ILicenseInstance* licensePtr = productInstancePtr->GetLicenseInstance(licenseCollectionId);
		if (licensePtr != nullptr){
			QByteArray licenseId = licensePtr->GetLicenseId();
			QDateTime expirationTime = licensePtr->GetExpiration();

			if (expirationTime.isNull()){
				retVal += "\n" + QString("INSERT INTO ProductInstanceLicenses(InstanceId, LicenseId) VALUES('%1', '%2');")
							.arg(qPrintable(productInstanceId))
							.arg(qPrintable(licenseId))
							.toLocal8Bit();
			}
			else{
				retVal += "\n" + QString("INSERT INTO ProductInstanceLicenses(InstanceId, LicenseId, ExpirationDate) VALUES('%1', '%2', '%3');")
							.arg(qPrintable(productInstanceId))
							.arg(qPrintable(licenseId))
							.arg(expirationTime.date().toString(Qt::ISODate))
							.toLocal8Bit();
			}
		}
	}

	return retVal;
}


QByteArray CProductInstanceDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		const imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(objectPtr.GetPtr());
		if (productInstancePtr == nullptr){
			return QByteArray();
		}

		QByteArray productInstanceId = productInstancePtr->GetProductInstanceId();
		if (productInstanceId.isEmpty()){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM ProductInstances WHERE InstanceId = '%1';").arg(qPrintable(productInstanceId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CProductInstanceDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	return QByteArray();
}


QByteArray CProductInstanceDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName) const
{
	const imtlic::IProductInstanceInfo* productInstancePtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(objectPtr.GetPtr());
	}

	if (productInstancePtr == nullptr){
		return QByteArray();
	}

	QByteArray productInstanceId = productInstancePtr->GetProductInstanceId();
	if (productInstanceId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE ProductInstances SET Name = '%1' WHERE InstanceId ='%2';").arg(newObjectName).arg(qPrintable(productInstanceId)).toLocal8Bit();

	return retVal;
}


QByteArray CProductInstanceDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& description) const
{
	const imtlic::IProductInstanceInfo* productInstancePtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(objectPtr.GetPtr());
	}

	if (productInstancePtr == nullptr){
		return QByteArray();
	}

	QByteArray productInstanceId = productInstancePtr->GetProductInstanceId();
	if (productInstanceId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE ProductInstances SET Description = '%1' WHERE InstanceId ='%2';").arg(description).arg(qPrintable(productInstanceId)).toLocal8Bit();

	return retVal;
}


} // namespace imtlicdb


