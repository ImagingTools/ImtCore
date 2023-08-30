#include <imtlicdb/CLicenseDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/CLicenseInfo.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CLicenseDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtlic::CLicenseInfo> licenseInfoPtr;
	licenseInfoPtr.SetPtr(new imtlic::CLicenseInfo);
	if (!licenseInfoPtr.IsValid()){
		return nullptr;
	}

	QByteArray licenseId;
	if (record.contains("Id")){
		licenseId = record.value("Id").toByteArray();

		licenseInfoPtr->SetLicenseId(licenseId);
	}

	QString licenseName;
	if (record.contains("Name")){
		licenseName = record.value("Name").toString();

		licenseInfoPtr->SetLicenseName(licenseName);
	}

	imtlic::ILicenseInfo::FeatureInfos featuresInfo;

	QByteArray selectLicenseFeatures = QString("SELECT * FROM \"ProductLicenseFeatures\" WHERE \"LicenseId\" = '%1';").arg(qPrintable(licenseId)).toUtf8();

	QSqlError error;
	QSqlQuery productLicensesFeaturesQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectLicenseFeatures, &error);

	while (productLicensesFeaturesQuery.next()){
		QSqlRecord licenseFeatureRecord = productLicensesFeaturesQuery.record();

		QByteArray featureId;
		QString featureName;

		if (licenseFeatureRecord.contains("FeatureId")){
			featureId = licenseFeatureRecord.value("FeatureId").toByteArray();
		}

		imtlic::ILicenseInfo::FeatureInfo featureInfo;

		QByteArray selectFeatures = QString("SELECT * FROM \"Features\" WHERE \"Id\" = '%1';").arg(qPrintable(featureId)).toUtf8();
		QSqlQuery selectFeaturesQuery = m_databaseEngineCompPtr->ExecSqlQuery(selectFeatures, &error);

		selectFeaturesQuery.next();

		QSqlRecord featureRecord = selectFeaturesQuery.record();
		if (featureRecord.contains("Name")){
			featureName = featureRecord.value("Name").toString();
		}

		featureInfo.id = featureId;
		featureInfo.name = featureName;

		featuresInfo << featureInfo;
	}

	licenseInfoPtr->SetFeatureInfos(featuresInfo);

	return licenseInfoPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CLicenseDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* /*valuePtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return imtdb::IDatabaseObjectDelegate::NewObjectQuery();
}


QByteArray CLicenseDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CLicenseDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	return QByteArray();
}


QByteArray CLicenseDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CLicenseDatabaseDelegateComp::CLicenseDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


} // namespace imtlicdb


