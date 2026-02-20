// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CFeatureContainer.h>


namespace imtlic
{


// public methods

CFeatureContainer::CFeatureContainer()
	:BaseClass("FeatureInfo", "Feature info", "Features")
{
}


IFeatureInfoSharedPtr CFeatureContainer::FindFeatureById(const QByteArray& featureId) const
{
	imtbase::ICollectionInfo::Ids featureIds = GetFeatureList().GetElementIds();
	for (const QByteArray& id : featureIds){
		IFeatureInfoSharedPtr featurePtr = GetFeatureInfo(id);
		Q_ASSERT(featurePtr.IsValid());
		if (featurePtr.IsValid()){
			if (featurePtr->GetFeatureId() == featureId){
				return featurePtr;
			}

			QByteArrayList subfeaturesIds = featurePtr->GetSubFeatureIds();
			if (subfeaturesIds.contains(featureId)){
				return featurePtr->GetSubFeature(featureId);
			}
		}
	}

	return nullptr;
}


QByteArray CFeatureContainer::GetFeatureCollectionId(const QByteArray& featureId) const
{
	imtbase::ICollectionInfo::Ids featureIds = GetFeatureList().GetElementIds();
	for (QByteArray id : featureIds){
		IFeatureInfoSharedPtr featurePtr = GetFeatureInfo(id);
		Q_ASSERT(featurePtr.IsValid());
		if (featurePtr.IsValid()){
			if (featurePtr->GetFeatureId() == featureId){
				return id;
			}
		}
	}

	return QByteArray();
}


// reimplemented (IFeatureInfoProvider)

const imtbase::ICollectionInfo& CFeatureContainer::GetFeatureList() const
{
	return m_collection;
}


IFeatureInfoSharedPtr CFeatureContainer::GetFeatureInfo(const QByteArray& featureId) const
{
	DataPtr dataPtr;
	if (m_collection.GetObjectData(featureId, dataPtr)) {
		IFeatureInfoSharedPtr retVal;

		retVal.SetCastedPtr(dataPtr);

		return retVal;
	}

	return IFeatureInfoSharedPtr();
}


} // namespace imtlic


