// ImtCore includes
#include <imtlicgql/CFeaturesExportProviderComp.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/CLicenseInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtbase/ICollectionInfo.h>
#include <imtlic/CFeaturePackage.h>
#include <ifile/CCompactXmlFileWriteArchive.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>


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
        for ( const QByteArray& licenseId : licenseCollectionIds){
            const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
            if (licenseInfoPtr == nullptr){
                continue;
            }

            imtlic::ILicenseInfo::FeatureInfos featureInfos = licenseInfoPtr->GetFeatureInfos();
            for (int i = 0; i < featureInfos.size(); i++){
                QByteArray featureId = featureInfos[i].id;
                const imtlic::IFeatureInfo* featurePtr = featurePackagePtr->GetFeatureInfo(featureId);
                if (featurePtr == nullptr){
                    QString featureName = featureInfos[i].name;
                    istd::TDelPtr<imtlic::CFeatureInfo> featureInfoPtr = new imtlic::CFeatureInfo;
                    featureInfoPtr->SetFeatureId(featureId);
                    featureInfoPtr->SetFeatureName(featureName);
                    featurePackagePtr->InsertNewObject("FeatureInfo", featureName, "", featureInfoPtr.GetPtr());
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





