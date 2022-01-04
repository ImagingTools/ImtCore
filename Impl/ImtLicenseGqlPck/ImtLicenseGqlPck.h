#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlicgql/CFeaturePackageCollectionControllerComp.h>
#include <imtlicgql/CProductCollectionControllerComp.h>
#include <imtlicgql/CAccountCollectionControllerComp.h>
#include <imtlicgql/CInstallationCollectionControllerComp.h>
#include <imtlicgql/CPackageControllerComp.h>
#include <imtlicgql/CAccountControllerComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGqlPck
{


typedef imtlicgql::CFeaturePackageCollectionControllerComp FeaturePackageCollectionController;
typedef imtlicgql::CProductCollectionControllerComp ProductCollectionController;
typedef imtlicgql::CAccountCollectionControllerComp AccountCollectionController;
typedef imtlicgql::CAccountControllerComp AccountController;
typedef imtlicgql::CInstallationCollectionControllerComp InstallationCollectionController;
typedef imtlicgql::CPackageControllerComp PackageController;


} // namespace ImtLicenseGqlPck


