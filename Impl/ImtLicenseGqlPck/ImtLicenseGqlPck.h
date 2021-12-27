#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlicgql/CFeaturePackageCollectionControllerComp.h>
#include <imtlicgql/CProductCollectionControllerComp.h>
#include <imtlicgql/CAccountCollectionControllerComp.h>
#include <imtlicgql/CInstallationCollectionControllerComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGqlPck
{


typedef imtlicgql::CFeaturePackageCollectionControllerComp FeaturePackageCollectionController;
typedef imtlicgql::CProductCollectionControllerComp ProductCollectionController;
typedef imtlicgql::CAccountCollectionControllerComp AccountCollectionController;
typedef imtlicgql::CInstallationCollectionControllerComp InstallationCollectionController;


} // namespace ImtLicenseGqlPck


