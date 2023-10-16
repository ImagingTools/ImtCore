#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlicgql/CProductCollectionControllerComp.h>
#include <imtlicgql/CAccountCollectionControllerComp.h>
#include <imtlicgql/CProductControllerComp.h>
#include <imtlicgql/CLicenseControllerComp.h>
#include <imtlicgql/CLicenseCollectionControllerComp.h>
#include <imtlicgql/CAccountControllerComp.h>
#include <imtlicgql/CFeaturesExportProviderComp.h>
#include <imtlicgql/CProductPermissionsControllerComp.h>
#include <imtlicgql/CFeatureControllerComp.h>
#include <imtlicgql/CFeatureCollectionControllerComp.h>
#include <imtlicgql/CFeatureDependenciesProviderComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGqlPck
{


typedef imtlicgql::CProductCollectionControllerComp ProductCollectionController;
typedef imtlicgql::CProductControllerComp ProductController;
typedef imtlicgql::CLicenseControllerComp LicenseController;
typedef imtlicgql::CLicenseCollectionControllerComp LicenseCollectionController;
typedef imtlicgql::CAccountCollectionControllerComp AccountCollectionController;
typedef imtlicgql::CAccountControllerComp AccountController;
typedef imtlicgql::CFeaturesExportProviderComp FeaturesExportProvider;
typedef imtlicgql::CProductPermissionsControllerComp ProductPermissionController;
typedef imtlicgql::CFeatureControllerComp FeatureController;
typedef imtlicgql::CFeatureCollectionControllerComp FeatureCollectionController;
typedef imtlicgql::CFeatureDependenciesProviderComp FeatureDependenciesProvider;


} // namespace ImtLicenseGqlPck


