#include <imtlicdb/CFeaturePackageDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CFeaturePackage.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::IDatabaseObjectDelegate)

istd::IChangeable* CFeaturePackageDatabaseDelegateComp::CreateObjectFromRecord(
			const QByteArray& /*typeId*/,
			const QSqlRecord& record,
			QString& objectName,
			QString& /*objectDescription*/) const
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

		objectName = packageName;
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
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
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
										.arg(qPrintable(packageId));
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


} // namespace imtlicdb


