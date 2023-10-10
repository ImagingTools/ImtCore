#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtlic/CFeatureInfoComp.h>
#include <imtlic/CLicenseInfoManager.h>
#include <imtlic/CLicenseBasedRightsProviderComp.h>
#include <imtlic/CProductLicensingInfo.h>
#include <imtlic/CProductCollection.h>
#include <imtlic/CProductInstanceInfoComp.h>
#include <imtlic/CProductInstanceCollection.h>
#include <imtlic/CProductLicensingInfoComp.h>
#include <imtlic/CProductLicensingInfoMetaInfoCreatorComp.h>
#include <imtlic/CProductInstanceMetaInfoCreatorComp.h>
#include <imtlic/CLicenseControllerComp.h>
#include <imtlic/CFeatureContainer.h>
#include <imtlic/CProductInfoProviderComp.h>
#include <imtlic/CFeatureInfoProviderComp.h>
#include <imtlic/CFeatureDependenciesProviderComp.h>
#include <imtlic/CPermissionsProviderComp.h>
#include <imtlic/CProductInfo.h>


/**
	ImtLicensePck package
*/
namespace ImtLicensePck
{


typedef icomp::TModelCompWrap<imtlic::CFeatureInfoComp> FeatureInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CLicenseInfoManager,
						imtlic::ILicenseInfoProvider,
						imtbase::IObjectCollection,
						imtbase::IObjectCollectionInfo,
						imtbase::ICollectionInfo,
						iser::ISerializable>> LicenseInfoManager;
typedef icomp::TModelCompWrap<imtlic::CLicenseBasedRightsProviderComp> LicenseBasedRightsProvider;
typedef icomp::TModelCompWrap<imtlic::CProductLicensingInfoComp> ProductLicensingInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CProductCollection,
						imtlic::ILicenseInfoProvider,
						imtlic::IProductLicensingInfo,
						imtlic::IProductInfo,
						iser::ISerializable>> ProductCollection;
typedef icomp::TModelCompWrap<imtlic::CProductInstanceInfoComp> ProductInstanceInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CProductInstanceCollection,
						imtlic::IProductInstanceInfoProvider,
						iser::ISerializable>> ProductInstanceCollection;
typedef imtlic::CProductLicensingInfoMetaInfoCreatorComp ProductLicensingInfoMetaInfoCreator;
typedef imtlic::CProductInstanceMetaInfoCreatorComp ProductInstanceMetaInfoCreator;
typedef imtlic::CLicenseControllerComp LicenseController;
typedef imtlic::CProductInfoProviderComp ProductInfoProvider;
typedef imtlic::CFeatureInfoProviderComp FeatureInfoProvider;
typedef imtlic::CFeatureDependenciesProviderComp FeatureDependenciesProvider;
typedef imtlic::CPermissionsProviderComp PermissionsProvider;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CProductInfo,
						imtlic::IProductInfo,
						iprm::INameParam,
						iser::ISerializable>> ProductInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CLicenseInfo,
						imtlic::ILicenseInfo,
						iser::IObject,
						iser::ISerializable>> LicenseInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CFeatureContainer,
						imtbase::IObjectCollection,
						imtlic::IFeatureInfoProvider,
						iser::ISerializable>> FeatureContainer;

} // namespace ImtLicensePck


