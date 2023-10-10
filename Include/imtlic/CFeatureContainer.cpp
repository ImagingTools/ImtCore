#include <imtlic/CFeatureContainer.h>


namespace imtlic
{


// public methods

CFeatureContainer::CFeatureContainer()
	:BaseClass("FeatureInfo", "Feature info", "Features")
{
}


const IFeatureInfo* CFeatureContainer::FindFeatureById(const QByteArray& featureId) const
{
	imtbase::ICollectionInfo::Ids featureIds = GetFeatureList().GetElementIds();
	for (QByteArray id : featureIds){
		const IFeatureInfo* featurePtr = GetFeatureInfo(id);
		Q_ASSERT(featurePtr != nullptr);
		if (featurePtr != nullptr){
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
		const IFeatureInfo* featurePtr = GetFeatureInfo(id);
		Q_ASSERT(featurePtr != nullptr);
		if (featurePtr != nullptr){
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


const IFeatureInfo* CFeatureContainer::GetFeatureInfo(const QByteArray& featureId) const
{
	return dynamic_cast<const IFeatureInfo*>(m_collection.GetObjectPtr(featureId));
}


} // namespace imtlic


