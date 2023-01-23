#include <imtlicdb/CProductInstanceDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/ILicenseInstance.h>
#include <imtlic/CProductInstanceMetaInfo.h>
#include <imtdb/CDatabaseEngineComp.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CProductInstanceDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
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

	// Query for getting licenses inside of the product instance:
	QByteArray productLicenses = QString("SELECT * from \"ProductInstanceLicenses\" WHERE InstanceId = '%1' AND ProductId = '%2'")
			.arg(qPrintable(productInstanceId))
			.arg(qPrintable(productId))
			.toUtf8();

	QSqlError error;
	QSqlQuery productLicensesQuery = m_databaseEngineCompPtr->ExecSqlQuery(productLicenses, &error, true);

	if (error.type() != QSqlError::NoError){
		return nullptr;
	}

	QList<QSqlRecord> records;

	while (productLicensesQuery.next()){
		records.append(productLicensesQuery.record());
	}

	for (const QSqlRecord& record : records){
		QByteArray licenseId;

		if (record.contains("LicenseId")){
			licenseId = record.value("LicenseId").toByteArray();
		}

		QDateTime expirationDate;
		if (record.contains("ExpirationDate")){
			expirationDate = record.value("ExpirationDate").toDateTime();
		}

		productInstancePtr->AddLicense(licenseId, expirationDate);
	}

	return productInstancePtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CProductInstanceDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	const imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(valuePtr);
	if (productInstancePtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray productId = productInstancePtr->GetProductId();
	if (productId.isEmpty()){
		return NewObjectQuery();
	}

	QByteArray accountId = productInstancePtr->GetCustomerId();
	if (accountId.isEmpty()){
		return NewObjectQuery();
	}

	QByteArray productInstanceId = productInstancePtr->GetProductInstanceId();
	if (productInstanceId.isEmpty()){
		productInstanceId = objectName.toLocal8Bit();
	}

	QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

	NewObjectQuery retVal;

	retVal.query = QString("INSERT INTO \"ProductInstances\" (InstanceId, ProductId, AccountId, Name, Description, Added, LastModified) VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7');")
				.arg(qPrintable(productInstanceId))
				.arg(qPrintable(productId))
				.arg(qPrintable(accountId))
				.arg(objectName)
				.arg(objectDescription)
				.arg(timestamp)
				.arg(timestamp)
				.toLocal8Bit();

	retVal.objectName = objectName;

	const imtbase::ICollectionInfo& licenseList = productInstancePtr->GetLicenseInstances();

	imtbase::ICollectionInfo::Ids licenseIds = licenseList.GetElementIds();
	for (const QByteArray& licenseCollectionId : licenseIds){
		const imtlic::ILicenseInstance* licensePtr = productInstancePtr->GetLicenseInstance(licenseCollectionId);
		if (licensePtr != nullptr){
			QByteArray licenseId = licensePtr->GetLicenseId();
			QDateTime expirationTime = licensePtr->GetExpiration();

			if (expirationTime.isNull()){
				retVal.query += "\n" + QString("INSERT INTO \"ProductInstanceLicenses\" (InstanceId, LicenseId, ProductId) VALUES('%1', '%2', '%3');")
							.arg(qPrintable(productInstanceId))
							.arg(qPrintable(licenseId))
							.arg(qPrintable(productId))
							.toLocal8Bit();
			}
			else{
				retVal.query += "\n" + QString("INSERT INTO \"ProductInstanceLicenses\" (InstanceId, LicenseId, ProductId, ExpirationDate) VALUES('%1', '%2', '%3', '%4');")
							.arg(qPrintable(productInstanceId))
							.arg(qPrintable(licenseId))
							.arg(qPrintable(productId))
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

		QByteArray productId = productInstancePtr->GetProductId();
		if (productId.isEmpty()){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM \"ProductInstances\" WHERE InstanceId = '%1' AND ProductId = '%2';")
				.arg(qPrintable(productInstanceId))
				.arg(qPrintable(productId))
				.toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CProductInstanceDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	const imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(&object);
	if (productInstancePtr == nullptr){
		return QByteArray();
	}

	const imtlic::IProductInstanceInfo* oldProductInstancePtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		oldProductInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(objectPtr.GetPtr());
	}

	if (oldProductInstancePtr == nullptr){
		return QByteArray();
	}

	QByteArray oldProductInstanceId = oldProductInstancePtr->GetProductInstanceId();
	if (oldProductInstanceId.isEmpty()){
		return QByteArray();
	}

	QByteArray newProductInstanceId = productInstancePtr->GetProductInstanceId();
	if (newProductInstanceId.isEmpty()){
		return QByteArray();
	}

	QByteArray oldProductId = oldProductInstancePtr->GetProductId();
	if (oldProductId.isEmpty()){
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

	QString objectName = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	QString objectDescription = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

	QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

	QByteArray retVal = QString("UPDATE \"ProductInstances\" SET InstanceId = '%1', ProductId = '%2', AccountId = '%3', Name = '%4', Description = '%5', LastModified = '%6' WHERE InstanceId = '%7' AND ProductId = '%8';")
							.arg(qPrintable(newProductInstanceId))
							.arg(qPrintable(productId))
							.arg(qPrintable(accountId))
							.arg(objectName)
							.arg(objectDescription)
							.arg(timestamp)
							.arg(qPrintable(oldProductInstanceId))
							.arg(qPrintable(oldProductId))
							.toLocal8Bit();

	QByteArrayList addedLicenses;
	QByteArrayList removedLicenses;
	QByteArrayList updatedLicenses;

	GenerateDifferencesLicenses(oldProductInstancePtr, productInstancePtr, addedLicenses, removedLicenses, updatedLicenses);

	// Add new license to the product:
	for (const QByteArray& addLicenseId : addedLicenses){
		const imtlic::ILicenseInstance* licensePtr = productInstancePtr->GetLicenseInstance(addLicenseId);
		if (licensePtr != nullptr){
			QByteArray licenseId = licensePtr->GetLicenseId();
			QDateTime expirationTime = licensePtr->GetExpiration();

			if (expirationTime.isNull()){
				retVal += "\n" + QString("INSERT INTO \"ProductInstanceLicenses\" (InstanceId, LicenseId, ProductId) VALUES('%1', '%2', '%3');")
							.arg(qPrintable(newProductInstanceId))
							.arg(qPrintable(licenseId))
							.arg(qPrintable(productId))
							.toLocal8Bit();
			}
			else{
				retVal += "\n" + QString("INSERT INTO \"ProductInstanceLicenses\" (InstanceId, LicenseId, ProductId, ExpirationDate) VALUES('%1', '%2', '%3', '%4');")
							.arg(qPrintable(newProductInstanceId))
							.arg(qPrintable(licenseId))
							.arg(qPrintable(productId))
							.arg(expirationTime.date().toString(Qt::ISODate))
							.toLocal8Bit();
			}
		}
	}

	// Delete removed licenses to the product instance:
	for (const QByteArray& removedLicenseId : removedLicenses){
		const imtlic::ILicenseInstance* licensePtr = oldProductInstancePtr->GetLicenseInstance(removedLicenseId);
		if (licensePtr != nullptr){
			QByteArray licenseId = licensePtr->GetLicenseId();
			retVal += "\n" +
					QString("DELETE FROM \"ProductInstanceLicenses\" WHERE InstanceId = '%1' AND LicenseId = '%2';")
							.arg(qPrintable(oldProductInstanceId))
							.arg(qPrintable(licenseId))
							.toLocal8Bit();
		}
	}

	// Update changed licenses in the product instance:
	for (const QByteArray& updatedLicenseId : updatedLicenses){
		const imtlic::ILicenseInstance* licensePtr = productInstancePtr->GetLicenseInstance(updatedLicenseId);
		if (licensePtr != nullptr){
			QByteArray licenseId = licensePtr->GetLicenseId();
			QString expirationDataString = QString("'%1'").arg(licensePtr->GetExpiration().date().toString(Qt::ISODate));
			if(expirationDataString == "''"){
				expirationDataString = "null";
			}
			retVal += "\n" +
				QString("UPDATE \"ProductInstanceLicenses\" SET ExpirationDate = %1 WHERE  InstanceId = '%2' AND LicenseId = '%3' AND ProductId = '%4';")
							.arg(expirationDataString)
							.arg(qPrintable(newProductInstanceId))
							.arg(qPrintable(licenseId))
							.arg(qPrintable(productId))
							.toLocal8Bit();
		}

	}

	return retVal;
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

	QByteArray productId = productInstancePtr->GetProductId();
	if (productId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"ProductInstances\" SET Name = '%1', LastModified = '%2' WHERE InstanceId ='%3' AND ProductId ='%4';")
			.arg(newObjectName)
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.arg(qPrintable(productInstanceId))
			.arg(qPrintable(productId))
			.toLocal8Bit();

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

	QByteArray productId = productInstancePtr->GetProductId();
	if (productId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"ProductInstances\" SET Description = '%1', LastModified = '%2' WHERE InstanceId ='%3' AND ProductId ='%4';")
			.arg(description)
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.arg(qPrintable(productInstanceId))
			.arg(qPrintable(productId))
			.toLocal8Bit();

	return retVal;
}


QByteArray CProductInstanceDatabaseDelegateComp::GetObjectIdFromRecord(const QSqlRecord& record) const
{
	QByteArray objectId;

	if (record.contains("InstanceId")){
		objectId = record.value("InstanceId").toByteArray();
	}

	if (m_separatorObjectIdAttrPtr.IsValid()){
		objectId += *m_separatorObjectIdAttrPtr;
	}

	if (record.contains("ProductId")){
		objectId += record.value("ProductId").toByteArray();
	}

	return objectId;
}


QByteArray CProductInstanceDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		QString objectIdStr = objectId;

		QByteArray instanceId, productId;

		if (m_separatorObjectIdAttrPtr.IsValid()){
			QStringList splitData = objectIdStr.split(*m_separatorObjectIdAttrPtr);

			instanceId = splitData[0].toUtf8();
			productId = splitData[1].toUtf8();
		}

		return QString("SELECT * FROM \"%1\" WHERE InstanceId = '%2' AND ProductId = '%3'")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(instanceId))
					.arg(qPrintable(productId))
					.toLocal8Bit();
	}
	else{
		QString sortQuery;
		QString filterQuery;
		if (count == 0){
			return QByteArray();
		}
		if (paramsPtr != nullptr){
			if (!CreateFilterQuery(*paramsPtr, filterQuery)){
				return QByteArray();
			}

			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(paramsPtr, "Filter");
			if (collectionFilterParamPtr.IsValid()){
				if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
					return QByteArray();
				}
			}
		}

		QByteArray paginationQuery;
		if (!CreatePaginationQuery(offset, count, paginationQuery)){
			return QByteArray();
		}

		// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
		QString retVal = "(SELECT * FROM";
		retVal += QString(" \"") + qPrintable(*m_tableNameAttrPtr);
		retVal += QString("\" ") + filterQuery;
		retVal += QString(" ") + qPrintable(paginationQuery) + ")";
		retVal += QString(" ") + sortQuery;

		return retVal.toLocal8Bit();
	}

	return QByteArray();
}


