#include <imtlicdb/CFeaturePackageDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CFeaturePackageDatabaseDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
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

	QByteArray query = QString("SELECT * from \"Features\" WHERE PackageId = '%1'").arg(qPrintable(packageId)).toUtf8();

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

		QByteArrayList featureDependencies;
		QByteArray dependenciesQuery = QString("SELECT dependencyid FROM \"FeatureDependencies\" WHERE featureid = '%1'").arg(qPrintable(featureId)).toUtf8();
		QSqlQuery dependenciesSqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(dependenciesQuery, &error);

		while (dependenciesSqlQuery.next()){
			QSqlRecord depFeatureRecord = dependenciesSqlQuery.record();
			QByteArray dependFeatureId, dependPackageId;

			if (depFeatureRecord.contains("DependencyId")){
				dependFeatureId = depFeatureRecord.value("DependencyId").toByteArray();
			}

			featureDependencies.append(dependFeatureId);
		}

		QByteArray subFeaturesQuery = QString("SELECT * FROM \"Features\" WHERE ParentId = '%1'").arg(qPrintable(featureId)).toUtf8();

		CreateSubFeaturesFromRecord(featureInfoPtr.GetPtr(), subFeaturesQuery);

		featurePackagePtr->InsertNewObject("FeatureInfo", featureName, description, featureInfoPtr.GetPtr());

		if (!featureDependencies.isEmpty()){
			featurePackagePtr->SetFeatureDependencies(featureId, featureDependencies);
		}
	}

	if (!packageId.isEmpty() && !packageName.isEmpty()){
		return featurePackagePtr.PopPtr();
	}

	return nullptr;
}


void CFeaturePackageDatabaseDelegateComp::CreateSubFeaturesFromRecord(
		imtlic::IFeatureInfo* featureInfoPtr,
		const QByteArray& query) const
{
	QSqlError error;
	QSqlQuery subFeaturesSqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &error);
	while (subFeaturesSqlQuery.next()){
		QSqlRecord subFeaturesRecord = subFeaturesSqlQuery.record();

		QByteArray subFeatureId;
		if (subFeaturesRecord.contains("Id")){
			subFeatureId = subFeaturesRecord.value("Id").toByteArray();
		}

		QString subFeatureName;
		if (subFeaturesRecord.contains("Name")){
			subFeatureName = subFeaturesRecord.value("Name").toString();
		}

		imtlic::CFeatureInfo* subFeatureInfoPtr = new imtlic::CFeatureInfo;
		subFeatureInfoPtr->SetFeatureId(subFeatureId);
		subFeatureInfoPtr->SetFeatureName(subFeatureName);

		featureInfoPtr->InsertSubFeature(subFeatureInfoPtr);

		QByteArray parentFeatureId;
		if (subFeaturesRecord.contains("ParentId")){
			parentFeatureId = subFeaturesRecord.value("ParentId").toByteArray();
		}

		QByteArray subFeaturesQuery = QString("SELECT * FROM \"Features\" WHERE ParentId = '%1'").arg(qPrintable(subFeatureId)).toUtf8();

		CreateSubFeaturesFromRecord(subFeatureInfoPtr, subFeaturesQuery);
	}
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

		retVal.query = QString("INSERT INTO \"Packages\"(Id, Name, Description, Added, LastModified) VALUES('%1', '%2', '%3', '%4', '%5');")
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
							QString("INSERT INTO \"Features\"(Id, Name, Description, PackageId) VALUES('%1', '%2', '%3', '%4');")
										.arg(qPrintable(featureId))
										.arg(featureName)
										.arg(featureDescription)
										.arg(qPrintable(packageId)).toLocal8Bit();

				QByteArrayList dependsIds = featurePackagePtr->GetFeatureDependencies(featureId);
				if (dependsIds.size() > 0){
					for (const QByteArray& dependFeatureId : dependsIds){
						retVal.query += "\n" +
									QString("INSERT INTO \"FeatureDependencies\"(featureid, dependencyid) VALUES('%1', '%2');")
												.arg(qPrintable(featureId))
												.arg(qPrintable(dependFeatureId))
												.toLocal8Bit();
					}
				}
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
				QByteArray retVal = QString("DELETE FROM \"Features\" WHERE ParentId = '%1';").arg(qPrintable(packageId)).toLocal8Bit();
				retVal += "\n" + QString("DELETE FROM \"Packages\" WHERE Id = '%1';").arg(qPrintable(packageId)).toLocal8Bit();

				return retVal;
			}
		}
	}

	return QByteArray();
}


