#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>


/**
	ImtDatabasePck package
*/
namespace ImtDatabasePck
{


typedef imtdb::CDatabaseEngineComp SqlDatabaseEngine;
typedef icomp::TModelCompWrap<imtdb::CSqlDatabaseObjectCollectionComp> SqlDatabaseCollection;


} // namespace ImtDatabasePck


