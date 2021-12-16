#include <imtlicdb/CFeaturePackageDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::IDatabaseObjectDelegate)

QByteArray CFeaturePackageDatabaseDelegateComp::GetSelectionQueryForObject(const QByteArray& objectId) const
{
	if (objectId.isEmpty()){
		return "SELECT * from Packages";
	}
	else{
		return QString("SELECT * from Packages WHERE Id = '%1'").arg(qPrintable(objectId)).toLocal8Bit();
	}
}


istd::IChangeable* CFeaturePackageDatabaseDelegateComp::CreateObjectFromRecord(
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

	istd::TDelPtr<imtlic::CFeaturePackage> featurePackagePtr = new imtlic::CFeaturePackage;

	QByteArray packageId;
	QString packageName;

	if (record.contains("Id")){
		packageId = record.value("Id").toByteArray();
		featurePackagePtr->SetPackageId(packageId);
		objectId = packageId;
	}

	if (record.contains("Name")){
		packageName = record.value("Name").toString();

		objectName = packageName;
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

	QByteArray query = QString("SELECT * from Features WHERE PackageId = '%1'").arg(qPrintable(packageId)).toUtf8();

	QSqlError error;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &error);

	while (sqlQuery.next()){
		QSqlRecord featureRecord = sqlQuery.record();
		QByteArray featureId;
		QString featureName;
		QString description;

		if (featureRecord.contains("Id")){
			featureId = featureRecord.value("Id").toByteArray();
		}

		if (featureRecord.contains("Name")){
			featureName = featureRecord.value("Name").toString();
		}

		if (featureRecord.contains("Description")){
			description = featureRecord.value("Description").toString();
		}

		istd::TDelPtr<imtlic::CFeatureInfo> featureInfoPtr = new imtlic::CFeatureInfo;
		featureInfoPtr->SetFeatureId(featureId);
		featureInfoPtr->SetFeatureName(featureName);

		featurePackagePtr->InsertNewObject("FeatureInfo", featureName, description, featureInfoPtr.GetPtr());
	}

	if (!packageId.isEmpty() && !packageName.isEmpty()){
		return featurePackagePtr.PopPtr();
	}

	return nullptr;
}


QByteArray CFeaturePackageDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr) const
{
	QByteArray packageId;
	const imtlic::CFeaturePackage* featurePackagePtr = dynamic_cast<const imtlic::CFeaturePackage*>(valuePtr);
	if (featurePackagePtr != nullptr){
		packageId = featurePackagePtr->GetPackageId();
		if (packageId.isEmpty()){
			packageId = objectName.toLocal8Bit();
		}

		QByteArray retVal = QString("INSERT INTO Packages(Id, Name) VALUES('%1', '%2');").arg(qPrintable(packageId)).arg(objectName).toLocal8Bit();

		imtbase::ICollectionInfo::Ids featureIds = featurePackagePtr->GetElementIds();
		for (const QByteArray& collectionId : featureIds){
			const imtlic::IFeatureInfo* featureInfoPtr = featurePackagePtr->GetFeatureInfo(collectionId);
			if (featureInfoPtr != nullptr){
				QByteArray featureId = featureInfoPtr->GetFeatureId();
				QString featureName = featureInfoPtr->GetFeatureName();
				QString featureDescription = featurePackagePtr->GetFeatureList().GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				retVal += "\n" + 
							QString("INSERT INTO Features(Id, Name, Description, PackageId) VALUES('%1', '%2', '%3', '%4');")
										.arg(qPrintable(featureId))
										.arg(featureName)
										.arg(featureDescription)
										.arg(qPrintable(packageId)).toLocal8Bit();
			}
		}

		return retVal;
	}

	return QByteArray();
}


QByteArray CFeaturePackageDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		QByteArray packageId;
		const imtlic::CFeaturePackage* featurePackagePtr = dynamic_cast<const imtlic::CFeaturePackage*>(objectPtr.GetPtr());
		if (featurePackagePtr != nullptr){
			packageId = featurePackagePtr->GetPackageId();
			if (!packageId.isEmpty()){
				QByteArray retVal = QString("DELETE FROM Features WHERE PackageId = '%1';").arg(qPrintable(packageId)).toLocal8Bit();
				retVal += "\n" + QString("DELETE FROM Packages WHERE Id = '%1';").arg(qPrintable(packageId)).toLocal8Bit();

				return retVal;
			}
		}
	}

	return QByteArray();
}

QByteArray CFeaturePackageDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	const imtlic::CFeaturePackage* newObjectPtr = dynamic_cast<const imtlic::CFeaturePackage*>(&object);
	if (newObjectPtr == nullptr){
		return QByteArray();
	}

	const imtlic::CFeaturePackage* oldObjectPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		oldObjectPtr = dynamic_cast<const imtlic::CFeaturePackage*>(objectPtr.GetPtr());
	}

	if (oldObjectPtr == nullptr){
		return QByteArray();
	}

	QByteArray oldPackageId = oldObjectPtr->GetPackageId();
	QByteArray newPackageId = newObjectPtr->GetPackageId();

	QByteArray retVal = QString("UPDATE Packages SET Id ='%1' WHERE Id ='%2';").arg(qPrintable(newPackageId)).arg(qPrintable(oldPackageId)).toLocal8Bit();

	QByteArrayList addedFeatures;
	QByteArrayList removedFeatures;
	QByteArrayList updatedFeatures;

	GenerateDifferences(*oldObjectPtr, *newObjectPtr, addedFeatures, removedFeatures, updatedFeatures);

	// Add new features to the package:
	for (const QByteArray& addFeatureId : addedFeatures){
		const imtlic::IFeatureInfo* featureInfoPtr = newObjectPtr->FindFeatureById(addFeatureId);
		if (featureInfoPtr != nullptr){
			QByteArray collectionId = newObjectPtr->GetFeatureCollectionId(addFeatureId);

			QString featureName = featureInfoPtr->GetFeatureName();
			QString featureDescription = newObjectPtr->GetFeatureList().GetElementInfo(
						collectionId,
						imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

			retVal += "\n" +
						QString("INSERT INTO Features(Id, Name, Description, PackageId) VALUES('%1', '%2', '%3', '%4');")
									.arg(qPrintable(addFeatureId))
									.arg(featureName)
									.arg(featureDescription)
									.arg(qPrintable(newPackageId)).toLocal8Bit();
		}
	}

	// Delete removed features to the package:
	for (const QByteArray& removedFeatureId : removedFeatures){
		retVal += "\n" +
					QString("DELETE FROM Features WHERE Id = '%1' AND PackageId = '%2';")
								.arg(qPrintable(removedFeatureId))
								.arg(qPrintable(newPackageId)).toLocal8Bit();
	}

	// Update changed features in the package:
	for (const QByteArray& updatedFeatureId : updatedFeatures){
		const imtlic::IFeatureInfo* featureInfoPtr = newObjectPtr->FindFeatureById(updatedFeatureId);
		if (featureInfoPtr != nullptr){
			QByteArray collectionId = newObjectPtr->GetFeatureCollectionId(updatedFeatureId);

			QString featureName = featureInfoPtr->GetFeatureName();
			QString featureDescription = newObjectPtr->GetFeatureList().GetElementInfo(
				collectionId,
				imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

			retVal += "\n" +
				QString("UPDATE Features SET Id = '%1', Name = '%2', Description = '%3', PackageId = '%4' WHERE PackageId = '%4';")
				.arg(qPrintable(updatedFeatureId))
				.arg(featureName)
				.arg(featureDescription)
				.arg(qPrintable(newPackageId)).toLocal8Bit();
		}
	}

	return retVal;
}


QByteArray CFeaturePackageDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName) const
{
	const imtlic::CFeaturePackage* currentPackagePtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		currentPackagePtr = dynamic_cast<const imtlic::CFeaturePackage*>(objectPtr.GetPtr());
	}

	if (currentPackagePtr == nullptr){
		return QByteArray();
	}

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray oldPackageId = currentPackagePtr->GetPackageId();
	QByteArray newPackageId = newObjectName.toLocal8Bit();

	QByteArray retVal = QString("UPDATE Packages SET Id ='%1', Name = '%1' WHERE Id ='%2';").arg(newObjectName).arg(qPrintable(oldPackageId)).toLocal8Bit();

	return retVal;
}


