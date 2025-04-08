#include <imtlicgql/CLicenseMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtlic/ILicenseDefinition.h>


namespace imtlicgql
{


// protected methods

bool CLicenseMetaInfoDelegateComp::FillRepresentation(sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray licenseId = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_ID).toByteArray();
	metaInfoRepresentation.licenseId = licenseId;
	
	QString licenseName = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_NAME).toString();
	metaInfoRepresentation.licenseName = licenseName;
	
	QString description = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_DESCRIPTION).toString();
	metaInfoRepresentation.description = description;
	
	QByteArray productId = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_PRODUCT_ID).toByteArray();
	metaInfoRepresentation.productId = productId;
	
	return true;
}


bool CLicenseMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.licenseId){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_ID, *metaInfoRepresentation.licenseId);
	}
	
	if (metaInfoRepresentation.licenseName){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_NAME, *metaInfoRepresentation.licenseName);
	}
	
	if (metaInfoRepresentation.description){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_DESCRIPTION, *metaInfoRepresentation.description);
	}
	
	if (metaInfoRepresentation.productId){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_PRODUCT_ID, *metaInfoRepresentation.productId);
	}
	
	return true;
}


} // namespace imtlicgql


