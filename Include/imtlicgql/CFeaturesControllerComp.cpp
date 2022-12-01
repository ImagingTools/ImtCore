#include <imtlicgql/CFeaturesControllerComp.h>


// ACF includes
#include <istd/TChangeDelegator.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>


namespace imtlicgql
{


imtbase::CTreeItemModel *CFeaturesControllerComp::ListObjects(const QList<imtgql::CGqlObject> &inputParams, const imtgql::CGqlObject &gqlObject, QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();

	for (const QByteArray& collectionId : collectionIds){
		int itemIndex = rootModel->InsertNewItem();
		if (!SetupGqlItem(gqlObject, *rootModel, itemIndex, collectionId, errorMessage)){
			return nullptr;
		}

		imtbase::CTreeItemModel* featuresModel = rootModel->AddTreeModel("ChildModel", itemIndex);
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
			if (packagePtr != nullptr){
				QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featureInfoPtr != nullptr){
						const imtlic::IFeatureInfo* parentFeatureInfoPtr = featureInfoPtr->GetParentFeature();
						if (parentFeatureInfoPtr == nullptr){

						}
					}
				}
			}
		}
	}

	return rootModel;
}


} // namespace imtlicgql
