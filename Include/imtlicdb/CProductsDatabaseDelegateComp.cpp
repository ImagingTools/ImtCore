#include <imtlicdb/CProductsDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/IFeaturePackage.h>
#include <imtlic/CLicenseInfo.h>
#include <imtlic/CFeaturePackageCollectionUtility.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CProductsDatabaseDelegateComp::CreateObjectFromRecord(
		const QByteArray& /*typeId*/,
		const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_productFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtlic::IProductLicensingInfo> productPtr = m_productFactCompPtr.CreateInstance();
	if (!productPtr.IsValid()){
		return nullptr;
	}

	imtbase::IObjectCollection* licenseCollectionPtr = dynamic_cast<imtbase::IObjectCollection*>(productPtr.GetPtr());
	if (licenseCollectionPtr == nullptr){
		return nullptr;
	}

	QByteArray productId;
	if (record.contains("Id")){
		productId = record.value("Id").toByteArray();

		productPtr->SetProductId(productId);
	}

	QString productName;
	if (record.contains("Name")){
		productName = record.value("Name").toString();

		productPtr->SetName(productName);
	}

	QByteArray selectProductLicenses = QString("SELECT * from ProductLicenses WHERE ProductId = '%1'").arg(qPrintable(productId)).toUtf8();

	QSqlError error;
	QSqlQuery productLicensesQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectProductLicenses, &error);

	while (productLicensesQuery.next()){
		QSqlRecord licenseRecord = productLicensesQuery.record();
		QByteArray licenseId;
		QString licenseName;
		QString description;

		if (licenseRecord.contains("Id")){
			licenseId = licenseRecord.value("Id").toByteArray();
		}

		if (licenseRecord.contains("Name")){
			licenseName = licenseRecord.value("Name").toString();
		}

		if (licenseRecord.contains("Description")){
			description = licenseRecord.value("Description").toString();
		}

		istd::TDelPtr<imtlic::CLicenseInfo> licenseInfoPtr = new imtlic::CLicenseInfo;
		licenseInfoPtr->SetLicenseName(licenseName);
		licenseInfoPtr->SetLicenseId(licenseId);

		QByteArray selectLicenseFeatures = QString("SELECT * from ProductLicenseFeatures WHERE LicenseId = '%1'").arg(qPrintable(licenseId)).toUtf8();

		QSqlQuery licenseFeatureQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectLicenseFeatures, &error);

		imtlic::ILicenseInfo::FeatureInfos featureInfos;

		while (licenseFeatureQuery.next()){
			QSqlRecord licenseFeatureRecord = licenseFeatureQuery.record();

			imtlic::ILicenseInfo::FeatureInfo featureInfo;

			QByteArray featureId, packageId;
			if (licenseFeatureRecord.contains("FeatureId")){
//				featureInfo.id = licenseFeatureRecord.value("FeatureId").toByteArray();
				featureId = licenseFeatureRecord.value("FeatureId").toByteArray();
			}

			if (licenseFeatureRecord.contains("PackageId")){
//				featureInfo.id = licenseFeatureRecord.value("PackageId").toByteArray();
				packageId = licenseFeatureRecord.value("PackageId").toByteArray();
			}

			featureInfo.id = packageId + '.' + featureId;

			if (featureInfo.id.isEmpty()){
				return nullptr;
			}

			featureInfos.push_back(featureInfo);
		}

		licenseInfoPtr->SetFeatureInfos(featureInfos);

		licenseCollectionPtr->InsertNewObject(imtlic::CLicenseInfo::GetTypeId(), licenseName, description, licenseInfoPtr.GetPtr(), licenseId);
	}

	return productPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CProductsDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(valuePtr);
	if (productPtr == nullptr){
		return NewObjectQuery();
	}

	const imtbase::IObjectCollection* packagesCollectionPtr = productPtr->GetFeaturePackages();
	if (packagesCollectionPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray productId = productPtr->GetProductId();
	if (productId.isEmpty()){
		productId = objectName.toLocal8Bit();
	}

	if (productId.isEmpty()){
		return NewObjectQuery();
	}

	QString productName = productPtr->GetName();
	if (productName.isEmpty()){
		productName = objectName;
	}

	NewObjectQuery retVal;
	retVal.query = QString("INSERT INTO Products(Id, Name, Description) VALUES('%1', '%2', '%3');")
				.arg(qPrintable(productId))
				.arg(productName)
				.arg(objectDescription).toLocal8Bit();
	retVal.objectName = productName;

	imtbase::ICollectionInfo::Ids licenseIds = productPtr->GetLicenseList().GetElementIds();
	for (const QByteArray& collectionId : licenseIds){
		const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(collectionId);
		if (licenseInfoPtr != nullptr){
			QByteArray licenseId = licenseInfoPtr->GetLicenseId();
			QString licenseName = licenseInfoPtr->GetLicenseName();
			QString licenseDescription = productPtr->GetLicenseList().GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			retVal.query += "\n" +
						QString("INSERT INTO ProductLicenses(Id, Name, Description, ProductId) VALUES('%1', '%2', '%3', '%4');")
						.arg(qPrintable(licenseId))
						.arg(licenseName)
						.arg(licenseDescription)
						.arg(qPrintable(productId)).toLocal8Bit();


			imtlic::ILicenseInfo::FeatureInfos featureInfos = licenseInfoPtr->GetFeatureInfos();
			for (const imtlic::ILicenseInfo::FeatureInfo& featureInfo : featureInfos){
				QByteArray packageId;

				const imtlic::IFeaturePackage* packagePtr = imtlic::CFeaturePackageCollectionUtility::GetFeaturePackagePtr(*packagesCollectionPtr, featureInfo.id);
				if (packagePtr == nullptr){
					return NewObjectQuery();
				}

				packageId = packagePtr->GetPackageId();

				retVal.query += "\n" +
							QString("INSERT INTO ProductLicenseFeatures(LicenseId, FeatureId) VALUES('%1', '%2');")
							.arg(qPrintable(licenseId))
							.arg(qPrintable(featureInfo.id))
							.toLocal8Bit();
			}
		}
	}

	return retVal;
}


