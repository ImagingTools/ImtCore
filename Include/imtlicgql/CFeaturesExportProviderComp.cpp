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
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_productCollectionCompPtr->GetObjectData(dataId, dataPtr)){
		const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
		const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
		const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();

		istd::TDelPtr<imtlic::CFeaturePackage> featurePackagePtr = new imtlic::CFeaturePackage;
		for (const QByteArray& licenseId : licenseCollectionIds){
			const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
			if (licenseInfoPtr != nullptr){
				imtlic::ILicenseInfo::FeatureInfos featureInfos = licenseInfoPtr->GetFeatureInfos();
				for (int i = 0; i < featureInfos.size(); i++){
					QByteArray featureId = featureInfos[i].id;

					if (m_featureInfoProviderCompPtr.IsValid()){
						const imtlic::IFeatureInfo* featureInfoPtr = m_featureInfoProviderCompPtr->GetFeatureInfo(featureId);
						if (featureInfoPtr != nullptr){
							QString featureName = featureInfoPtr->GetFeatureName();
							featurePackagePtr->InsertNewObject("FeatureInfo", featureName, "", featureInfoPtr);
						}
					}
				}
			}
		}

		QString filePathTmp = QDir::tempPath() + "/"  + QUuid::createUuid().toString() + ".xml";
		{
			ifile::CCompactXmlFileWriteArchive writeArchive(filePathTmp);

			featurePackagePtr->Serialize(writeArchive);
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





