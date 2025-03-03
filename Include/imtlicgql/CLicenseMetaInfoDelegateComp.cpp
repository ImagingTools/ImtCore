#include <imtlicgql/CLicenseMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtlic/ILicenseDefinition.h>


namespace imtlicgql
{


// protected methods

bool CLicenseMetaInfoDelegateComp::FillRepresentation(sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray licenseId = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_ID).toByteArray();
	metaInfoRepresentation.LicenseId = licenseId;
	
	QString licenseName = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_NAME).toString();
	metaInfoRepresentation.LicenseName = licenseName;
	
	QString description = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_DESCRIPTION).toString();
	metaInfoRepresentation.Description = description;
	
	QByteArray productId = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_PRODUCT_ID).toByteArray();
	metaInfoRepresentation.ProductId = productId;
	
	return true;
}


bool CLicenseMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.LicenseId){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_ID, *metaInfoRepresentation.LicenseId);
	}
	
	if (metaInfoRepresentation.LicenseName){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_NAME, *metaInfoRepresentation.LicenseName);
	}
	
	if (metaInfoRepresentation.Description){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_DESCRIPTION, *metaInfoRepresentation.Description);
	}
	
	if (metaInfoRepresentation.ProductId){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_PRODUCT_ID, *metaInfoRepresentation.ProductId);
	}
	
	return true;
}


} // namespace imtlicgql


