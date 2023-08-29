#include <imtlicgql/CLicenseControllerComp.h>


// ImtCore includes
#include <imtlic/ILicenseInfo.h>


namespace imtlicgql
{


imtbase::CTreeItemModel* CLicenseControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray licenseId = GetObjectIdFromInputParams(gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr licenseDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(licenseId, licenseDataPtr)){
		imtlic::ILicenseInfo* licenseInfoPtr = dynamic_cast<imtlic::ILicenseInfo*>(licenseDataPtr.GetPtr());
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

			return rootModelPtr.PopPtr();
		}
	}

	return nullptr;
}


} // namespace imtlicgql