QByteArray CFeaturePackageDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& description) const
{
	const imtlic::IFeaturePackage* packagePtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		packagePtr = dynamic_cast<const imtlic::IFeaturePackage*>(objectPtr.GetPtr());
	}

	if (packagePtr == nullptr){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE Packages SET Description = '%1' WHERE Id ='%2';").arg(description).arg(qPrintable(packagePtr->GetPackageId())).toLocal8Bit();

	return retVal;
}


// protected methods

void CFeaturePackageDatabaseDelegateComp::GenerateDifferences(
			const imtlic::CFeaturePackage& currentPackage,
			const imtlic::CFeaturePackage& newPackage,
			QByteArrayList& addFeatures,
			QByteArrayList& removedFeatures,
			QByteArrayList& updatedFeatures) const
{
	imtbase::ICollectionInfo::Ids currentFeatureCollectionIds = currentPackage.GetFeatureList().GetElementIds();
	QByteArrayList currentFeatureIds;

	for (QByteArray id : currentFeatureCollectionIds){
		const imtlic::IFeatureInfo* featureInfoPtr = currentPackage.GetFeatureInfo(id);
		Q_ASSERT(featureInfoPtr != nullptr);
		Q_ASSERT(!featureInfoPtr->GetFeatureId().isEmpty());

		currentFeatureIds.push_back(featureInfoPtr->GetFeatureId());
	}

	imtbase::ICollectionInfo::Ids newFeatureCollectionIds = newPackage.GetFeatureList().GetElementIds();
	QByteArrayList newFeatureIds;

	for (QByteArray id : newFeatureCollectionIds){
		const imtlic::IFeatureInfo* featureInfoPtr = newPackage.GetFeatureInfo(id);
		Q_ASSERT(featureInfoPtr != nullptr);
		Q_ASSERT(!featureInfoPtr->GetFeatureId().isEmpty());

		newFeatureIds.push_back(featureInfoPtr->GetFeatureId());
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

	// Calculate changed features:
	for (QByteArray currentFeatureId : currentFeatureIds){
		// Feature-ID in both packages:
		if (newFeatureIds.contains(currentFeatureId)){
			const imtlic::IFeatureInfo* currentFeaturePtr = currentPackage.FindFeatureById(currentFeatureId);
			Q_ASSERT(currentFeaturePtr != nullptr);

			const imtlic::IFeatureInfo* newFeaturePtr = newPackage.FindFeatureById(currentFeatureId);
			Q_ASSERT(newFeaturePtr != nullptr);

			if (!newFeaturePtr->IsEqual(*currentFeaturePtr)){
				updatedFeatures.push_back(currentFeatureId);
			}
			else{
				QByteArray newCollectionId = newPackage.GetFeatureCollectionId(currentFeatureId);
				QString newFeatureDescription = newPackage.GetFeatureList().GetElementInfo(newCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

				QByteArray currentCollectionId = currentPackage.GetFeatureCollectionId(currentFeatureId);
				QString currentFeatureDescription = currentPackage.GetFeatureList().GetElementInfo(newCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

				if (newFeatureDescription != currentFeatureDescription){
					updatedFeatures.push_back(currentFeatureId);
				}
			}
		}
	}
}


} // namespace imtlicdb


