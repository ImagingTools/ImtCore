#include <imtlicgql/CLicenseCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/ILicenseInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CLicenseCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	const imtgql::CGqlObject* licensesParamPtr = gqlRequest.GetParam("licenses");
	if (licensesParamPtr != nullptr){
		QByteArrayList fieldIds = licensesParamPtr->GetFieldIds();

		for (const QByteArray& licenseId : fieldIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(licenseId, dataPtr)){
				imtlic::ILicenseInfo* licenseInfoPtr = dynamic_cast<imtlic::ILicenseInfo*>(dataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					dataModelPtr->SetData("Id", licenseInfoPtr->GetLicenseId());
					dataModelPtr->SetData("Name", licenseInfoPtr->GetLicenseName());

					imtbase::CTreeItemModel* featuresInfoModelPtr = dataModelPtr->AddTreeModel("FeaturesInfo");

					imtlic::ILicenseInfo::FeatureInfos featuresInfos = licenseInfoPtr->GetFeatureInfos();
					for (const imtlic::ILicenseInfo::FeatureInfo& featureInfo : featuresInfos){
						int index = featuresInfoModelPtr->InsertNewItem();

						featuresInfoModelPtr->SetData("Id", featureInfo.id, index);
						featuresInfoModelPtr->SetData("Name", featureInfo.name, index);
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtlicgql


