#include <imtauth/CPermissionsProviderComp.h>

//ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/IFeatureInfo.h>


namespace imtauth
{


QByteArray CPermissionsProviderComp::GetModelId() const
{
	return *m_modelIdAttrPtr;
}


imtbase::CTreeItemModel *CPermissionsProviderComp::GetTreeItemModel(const QList<imtgql::CGqlObject> &params, const QByteArrayList &fields, const imtgql::IGqlContext *gqlContext)
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (m_featurePackageCompPtr.IsValid()){
		QByteArrayList featureCollectionIds = m_featurePackageCompPtr->GetFeatureList().GetElementIds().toList();

		for (const QByteArray& featureCollectionId : featureCollectionIds){
			const imtlic::IFeatureInfo* featureInfoPtr = m_featurePackageCompPtr->GetFeatureInfo(featureCollectionId);
			if (featureInfoPtr != nullptr){
				int index = rootModel->InsertNewItem();
				QByteArray featureId = featureInfoPtr->GetFeatureId();
				QString featureName = featureInfoPtr->GetFeatureName();

				rootModel->SetData("Id", featureId, index);
				rootModel->SetData("Name", featureName, index);
			}
		}
	}

	return rootModel;
}


} // namespace imtauth