void CProductInstanceDatabaseDelegateComp::GenerateDifferencesLicenses(const imtlic::IProductInstanceInfo *currentProductInstancePtr,
															   const imtlic::IProductInstanceInfo *newProductInstancePtr,
															   QByteArrayList &addLicenseInstances,
															   QByteArrayList &removedLicenseInstances,
															   QByteArrayList &updatedLicenseInstances) const
{
	imtbase::ICollectionInfo::Ids currentLicenseIds = currentProductInstancePtr->GetLicenseInstances().GetElementIds();
	for (const QByteArray& licenseCollectionId : currentLicenseIds){
		const imtlic::ILicenseInstance* licensePtr = currentProductInstancePtr->GetLicenseInstance(licenseCollectionId);
		if (licensePtr != nullptr && !currentLicenseIds.contains(licenseCollectionId)){
			currentLicenseIds.push_back(licenseCollectionId);
		}
	}

	imtbase::ICollectionInfo::Ids newLicenseIds = newProductInstancePtr->GetLicenseInstances().GetElementIds();
	int count = newProductInstancePtr->GetLicenseInstances().GetElementsCount();
	for (const QByteArray& licenseCollectionId : newLicenseIds){
		const imtlic::ILicenseInstance* licensePtr = newProductInstancePtr->GetLicenseInstance(licenseCollectionId);
		if (licensePtr != nullptr && !newLicenseIds.contains(licenseCollectionId)){
			newLicenseIds.push_back(licenseCollectionId);
		}
	}

	// Calculate added features:
	for (QByteArray newLicenseId : newLicenseIds){
		if (!currentLicenseIds.contains(newLicenseId)){
			addLicenseInstances.push_back(newLicenseId);
		}
	}

	// Calculate removed features:
	for (QByteArray currentLicenseId : currentLicenseIds){
		if (!newLicenseIds.contains(currentLicenseId)){
			removedLicenseInstances.push_back(currentLicenseId);
		}
	}

	// Calculate changed features:
	for (QByteArray currentLicenseId : currentLicenseIds){
		// Feature-ID in both packages:
		if (newLicenseIds.contains(currentLicenseId)){
			const imtlic::ILicenseInstance* currentLicenseInstancePtr = currentProductInstancePtr->GetLicenseInstance(currentLicenseId);
			Q_ASSERT(currentLicenseInstancePtr != nullptr);

			const imtlic::ILicenseInstance* newLicenseInstancePtr = newProductInstancePtr->GetLicenseInstance(currentLicenseId);
			Q_ASSERT(newLicenseInstancePtr != nullptr);

			if (!newLicenseInstancePtr->IsEqual(*currentLicenseInstancePtr)){
				updatedLicenseInstances.push_back(currentLicenseId);
			}
		}
	}
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CProductInstanceDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new imod::TModelWrap<imtlic::CProductInstanceMetaInfo>);
}


bool CProductInstanceDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	const istd::IChangeable* instancePtr = CreateObjectFromRecord(record);
	if ((instancePtr != nullptr) && m_metaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr retVal;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(instancePtr, "ProductInstanceInfo", retVal)){
			Q_ASSERT(retVal.IsValid());

			return metaInfo.CopyFrom(*retVal);
		}
	}

	return false;
}


} // namespace imtlicdb


