#include <imtlicdb/CProductsDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/IFeaturePackage.h>
#include <imtlic/CLicenseInfo.h>
#include <imtlic/CFeaturePackageCollectionUtility.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::IDatabaseObjectDelegate)

istd::IChangeable* CProductsDatabaseDelegateComp::CreateObjectFromRecord(
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

		objectId = productId;

		productPtr->SetProductId(productId);
	}

	QString productName;
	if (record.contains("Name")){
		productName = record.value("Name").toString();

		productPtr->SetName(productName);

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

			if (licenseFeatureRecord.contains("FeatureId")){
				featureInfo.id = licenseFeatureRecord.value("FeatureId").toByteArray();
			}

			if (featureInfo.id.isEmpty()){
				return nullptr;
			}

			featureInfos.push_back(featureInfo);
		}

		licenseInfoPtr->SetFeatureInfos(featureInfos);

		licenseCollectionPtr->InsertNewObject(imtlic::CLicenseInfo::GetTypeId(), licenseName, description, licenseInfoPtr.GetPtr());
	}

	return productPtr.PopPtr();
}


QByteArray CProductsDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(valuePtr);
	if (productPtr == nullptr){
		return QByteArray();
	}

	const imtbase::IObjectCollection* packagesCollectionPtr = productPtr->GetFeaturePackages();
	if (packagesCollectionPtr == nullptr){
		return QByteArray();
	}

	QByteArray productId = productPtr->GetProductId();
	if (productId.isEmpty()){
		productId = objectName.toLocal8Bit();
	}

	if (productId.isEmpty()){
		return QByteArray();
	}

	QString productName = productPtr->GetName();
	if (productName.isEmpty()){
		productName = objectName;
	}

	QByteArray retVal = QString("INSERT INTO Products(Id, Name, Description) VALUES('%1', '%2', '%3');")
				.arg(qPrintable(productId))
				.arg(productName)
				.arg(objectDescription).toLocal8Bit();

	imtbase::ICollectionInfo::Ids licenseIds = productPtr->GetLicenseList().GetElementIds();
	for (const QByteArray& collectionId : licenseIds){
		const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(collectionId);
		if (licenseInfoPtr != nullptr){
			QByteArray licenseId = licenseInfoPtr->GetLicenseId();
			QString licenseName = licenseInfoPtr->GetLicenseName();
			QString licenseDescription = productPtr->GetLicenseList().GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			retVal += "\n" +
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
					return QByteArray();
				}

				packageId = packagePtr->GetPackageId();

				retVal += "\n" +
							QString("INSERT INTO ProductLicenseFeatures(LicenseId, FeatureId, PackageId) VALUES('%1', '%2', '%3');")
							.arg(qPrintable(licenseId))
							.arg(qPrintable(featureInfo.id))
							.arg(qPrintable(packageId)).toLocal8Bit();
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
	return QByteArray();
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

	QByteArray retVal = QString("UPDATE Products SET Id ='%1', Name = '%1' WHERE Id ='%2';").arg(qPrintable(newProductId)).arg(qPrintable(oldProductId)).toLocal8Bit();

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


} // namespace imtlicdb


