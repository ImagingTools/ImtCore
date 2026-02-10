// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
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
#include <imtlicgql/CProductInfoFileGeneratorComp.h>


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


