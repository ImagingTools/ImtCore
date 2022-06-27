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
#include <imtlicgql/CAccountControllerComp.h>
#include <imtlicgql/CKeyDataProviderComp.h>
#include <imtlicgql/CUserCollectionControllerComp.h>
#include <imtlicgql/CUserControllerComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGqlPck
{


typedef imtlicgql::CFeaturePackageCollectionControllerComp FeaturePackageCollectionController;
typedef imtlicgql::CPackageControllerComp PackageController;
typedef imtlicgql::CProductCollectionControllerComp ProductCollectionController;
typedef imtlicgql::CProductControllerComp ProductController;
typedef imtlicgql::CAccountCollectionControllerComp AccountCollectionController;
typedef imtlicgql::CAccountControllerComp AccountController;
typedef imtlicgql::CInstallationCollectionControllerComp InstallationCollectionController;
typedef imtlicgql::CInstallationControllerComp InstallationController;
typedef imtlicgql::CKeyDataProviderComp KeyDataProvider;
typedef imtlicgql::CUserCollectionControllerComp UserCollectionController;
typedef imtlicgql::CUserControllerComp UserController;


} // namespace ImtLicenseGqlPck


