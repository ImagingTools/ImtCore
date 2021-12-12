#include <imtlicdb/CProductInstanceDatabaseDelegateComp.h>


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

	//QByteArray selectProductLicenses = QString("SELECT * from ProductInstances WHERE ProductInstanceId = '%1'").arg(qPrintable(productInstanceId)).toUtf8();

	//QSqlError error;
	//QSqlQuery productLicensesQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectProductLicenses, &error);

	//while (productLicensesQuery.next()){
	//	QSqlRecord licenseRecord = productLicensesQuery.record();
	//	QByteArray licenseId;
	//	QString licenseName;
	//	QString description;

	//	if (licenseRecord.contains("Id")){
	//		licenseId = licenseRecord.value("Id").toByteArray();
	//	}

	//	if (licenseRecord.contains("Name")){
	//		licenseName = licenseRecord.value("Name").toString();
	//	}

	//	if (licenseRecord.contains("Description")){
	//		description = licenseRecord.value("Description").toString();
	//	}

	//	istd::TDelPtr<imtlic::CLicenseInfo> licenseInfoPtr = new imtlic::CLicenseInfo;
	//	licenseInfoPtr->SetLicenseName(licenseName);
	//	licenseInfoPtr->SetLicenseId(licenseId);

	//	QByteArray selectLicenseFeatures = QString("SELECT * from ProductLicenseFeatures WHERE LicenseId = '%1'").arg(qPrintable(licenseId)).toUtf8();

	//	QSqlQuery licenseFeatureQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectLicenseFeatures, &error);

	//	imtlic::ILicenseInfo::FeatureInfos featureInfos;

	//	while (licenseFeatureQuery.next()){
	//		QSqlRecord licenseFeatureRecord = licenseFeatureQuery.record();

	//		imtlic::ILicenseInfo::FeatureInfo featureInfo;

	//		if (licenseFeatureRecord.contains("FeatureId")){
	//			featureInfo.id = licenseFeatureRecord.value("FeatureId").toByteArray();
	//		}

	//		if (featureInfo.id.isEmpty()){
	//			return nullptr;
	//		}

	//		featureInfos.push_back(featureInfo);
	//	}

	//	licenseInfoPtr->SetFeatureInfos(featureInfos);

	//	licenseCollectionPtr->InsertNewObject(imtlic::CLicenseInfo::GetTypeId(), licenseName, description, licenseInfoPtr.GetPtr());
	//}

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

	QByteArray retVal = QString("INSERT INTO ProductInstances(InstanceId, ProductId, AccountId, Name, Description) VALUES('%1', '%2', '%3', '%4', '%5');")
				.arg(qPrintable(productInstanceId))
				.arg(qPrintable(productId))
				.arg(qPrintable(accountId))
				.arg(objectName)
				.arg(objectDescription)
				.toLocal8Bit();

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

		QByteArray retVal = QString("DELETE FROM ProductInstances WHERE Id = '%1';").arg(qPrintable(productInstanceId)).toLocal8Bit();

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

	QByteArray retVal = QString("UPDATE ProductInstances SET Name = '%1' WHERE Id ='%2';").arg(newObjectName).arg(qPrintable(productInstanceId)).toLocal8Bit();

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

	QByteArray retVal = QString("UPDATE ProductInstances SET Description = '%1' WHERE Id ='%2';").arg(description).arg(qPrintable(productInstanceId)).toLocal8Bit();

	return retVal;
}


} // namespace imtlicdb