void CFeaturePackageDatabaseDelegateComp::CreateInsertSubFeaturesQuery(const imtlic::IFeatureInfo *featureInfoPtr, QByteArray &retVal) const
{
	QList<const imtlic::IFeatureInfo*> subFeatures = featureInfoPtr->GetSubFeatures();

	QByteArray parentFeatureId = featureInfoPtr->GetFeatureId();

	for (const imtlic::IFeatureInfo* featureInfo : subFeatures){
		QByteArray featureId = featureInfo->GetFeatureId();
		QString featureName = featureInfo->GetFeatureName();

		retVal += "\n" +
					QString("INSERT INTO \"Features\"(Id, Name, Description, ParentId) VALUES('%1', '%2', '%3', '%4');")
								.arg(qPrintable(featureId))
								.arg(featureName)
								.arg("")
								.arg(qPrintable(parentFeatureId)).toLocal8Bit();

		CreateInsertSubFeaturesQuery(featureInfo, retVal);
	}
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

	QByteArray retVal = QString("UPDATE \"Packages\" SET Id ='%1', LastModified = '%2' WHERE Id ='%3';")
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
						QString("INSERT INTO \"Features\" (Id, Name, Description, PackageId) VALUES('%1', '%2', '%3', '%4');")
									.arg(qPrintable(addFeatureId))
									.arg(featureName)
									.arg(featureDescription)
									.arg(qPrintable(newPackageId)).toLocal8Bit();

			CreateInsertSubFeaturesQuery(featureInfoPtr, retVal);
		}
	}

	// Delete removed features to the package:
	for (const QByteArray& removedFeatureId : removedFeatures){
		retVal += "\n" +
					QString("DELETE FROM \"FeatureDependencies\" WHERE featureid = '%1' OR dependencyid = '%2';")
									.arg(qPrintable(removedFeatureId))
									.arg(qPrintable(removedFeatureId))
									.toLocal8Bit();
		retVal += "\n" +
					QString("DELETE FROM \"Features\" WHERE Id = '%1' AND PackageId = '%2';")
								.arg(qPrintable(removedFeatureId))
								.arg(qPrintable(newPackageId)).toLocal8Bit();
	}

	// Update changed features in the package:
	for (const QByteArray& updatedFeatureId : updatedFeatures){
		const imtlic::IFeatureInfo* featureInfoPtr = newObjectPtr->FindFeatureById(updatedFeatureId);
		if (featureInfoPtr != nullptr){
			QByteArray collectionId = newObjectPtr->GetFeatureCollectionId(updatedFeatureId);

			QString featureName = featureInfoPtr->GetFeatureName();
			QString featureDescription = newObjectPtr->GetFeatureList().GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

			retVal += QString("\nUPDATE \"Features\" SET Id = '%1', Name = '%2', Description = '%3', PackageId = '%4' WHERE PackageId = '%5' AND Id = '%6';")
				.arg(qPrintable(updatedFeatureId))
				.arg(featureName)
				.arg(featureDescription)
				.arg(qPrintable(newPackageId))
				.arg(qPrintable(newPackageId))
				.arg(qPrintable(updatedFeatureId)).toLocal8Bit();

			retVal += QString("\nDELETE FROM \"Features\" WHERE parentid = '%1';").arg(qPrintable(updatedFeatureId)).toLocal8Bit();

			CreateInsertSubFeaturesQuery(featureInfoPtr, retVal);
		}
	}

	// Add new dependencies for the features:
	imtbase::ICollectionInfo::Ids featureCollectionIds = newObjectPtr->GetFeatureList().GetElementIds();
	for (const QByteArray& featureCollectionId : featureCollectionIds){
		const imtlic::IFeatureInfo* featureInfoPtr = newObjectPtr->GetFeatureInfo(featureCollectionId);
		if (featureInfoPtr != nullptr){
			QByteArray featureId = featureInfoPtr->GetFeatureId();
			QByteArrayList dependsIds = newObjectPtr->GetFeatureDependencies(featureId);

			retVal += "\n" +
					QString("DELETE FROM \"FeatureDependencies\" WHERE featureid = '%1';")
					.arg(qPrintable(featureId))
					.toLocal8Bit();

			for (const QByteArray& dependFeatureId : dependsIds){
				retVal += "\n" +
						QString("INSERT INTO \"FeatureDependencies\" (featureid, dependencyid) VALUES('%1', '%2');")
						.arg(qPrintable(featureId))
						.arg(qPrintable(dependFeatureId))
						.toLocal8Bit();
			}
		}
	}

	qDebug() << "retVal " << retVal;

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

	QByteArray packageId = currentPackagePtr->GetPackageId();
//	QByteArray newPackageId = newObjectName.toLocal8Bit();

	QByteArray retVal = QString("UPDATE \"Packages\" SET Name = '%1', LastModified = '%2' WHERE Id ='%3';")
			.arg(newObjectName)
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.arg(qPrintable(packageId)).toLocal8Bit();

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

	QByteArray retVal = QString("UPDATE \"Packages\" SET Description = '%1', LastModified = '%2' WHERE Id ='%3';")
			.arg(description)
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
			.arg(qPrintable(packagePtr->GetPackageId())).toLocal8Bit();

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

idoc::MetaInfoPtr CFeaturePackageDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr();
}


bool CFeaturePackageDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& /*record*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return true;
}


} // namespace imtlicdb


