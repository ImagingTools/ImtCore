#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CFeatureInfoProviderComp.h>
#include <imtlic/CLicenseInfoManager.h>
#include <imtlic/CLicenseBasedRightsProviderComp.h>
#include <imtlic/CProductLicensingInfo.h>
#include <imtlic/CProductCollection.h>
#include <imtlic/CProductInstanceCollection.h>
#include <imtlic/CProductLicensingInfoComp.h>
#include <imtlic/CProductLicensingInfoMetaInfoCreatorComp.h>


/**
	ImtLicensePck package
*/
namespace ImtLicensePck
{


typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CFeatureInfo,
						imtlic::IFeatureInfo,
						iser::ISerializable>> FeatureInfo;
typedef icomp::TModelCompWrap<imtlic::CFeatureInfoProviderComp> FeatureInfoProvider;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CLicenseInfo,
						imtlic::ILicenseInfo,
						iser::ISerializable>> LicenseInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CLicenseInfoManager,
						imtlic::ILicenseInfoProvider,
						imtbase::IObjectCollection,
						imtbase::IObjectCollectionInfo,
						imtbase::ICollectionInfo,
						iser::ISerializable>> LicenseInfoManager;
typedef imtlic::CLicenseBasedRightsProviderComp LicenseBasedRightsProvider;
typedef icomp::TModelCompWrap<imtlic::CProductLicensingInfoComp> ProductLicensingInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CProductCollection,
						imtlic::ILicenseInfoProvider,
						imtlic::IProductLicensingInfo,
						imtlic::IProductInfo,
						iser::ISerializable>> ProductCollection;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CProductInstanceCollection,
						imtlic::IProductInstanceInfoProvider,
						iser::ISerializable>> ProductInstanceCollection;
typedef imtlic::CProductLicensingInfoMetaInfoCreatorComp ProductLicensingInfoMetaInfoCreator;


} // namespace ImtLicensePck


