#include "imtlic/CFeatureInfoProviderComp.h"


// ImtCore includes
#include <imtbase/CCollectionInfo.h>


namespace imtlic
{


const IFeatureInfo* CFeatureInfoProviderComp::GetFeatureInfo(const QByteArray& /*featureId*/) const
{
	if (!m_featureCollectionCompPtr.IsValid()){
		return nullptr;
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CFeatureInfoProviderComp::GetFeatureList() const
{
	return imtbase::CCollectionInfo();
}


} // namespace imtlic


