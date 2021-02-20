#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtlic/CLicenseInfo.h>
#include <imtlic/CLicenseInfoManager.h>
#include <imtlic/CLicenseBasedRightsProviderComp.h>
#include <imtlic/CProductLicensingInfo.h>
#include <imtlic/CProductCollection.h>
#include <imtlic/CProductInstanceCollection.h>
#include <imtlic/CProductLicensingInfo.h>


/**
	ImtLicensePck package
*/
namespace ImtLicensePck
{


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
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtlic::CProductLicensingInfo,
						imtlic::IProductLicensingInfo,
						iser::ISerializable>> ProductLicensingInfo;
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


} // namespace ImtLicensePck