QByteArray CProductsDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(objectPtr.GetPtr());
		if (productPtr == nullptr){
			return QByteArray();
		}

		QByteArray productId = productPtr->GetProductId();
		if (productId.isEmpty()){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM Products WHERE Id = '%1';").arg(qPrintable(productId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CProductsDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	const imtlic::IProductLicensingInfo* newProductPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(&object);
	if (newProductPtr == nullptr){
		return QByteArray();
	}

	const imtlic::IProductLicensingInfo* oldProductPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		oldProductPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(objectPtr.GetPtr());
	}

	if (oldProductPtr == nullptr){
		return QByteArray();
	}

	QByteArray oldProductId = oldProductPtr->GetProductId();
	QByteArray newProductId = newProductPtr->GetProductId();

	QString oldProductName = oldProductPtr->GetName();
	QString newProductName = newProductPtr->GetName();

	QByteArray retVal = QString("UPDATE Products SET Id ='%1', Name = '%2' WHERE Id ='%3';")
			.arg(qPrintable(newProductId))
			.arg(qPrintable(newProductName))
			.arg(qPrintable(oldProductId)).toLocal8Bit();

	QByteArrayList addedLicenses;
	QByteArrayList removedLicenses;
	QByteArrayList updatedLicenses;

	GenerateDifferences(oldProductPtr, newProductPtr, addedLicenses, removedLicenses, updatedLicenses);

	// Add new license to the product:
	for (const QByteArray& collectionLicenseId : addedLicenses){
		const imtlic::ILicenseInfo* licenseInfoPtr = newProductPtr->GetLicenseInfo(collectionLicenseId);
		if (licenseInfoPtr != nullptr){
			QString licenseName = licenseInfoPtr->GetLicenseName();
			QString licenseDescription = newProductPtr->GetLicenseList().GetElementInfo(
						collectionLicenseId,
						imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			QByteArray licenseId = licenseInfoPtr->GetLicenseId();

			retVal += "\n" +
					QString("INSERT INTO ProductLicenses(Id, Name, Description, ProductId) VALUES('%1', '%2', '%3', '%4');")
								.arg(qPrintable(licenseId))
								.arg(licenseName)
								.arg(licenseDescription)
								.arg(qPrintable(newProductId)).toLocal8Bit();
		}
	}

	// Delete removed licenses to the product:
	for (const QByteArray& collectionLicenseId : removedLicenses){
		const imtlic::ILicenseInfo* licenseInfoPtr = oldProductPtr->GetLicenseInfo(collectionLicenseId);
		if (licenseInfoPtr != nullptr){
			QByteArray licenseId = licenseInfoPtr->GetLicenseId();
			retVal += "\n" +
						QString("DELETE FROM ProductLicenses WHERE Id = '%1' AND ProductId = '%2';")
									.arg(qPrintable(licenseId))
									.arg(qPrintable(oldProductId)).toLocal8Bit();
		}
	}

	// Update changed licenses in the product:
	for (const QByteArray& collectionLicenseId : updatedLicenses){
		const imtlic::ILicenseInfo* newLicenseInfoPtr = newProductPtr->GetLicenseInfo(collectionLicenseId);
		const imtlic::ILicenseInfo* oldLicenseInfoPtr = oldProductPtr->GetLicenseInfo(collectionLicenseId);
		QByteArray productId = newProductPtr->GetProductId();
		if (newLicenseInfoPtr != nullptr && oldLicenseInfoPtr != nullptr){
			QString licenseName = newLicenseInfoPtr->GetLicenseName();
			QString licenseDescription = newProductPtr->GetLicenseList().GetElementInfo(
						collectionLicenseId,
						imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			QByteArray newLicenseId = newLicenseInfoPtr->GetLicenseId();
			QByteArray oldLicenseId = oldLicenseInfoPtr->GetLicenseId();
			//const imtbase::IObjectCollection* packagesCollectionPtr = newProductPtr->GetFeaturePackages();

			retVal += "\n" +
						QString("UPDATE ProductLicenses SET Id = '%1', Name = '%2', Description = '%3' WHERE Id = '%4' AND ProductId = '%5';")
									.arg(qPrintable(newLicenseId))
									.arg(licenseName)
									.arg(licenseDescription)
									.arg(qPrintable(oldLicenseId))
									.arg(qPrintable(newProductId)).toLocal8Bit();

			QByteArrayList addedFeatures;
			QByteArrayList removedFeatures;
			QByteArrayList updatedFeatures;

			GenerateDifferences(oldLicenseInfoPtr, newLicenseInfoPtr, addedFeatures, removedFeatures);

			// Add new features to the license:
			for (const QByteArray& addedFeatureId : addedFeatures){
//				const imtlic::IFeaturePackage* packagePtr = imtlic::CFeaturePackageCollectionUtility::GetFeaturePackagePtr(*packagesCollectionPtr, addedFeatureId);
//				if (packagePtr == nullptr){
//					continue;
//				}
				QByteArrayList data = addedFeatureId.split('.');

				QByteArray featureId = data[1];
				QByteArray packageId = data[0];

				retVal += "\n" +
							QString("INSERT INTO ProductLicenseFeatures(ProductId, LicenseId, PackageId, FeatureId) VALUES('%1', '%2', '%3', '%4');")
							.arg(qPrintable(productId))
							.arg(qPrintable(collectionLicenseId))
							.arg(qPrintable(packageId))
							.arg(qPrintable(featureId))
							.toLocal8Bit();
			}

			// Delete removed features to the license:
			for (const QByteArray& removedFeatureId : removedFeatures){
				QByteArrayList data = removedFeatureId.split('.');
				QByteArray featureId = data[1];

				retVal += "\n" +
							QString("DELETE FROM ProductLicenseFeatures WHERE FeatureId = '%1' AND LicenseId = '%2';")
							.arg(qPrintable(featureId))
							.arg(qPrintable(collectionLicenseId)).toLocal8Bit();
			}
		}
	}

	return retVal;
}


QByteArray CProductsDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName) const
{
	const imtlic::IProductLicensingInfo* productPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(objectPtr.GetPtr());
	}

	if (productPtr == nullptr){
		return QByteArray();
	}

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray oldProductId = productPtr->GetProductId();
	QByteArray newProductId = newObjectName.toLocal8Bit();

