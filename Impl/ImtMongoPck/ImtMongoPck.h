#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes

#include <imtmongo/CMongoDatabaseObjectCollectionComp.h>
#include <imtmongo/CMongoDatabaseEngineComp.h>


/**
	ImtDatabasePck package
*/
namespace ImtDatabasePck
{


typedef icomp::TModelCompWrap<imtmongo::CMongoDatabaseObjectCollectionComp> MongoDatabaseCollection;
typedef imtmongo::CMongoDatabaseEngineComp MongoDatabaseEngine;


} // namespace ImtDatabasePck


