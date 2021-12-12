#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtlicdb/CFeaturePackageDatabaseDelegateComp.h>
#include <imtlicdb/CProductsDatabaseDelegateComp.h>
#include <imtlicdb/CProductInstanceDatabaseDelegateComp.h>
#include <imtlicdb/CAccountDatabaseDelegateComp.h>


/**
	ImtLicenseDbPck package
*/
namespace ImtLicenseDbPck
{


typedef imtlicdb::CFeaturePackageDatabaseDelegateComp FeaturePackageDatabaseDelegate;
typedef imtlicdb::CProductsDatabaseDelegateComp ProductDatabaseDelegate;
typedef imtlicdb::CAccountDatabaseDelegateComp AccountDatabaseDelegate;
typedef imtlicdb::CProductInstanceDatabaseDelegateComp ProductInstanceDatabaseDelegate;


} // namespace ImtLicenseDbPck


