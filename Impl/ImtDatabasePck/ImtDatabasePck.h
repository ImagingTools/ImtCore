#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>
#include <imtdb/CDatabaseAccessSettingsComp.h>
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>
#include <imtdb/CStandardSqlMetaInfoTableDelegateComp.h>
#include <imtdb/CDatabaseAutomaticBackupComp.h>
#include <imtdb/CRemoteDatabaseServerConnectionComp.h>
#include <imtdb/CDatabaseServerConnectionControllerComp.h>
#include <imtdb/CRestoringDatabaseControllerComp.h>
#include <imtdb/CSqlStructureDelegateComp.h>


/**
	ImtDatabasePck package
*/
namespace ImtDatabasePck
{


typedef imtdb::CDatabaseEngineComp SqlDatabaseEngine;
typedef icomp::TModelCompWrap<imtdb::CSqlDatabaseObjectCollectionComp> SqlDatabaseCollection;
typedef icomp::TModelCompWrap<imtdb::CDatabaseAccessSettingsComp> DatabaseAccessSettings;
typedef imtdb::CSqlDatabaseDocumentDelegateComp SqlDatabaseDocumentDelegate;
typedef imtdb::CSqlJsonDatabaseDelegateComp SqlDatabaseJsonDelegate;
typedef imtdb::CStandardSqlMetaInfoTableDelegateComp StandardMetaInfoTableDelegate;
typedef imtdb::CDatabaseAutomaticBackupComp DatabaseAutomaticBackup;
typedef imtdb::CRemoteDatabaseServerConnectionComp RemoteDatabaseServerConnection;
typedef imtdb::CDatabaseServerConnectionControllerComp DatabaseServerConnectionController;
typedef imtdb::CRestoringDatabaseControllerComp RestoringDatabaseController;
typedef imtdb::CSqlStructureDelegateComp SqlStructureDelegate;


} // namespace ImtDatabasePck


