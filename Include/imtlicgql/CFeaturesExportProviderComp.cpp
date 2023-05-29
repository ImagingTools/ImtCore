#include <imtlicgql/CFeaturesExportProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/CLicenseInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtbase/ICollectionInfo.h>
#include <imtlic/CFeaturePackage.h>
#include <ifile/CCompactXmlFileWriteArchive.h>


namespace imtlicgql
{


// public methods

// reimplemented (imtbase::IBinaryDataProvider)

bool CFeaturesExportProviderComp::GetData(QByteArray& data, const QByteArray& dataId) const
{
	if (!m_productCollectionCompPtr.IsValid() || !m_packageCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr productDataPtr;
	if (m_productCollectionCompPtr->GetObjectData(dataId, productDataPtr)){
		const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(productDataPtr.GetPtr());
		const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
		const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();

		istd::TDelPtr<imtlic::CFeaturePackage> resultPackagePtr = new imtlic::CFeaturePackage;
		for (const QByteArray& licenseId : licenseCollectionIds){
			const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
			if (licenseInfoPtr != nullptr){
				imtlic::ILicenseInfo::FeatureInfos featureInfos = licenseInfoPtr->GetFeatureInfos();
				for (int i = 0; i < featureInfos.size(); i++){
					QByteArray featureId = featureInfos[i].id;

					const imtbase::IObjectCollectionInfo::Ids packageIds = m_packageCollectionCompPtr->GetElementIds();
					for (imtbase::IObjectCollectionInfo::Id packageId : packageIds){
						imtbase::IObjectCollection::DataPtr packageDataPtr;
						if (m_packageCollectionCompPtr->GetObjectData(packageId, packageDataPtr)){
							const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(packageDataPtr.GetPtr());
							if (packagePtr != nullptr){
								const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->FindFeatureById(featureId);
								const imtlic::IFeatureInfo* findedFeatureInfoPtr = resultPackagePtr->FindFeatureById(featureId);
								if (featureInfoPtr != nullptr && findedFeatureInfoPtr == nullptr){
									QString featureName = featureInfoPtr->GetFeatureName();

									resultPackagePtr->InsertNewObject("FeatureInfo", featureName, "", featureInfoPtr);

									QByteArrayList featureDependencies = packagePtr->GetFeatureDependencies(featureId);
									if (!featureDependencies.empty()){
										resultPackagePtr->SetFeatureDependencies(featureId, featureDependencies);
									}

									QByteArrayList allSubFeaturesList = featureInfoPtr->GetSubFeatureIds();
									for (const QByteArray& subFeatureId : allSubFeaturesList){
										QByteArrayList subfeatureDependencies = packagePtr->GetFeatureDependencies(subFeatureId);

										if (!subfeatureDependencies.empty()){
											resultPackagePtr->SetFeatureDependencies(subFeatureId, subfeatureDependencies);
										}
									}
								}
							}
						}
					}
				}
			}
		}

		QString filePathTmp = QDir::tempPath() + "/"  + QUuid::createUuid().toString() + ".xml";
		{
			ifile::CCompactXmlFileWriteArchive writeArchive(filePathTmp);

			resultPackagePtr->Serialize(writeArchive);
		}

		QFile file(filePathTmp);
		if (file.open(QIODevice::ReadOnly)){
			data = file.readAll();
			file.close();
			QFile::remove(filePathTmp);

			return true;
		}
	}

	return false;
}


} // namespace imtqml





