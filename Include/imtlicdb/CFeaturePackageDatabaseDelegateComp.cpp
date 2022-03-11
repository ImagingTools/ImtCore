#include <imtlicdb/CFeaturePackageDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CFeaturePackageDatabaseDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record, const QSqlQuery& querysql) const
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
	}

	if (record.contains("Name")){
		packageName = record.value("Name").toString();
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

		QByteArrayList featureDependencies;
		QByteArray dependenciesQuery = QString("SELECT dependencyid, dependencypackageid FROM featuredependencies"
											   " WHERE featureid = '%1' AND featurepackageid = '%2'")
												.arg(qPrintable(featureId))
												.arg(qPrintable(packageId)).toUtf8();
		QSqlQuery dependenciesSqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(dependenciesQuery, &error);

		while (dependenciesSqlQuery.next()){
			QSqlRecord depFeatureRecord = dependenciesSqlQuery.record();
			QByteArray dependFeatureId, dependPackageId;

			if (depFeatureRecord.contains("dependencyid")){
				dependFeatureId = depFeatureRecord.value("dependencyid").toByteArray();
			}

			if (depFeatureRecord.contains("dependencypackageid")){
				dependPackageId = depFeatureRecord.value("dependencypackageid").toByteArray();
			}

			featureDependencies.append(dependPackageId + "." + dependFeatureId);
		}

		if (!featureDependencies.isEmpty()){
			featurePackagePtr->SetFeatureDependencies(packageId + "." + featureId, featureDependencies);
		}
	}

	if (!packageId.isEmpty() && !packageName.isEmpty()){
		return featurePackagePtr.PopPtr();
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CFeaturePackageDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	QByteArray packageId;
	const imtlic::CFeaturePackage* featurePackagePtr = dynamic_cast<const imtlic::CFeaturePackage*>(valuePtr);
	if (featurePackagePtr != nullptr){
		packageId = featurePackagePtr->GetPackageId();
		if (packageId.isEmpty()){
			packageId = objectName.toLocal8Bit();
		}

		NewObjectQuery retVal;

		retVal.query = QString("INSERT INTO Packages(Id, Name, Description, Added, LastModified) VALUES('%1', '%2', '%3', '%4', '%5');")
					.arg(qPrintable(packageId))
					.arg(objectName)
					.arg(objectDescription)
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.toLocal8Bit();

		retVal.objectName = objectName;

		imtbase::ICollectionInfo::Ids featureIds = featurePackagePtr->GetElementIds();
		for (const QByteArray& collectionId : featureIds){
			const imtlic::IFeatureInfo* featureInfoPtr = featurePackagePtr->GetFeatureInfo(collectionId);
			if (featureInfoPtr != nullptr){
				QByteArray featureId = featureInfoPtr->GetFeatureId();
				QString featureName = featureInfoPtr->GetFeatureName();
				QString featureDescription = featurePackagePtr->GetFeatureList().GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				retVal.query += "\n" + 
							QString("INSERT INTO Features(Id, Name, Description, PackageId) VALUES('%1', '%2', '%3', '%4');")
										.arg(qPrintable(featureId))
										.arg(featureName)
										.arg(featureDescription)
										.arg(qPrintable(packageId)).toLocal8Bit();
			}
		}

		return retVal;
	}

	return NewObjectQuery();
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

	QByteArray retVal = QString("UPDATE Packages SET Id ='%1', LastModified = '%2' WHERE Id ='%3';")
				.arg(qPrintable(newPackageId))
				.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
				.arg(qPrintable(oldPackageId))
				.toLocal8Bit();

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
					QString("DELETE FROM FeatureDependencies WHERE featureid = '%1' AND featurepackageid = '%2' OR dependencyid = '%3' AND dependencypackageid = '%4';")
									.arg(qPrintable(removedFeatureId))
									.arg(qPrintable(newPackageId))
									.arg(qPrintable(removedFeatureId))
									.arg(qPrintable(newPackageId))
									.toLocal8Bit();
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
				QString("UPDATE Features SET Id = '%1', Name = '%2', Description = '%3', PackageId = '%4' WHERE PackageId = '%5' AND Id = '%6';")
				.arg(qPrintable(updatedFeatureId))
				.arg(featureName)
				.arg(featureDescription)
				.arg(qPrintable(newPackageId))
				.arg(qPrintable(newPackageId))
				.arg(qPrintable(updatedFeatureId)).toLocal8Bit();
		}
	}

	// Add new dependencies for the features:
	imtbase::ICollectionInfo::Ids packageIds = collection.GetElementIds();
	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		collection.GetObjectData(packageId, dataPtr);

		const imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(dataPtr.GetPtr());
		if (packagePtr != nullptr){
			const imtlic::IFeatureDependenciesProvider* dependenciesProvider = packagePtr->GetDependenciesInfoProvider();
			if (dependenciesProvider != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featureInfoPtr != nullptr){
						QByteArray featureId = featureInfoPtr->GetFeatureId();
						QByteArrayList dependsIds = newObjectPtr->GetFeatureDependencies(packageId + "." + featureId);


						retVal += "\n" +
									QString("DELETE FROM FeatureDependencies WHERE featureid = '%1' AND featurepackageid = '%2';")
													.arg(qPrintable(featureId))
													.arg(qPrintable(packageId))
													.toLocal8Bit();


						for (const QByteArray& dependFeatureId : dependsIds){
							QByteArrayList data = dependFeatureId.split('.');
							retVal += "\n" +
										QString("INSERT INTO FeatureDependencies(featureid, featurepackageid, dependencyid, dependencypackageid) VALUES('%1', '%2', '%3', '%4');")
													.arg(qPrintable(featureId))
													.arg(qPrintable(packageId))
													.arg(qPrintable(data[1]))
													.arg(qPrintable(data[0]))
													.toLocal8Bit();
						}
					}
				}
			}
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


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::IDocumentMetaInfo* CFeaturePackageDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return nullptr;
}


bool CFeaturePackageDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& /*record*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return true;
}


} // namespace imtlicdb


