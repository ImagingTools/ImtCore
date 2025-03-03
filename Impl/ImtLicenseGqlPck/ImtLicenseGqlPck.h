#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtservergql/TObjectCollectionControllerSearchCompWrap.h>
#include <imtlicgql/CProductCollectionControllerComp.h>
#include <imtlicgql/CProductControllerComp.h>
#include <imtlicgql/CLicenseControllerComp.h>
#include <imtlicgql/CLicenseCollectionControllerComp.h>
#include <imtlicgql/CAccountControllerComp.h>
#include <imtlicgql/CProductPermissionsControllerComp.h>
#include <imtlicgql/CFeatureControllerComp.h>
#include <imtlicgql/CFeatureCollectionControllerComp.h>
#include <imtlicgql/CFeatureDependenciesProviderComp.h>
#include <imtlicgql/CProductInfoProviderComp.h>
#include <imtlicgql/CPermissionsProviderComp.h>
#include <imtlicgql/CFeatureMetaInfoDelegateComp.h>
#include <imtlicgql/CProductMetaInfoDelegateComp.h>
#include <imtlicgql/CLicenseMetaInfoDelegateComp.h>


/**
	ImtLicenseGqlPck package
*/
namespace ImtLicenseGqlPck
{


typedef imtservergql::TObjectCollectionControllerSearchCompWrap<imtlicgql::CProductCollectionControllerComp> ProductCollectionController;
typedef imtlicgql::CProductControllerComp ProductController;
typedef imtlicgql::CLicenseControllerComp LicenseController;
typedef imtservergql::TObjectCollectionControllerSearchCompWrap<imtlicgql::CLicenseCollectionControllerComp> LicenseCollectionController;
typedef imtlicgql::CAccountControllerComp AccountController;
typedef imtlicgql::CProductPermissionsControllerComp ProductPermissionController;
typedef imtlicgql::CFeatureControllerComp FeatureController;
typedef imtservergql::TObjectCollectionControllerSearchCompWrap<imtlicgql::CFeatureCollectionControllerComp> FeatureCollectionController;
typedef imtlicgql::CFeatureDependenciesProviderComp FeatureDependenciesProvider;
typedef imtlicgql::CProductInfoProviderComp ProductInfoProvider;
typedef imtlicgql::CPermissionsProviderComp PermissionsProvider;
typedef imtlicgql::CFeatureMetaInfoDelegateComp FeatureMetaInfoDelegate;
typedef imtlicgql::CProductMetaInfoDelegateComp ProductMetaInfoDelegate;
typedef imtlicgql::CLicenseMetaInfoDelegateComp LicenseMetaInfoDelegate;


} // namespace ImtLicenseGqlPck


