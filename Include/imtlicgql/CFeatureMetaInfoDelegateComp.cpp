#include <imtlicgql/CFeatureMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgql
{


// protected methods

bool CFeatureMetaInfoDelegateComp::FillRepresentation(sdl::imtlic::Features::CFeatureData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray featureId = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_ID).toByteArray();
	metaInfoRepresentation.featureId = featureId;
	
	QString featureName = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_NAME).toString();
	metaInfoRepresentation.featureName = featureName;
	
	QString description = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_DESCRIPTION).toString();
	metaInfoRepresentation.description = description;
	
	bool isOptional = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_IS_OPTIONAL).toBool();
	metaInfoRepresentation.optional = isOptional;
	
	bool isPermission = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_IS_PERMISSION).toBool();
	metaInfoRepresentation.isPermission = isPermission;
	
	return true;
}


bool CFeatureMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::imtlic::Features::CFeatureData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.featureId){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_ID, *metaInfoRepresentation.featureId);
	}
	
	if (metaInfoRepresentation.featureName){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_NAME, *metaInfoRepresentation.featureName);
	}
	
	if (metaInfoRepresentation.description){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_DESCRIPTION, *metaInfoRepresentation.description);
	}
	
	if (metaInfoRepresentation.optional){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_IS_OPTIONAL, *metaInfoRepresentation.optional);
	}
	
	if (metaInfoRepresentation.isPermission){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_IS_PERMISSION, *metaInfoRepresentation.isPermission);
	}
	
	return true;
}


} // namespace imtlicgql


