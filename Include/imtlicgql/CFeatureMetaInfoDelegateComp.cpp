#include <imtlicgql/CFeatureMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgql
{


// protected methods

bool CFeatureMetaInfoDelegateComp::FillRepresentation(sdl::imtlic::Features::CFeatureData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray featureId = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_ID).toByteArray();
	metaInfoRepresentation.FeatureId = featureId;
	
	QString featureName = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_NAME).toString();
	metaInfoRepresentation.FeatureName = featureName;
	
	QString description = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_DESCRIPTION).toString();
	metaInfoRepresentation.Description = description;
	
	bool isOptional = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_IS_OPTIONAL).toBool();
	metaInfoRepresentation.Optional = isOptional;
	
	bool isPermission = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_IS_PERMISSION).toBool();
	metaInfoRepresentation.IsPermission = isPermission;
	
	return true;
}


bool CFeatureMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::imtlic::Features::CFeatureData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.FeatureId){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_ID, *metaInfoRepresentation.FeatureId);
	}
	
	if (metaInfoRepresentation.FeatureName){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_NAME, *metaInfoRepresentation.FeatureName);
	}
	
	if (metaInfoRepresentation.Description){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_DESCRIPTION, *metaInfoRepresentation.Description);
	}
	
	if (metaInfoRepresentation.Optional){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_IS_OPTIONAL, *metaInfoRepresentation.Optional);
	}
	
	if (metaInfoRepresentation.IsPermission){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_IS_PERMISSION, *metaInfoRepresentation.IsPermission);
	}
	
	return true;
}


} // namespace imtlicgql


