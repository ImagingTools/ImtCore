// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>


// forward declarations
namespace imtlicgql
{
class CProductCollectionControllerComp;
class CProductControllerComp;
class CLicenseControllerComp;
class CLicenseCollectionControllerComp;
class CAccountControllerComp;
class CProductPermissionsControllerComp;
class CFeatureControllerComp;
class CFeatureCollectionControllerComp;
class CFeatureDependenciesProviderComp;
class CProductInfoProviderComp;
class CPermissionsProviderComp;
class CFeatureMetaInfoDelegateComp;
class CProductMetaInfoDelegateComp;
class CLicenseMetaInfoDelegateComp;
class CProductInfoFileGeneratorComp;
} // namespace imtlicgql


/**
	ImtLicenseGqlPck package
*/
namespace ImtLicenseGqlPck
{


typedef imtlicgql::CProductCollectionControllerComp ProductCollectionController;
typedef imtlicgql::CProductControllerComp ProductController;
typedef imtlicgql::CLicenseControllerComp LicenseController;
typedef imtlicgql::CLicenseCollectionControllerComp LicenseCollectionController;
typedef imtlicgql::CAccountControllerComp AccountController;
typedef imtlicgql::CProductPermissionsControllerComp ProductPermissionController;
typedef imtlicgql::CFeatureControllerComp FeatureController;
typedef imtlicgql::CFeatureCollectionControllerComp FeatureCollectionController;
typedef imtlicgql::CFeatureDependenciesProviderComp FeatureDependenciesProvider;
typedef imtlicgql::CProductInfoProviderComp ProductInfoProvider;
typedef imtlicgql::CPermissionsProviderComp PermissionsProvider;
typedef imtlicgql::CFeatureMetaInfoDelegateComp FeatureMetaInfoDelegate;
typedef imtlicgql::CProductMetaInfoDelegateComp ProductMetaInfoDelegate;
typedef imtlicgql::CLicenseMetaInfoDelegateComp LicenseMetaInfoDelegate;
typedef imtlicgql::CProductInfoFileGeneratorComp ProductInfoFileGenerator;


} // namespace ImtLicenseGqlPck


