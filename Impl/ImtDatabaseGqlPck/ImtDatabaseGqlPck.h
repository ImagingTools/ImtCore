#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtdbgql/CStructureControllerComp.h>
#include <imtdbgql/CRemoteDatabaseServerConnectionComp.h>
#include <imtdbgql/CDatabaseServerConnectionControllerComp.h>


/**
	ImtDatabaseGqlPck package
*/
namespace ImtDatabaseGqlPck
{


typedef imtdbgql::CStructureControllerComp StructureController;
typedef imtdbgql::CRemoteDatabaseServerConnectionComp RemoteDatabaseServerConnection;
typedef imtdbgql::CDatabaseServerConnectionControllerComp DatabaseServerConnectionController;


} // namespace ImtDatabaseGqlPck