//	QByteArray retVal = QString("UPDATE Products SET Id ='%1', Name = '%1' WHERE Id ='%2';")
//			.arg(qPrintable(newProductId))
//			.arg(qPrintable(oldProductId)).toLocal8Bit();

	QByteArray retVal = QString("UPDATE Products SET Name = '%1' WHERE Id ='%2';")
			.arg(qPrintable(newObjectName))
			.arg(qPrintable(oldProductId)).toLocal8Bit();

	return retVal;
}


QByteArray CProductsDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& description) const
{
	const imtlic::IProductLicensingInfo* productPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(objectPtr.GetPtr());
	}

	if (productPtr == nullptr){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE Products SET Description = '%1' WHERE Id ='%2';").arg(description).arg(qPrintable(productPtr->GetProductId())).toLocal8Bit();

	return retVal;
}


void CProductsDatabaseDelegateComp::GenerateDifferences(
			const imtlic::IProductLicensingInfo* currentProductPtr,
			const imtlic::IProductLicensingInfo* newProductPtr,
			QByteArrayList& addLicenses,
			QByteArrayList& removedLicenses,
			QByteArrayList& updatedLicenses) const
{
	imtbase::ICollectionInfo::Ids currentLicenseCollectionIds = currentProductPtr->GetLicenseList().GetElementIds();
	QByteArrayList currentLicenseIds;

	for (QByteArray id : currentLicenseCollectionIds){
		const imtlic::ILicenseInfo* licenseInfoPtr = currentProductPtr->GetLicenseInfo(id);
		Q_ASSERT(licenseInfoPtr != nullptr);
		Q_ASSERT(!licenseInfoPtr->GetLicenseId().isEmpty());

		currentLicenseIds.push_back(id);
	}

	imtbase::ICollectionInfo::Ids newLicenseCollectionIds = newProductPtr->GetLicenseList().GetElementIds();
	QByteArrayList newLicenseIds;

	for (QByteArray id : newLicenseCollectionIds){
		const imtlic::ILicenseInfo* licenseInfoPtr = newProductPtr->GetLicenseInfo(id);
		Q_ASSERT(licenseInfoPtr != nullptr);
		Q_ASSERT(!licenseInfoPtr->GetLicenseId().isEmpty());

		newLicenseIds.push_back(id);
	}

	// Calculate added licenses:
	for (QByteArray newLicenseId : newLicenseIds){
		if (!currentLicenseIds.contains(newLicenseId)){
			addLicenses.push_back(newLicenseId);
		}
	}

	// Calculate removed licenses:
	for (QByteArray currentLicenseId : currentLicenseIds){
		if (!newLicenseIds.contains(currentLicenseId)){
			removedLicenses.push_back(currentLicenseId);
		}
	}

	// Calculate changed licenses:
	for (QByteArray currentLicenseId : currentLicenseIds){
		// License-ID in both licenses:
		if (newLicenseIds.contains(currentLicenseId)){
			const imtlic::ILicenseInfo* currentLicensePtr = currentProductPtr->GetLicenseInfo(currentLicenseId);
			Q_ASSERT(currentLicensePtr != nullptr);

			const imtlic::ILicenseInfo* newLicensePtr = newProductPtr->GetLicenseInfo(currentLicenseId);
			Q_ASSERT(newLicensePtr != nullptr);

			if (!newLicensePtr->IsEqual(*currentLicensePtr)){
				updatedLicenses.push_back(currentLicenseId);
			}
			else{
				QString newLicenseDescription = newProductPtr->GetLicenseList().GetElementInfo(currentLicenseId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

				QString currentLicenseDescription = currentProductPtr->GetLicenseList().GetElementInfo(currentLicenseId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

				if (newLicenseDescription != currentLicenseDescription){
					updatedLicenses.push_back(currentLicenseId);
				}
			}
		}
	}

}


void CProductsDatabaseDelegateComp::GenerateDifferences(
			const imtlic::ILicenseInfo* currentLicensePtr,
			const imtlic::ILicenseInfo* newLicensePtr,
			QByteArrayList& addFeatures,
			QByteArrayList& removedFeatures) const
{
	QByteArrayList currentFeatureIds;
	imtlic::ILicenseInfo::FeatureInfos currentFeatures = currentLicensePtr->GetFeatureInfos();

	for (const imtlic::ILicenseInfo::FeatureInfo& featureInfo : currentFeatures){
		currentFeatureIds.push_back(featureInfo.id);
	}

	QByteArrayList newFeatureIds;
	imtlic::ILicenseInfo::FeatureInfos newFeatures = newLicensePtr->GetFeatureInfos();

	for (const imtlic::ILicenseInfo::FeatureInfo& featureInfo : newFeatures){
//		QByteArray newId = featureInfo.id.split('.')[1];
		newFeatureIds.push_back(featureInfo.id);
	}

	// Calculate added features:
	for (QByteArray newFeatureId : newFeatureIds){
		if (!currentFeatureIds.contains(newFeatureId)){
			addFeatures.push_back(newFeatureId);
		}
	}

	// Calculate removed features:
	for (QByteArray currentFeatureId : currentFeatureIds){
		if (!newFeatureIds.contains(currentFeatureId)){
			removedFeatures.push_back(currentFeatureId);
		}
	}
}


} // namespace imtlicdb


