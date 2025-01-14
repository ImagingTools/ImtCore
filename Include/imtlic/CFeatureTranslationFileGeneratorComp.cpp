#include "imtlic/CFeatureTranslationFileGeneratorComp.h"


// ImtCore includes
#include <imtbase/CTranslationFile.h>
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


// reimplemented (icomp::CComponentBase)

void CFeatureTranslationFileGeneratorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featureContainerCompPtr.IsValid()){
		imtbase::IObjectCollection* featureCollectionPtr = m_featureContainerCompPtr->GetFeatures();
		if (featureCollectionPtr != nullptr){
			imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
			for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
					const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
					if (featureInfoPtr != nullptr){
						for (imtbase::ICollectionInfo::Id& subFeatureId : featureInfoPtr->GetSubFeatureIds()){
							const imtlic::IFeatureInfo* subFeatureInfoPtr = featureInfoPtr->GetSubFeature(subFeatureId);
							if (subFeatureInfoPtr != nullptr){
								QString featureName = subFeatureInfoPtr->GetFeatureName();
							}
						}
					}
				}
			}
		}
	}
}


void CFeatureTranslationFileGeneratorComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtlic


