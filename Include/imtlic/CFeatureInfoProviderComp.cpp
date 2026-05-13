// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "imtlic/CFeatureInfoProviderComp.h"


// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


imtlic::IFeatureInfoSharedPtr CFeatureInfoProviderComp::GetFeatureInfo(const QByteArray& featureId) const
{
	if (!m_featureCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_featureCollectionCompPtr->GetObjectData(featureId, dataPtr)){
		IFeatureInfoSharedPtr retVal;

		retVal.SetCastedPtr(dataPtr);

		return retVal;
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CFeatureInfoProviderComp::GetFeatureList() const
{
	return *m_featureCollectionCompPtr.GetPtr();
}


} // namespace imtlic


