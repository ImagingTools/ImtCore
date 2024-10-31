#include "imtlic/CFeatureInfoProviderComp.h"


// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


const IFeatureInfo* CFeatureInfoProviderComp::GetFeatureInfo(const QByteArray& featureId) const
{
	if (!m_featureCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_featureCollectionCompPtr->GetObjectData(featureId, dataPtr)){
		return dynamic_cast<const IFeatureInfo*>(dataPtr.GetPtr());
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CFeatureInfoProviderComp::GetFeatureList() const
{
	return *m_featureCollectionCompPtr.GetPtr();
}


} // namespace imtlic


