#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlicgql/CFeaturePackageCollectionControllerComp.h>
#include <imtlicgql/CProductCollectionControllerComp.h>
#include <imtlicgql/CAccountCollectionControllerComp.h>
#include <imtlicgql/CInstallationCollectionControllerComp.h>
#include <imtlicgql/CInstallationControllerComp.h>
#include <imtlicgql/CPackageControllerComp.h>
#include <imtlicgql/CProductControllerComp.h>
#include <imtlicgql/CLicenseControllerComp.h>
#include <imtlicgql/CLicenseCollectionControllerComp.h>
#include <imtlicgql/CAccountControllerComp.h>
#include <imtlicgql/CFeaturesExportProviderComp.h>
#include <imtlicgql/CProductPermissionsControllerComp.h>
#include <imtlicgql/CFeatureControllerComp.h>
#include <imtlicgql/CFeatureCollectionControllerComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGqlPck
{


typedef imtlicgql::CFeaturePackageCollectionControllerComp FeaturePackageCollectionController;
typedef imtlicgql::CPackageControllerComp PackageController;
typedef imtlicgql::CProductCollectionControllerComp ProductCollectionController;
typedef imtlicgql::CProductControllerComp ProductController;
typedef imtlicgql::CLicenseControllerComp LicenseController;
typedef imtlicgql::CLicenseCollectionControllerComp LicenseCollectionController;
typedef imtlicgql::CAccountCollectionControllerComp AccountCollectionController;
typedef imtlicgql::CAccountControllerComp AccountController;
typedef imtlicgql::CInstallationCollectionControllerComp InstallationCollectionController;
typedef imtlicgql::CInstallationControllerComp InstallationController;
typedef imtlicgql::CFeaturesExportProviderComp FeaturesExportProvider;
typedef imtlicgql::CProductPermissionsControllerComp ProductPermissionController;
typedef imtlicgql::CFeatureControllerComp FeatureController;
typedef imtlicgql::CFeatureCollectionControllerComp FeatureCollectionController;


} // namespace ImtLicenseGqlPck


