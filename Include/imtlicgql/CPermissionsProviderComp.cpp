#include <imtlicgql/CPermissionsProviderComp.h>


namespace imtlicgql
{


// protected methods

bool CPermissionsProviderComp::CreateRepresentationModelFromFeatureInfo(
			const imtlic::CFeatureInfo& featureInfo,
			imtbase::CTreeItemModel& representationModel,
			QString& errorMessage) const
{
	if (!featureInfo.IsPermission()){
		return false;
	}

	QByteArray featureId = featureInfo.GetFeatureId();

	representationModel.SetData("FeatureId", featureId);
	representationModel.SetData("FeatureName", featureInfo.GetFeatureName());
	representationModel.SetData("Optional", featureInfo.IsOptional());
	representationModel.SetData("FeatureDescription", featureInfo.GetFeatureDescription());
	representationModel.SetData("Dependencies", featureInfo.GetDependencies().join(';'));
	representationModel.SetData("ChildModel", 0);

	const imtlic::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.IsEmpty()){
		imtbase::CTreeItemModel* childModelPtr = representationModel.AddTreeModel("ChildModel");
		Q_ASSERT(childModelPtr != nullptr);

		for (int i = 0; i < subFeatures.GetCount(); i++){
			const imtlic::IFeatureInfo* featureInfoPtr = subFeatures.GetAt(i);
			if (featureInfoPtr == nullptr){
				errorMessage = QString("Unable to create representation model for invalid subfeature. Parent feature id: %1.").arg(qPrintable(featureId));
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr);
			Q_ASSERT(subFeatureInfoPtr != nullptr);

			imtbase::CTreeItemModel subFeatureRepresentationModel;
			bool ok = CreateRepresentationModelFromFeatureInfo(*subFeatureInfoPtr, subFeatureRepresentationModel, errorMessage);
			if (ok){
				childModelPtr->InsertNewItem();
				childModelPtr->CopyItemDataFromModel(i, &subFeatureRepresentationModel, 0);
			}
		}
	}

	return true;
}


// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CPermissionsProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	if (!m_productInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'FeatureContainer' was not set", "CPermissionsProviderComp");
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> representationModelPtr(new imtbase::CTreeItemModel());

	imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
	if (featureCollectionPtr != nullptr){
		for (imtbase::ICollectionInfo::Id& elementId : featureCollectionPtr->GetElementIds()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
				const imtlic::CFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(dataPtr.GetPtr());
				if (featureInfoPtr != nullptr){
					imtbase::CTreeItemModel featureRepresentationModel;
					QString errorMessage;
					bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, featureRepresentationModel, errorMessage);
					if (ok){
						int index = representationModelPtr->InsertNewItem();
						representationModelPtr->CopyItemDataFromModel(index, &featureRepresentationModel, 0);
						representationModelPtr->SetData("Id", elementId, index);
					}
				}
			}
		}
	}

	return representationModelPtr.PopPtr();
}


} // namespace imtlicgql


