#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtlic/CLicenseInfo.h>
#include <imtlic/CLicenseManager.h>
#include <imtlic/CLicenseBasedRightsProviderComp.h>


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
						imtlic::CLicenseManager,
						imtlic::ILicenseInfoProvider,
						imtbase::IObjectCollection,
						imtbase::IObjectCollectionInfo,
						imtbase::ICollectionInfo,
						iser::ISerializable>> LicenseManager;
typedef imtlic::CLicenseBasedRightsProviderComp LicenseBasedRightsProvider;


} // namespace ImtLicensePck


