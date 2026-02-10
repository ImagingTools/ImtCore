// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>
#include <imtdb/CDatabaseAccessSettingsComp.h>
#include <imtdb/CSqlDatabaseDocumentDelegateLegacyComp.h>
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>
#include <imtdb/CStandardSqlMetaInfoTableDelegateComp.h>
#include <imtdb/CDatabaseAutomaticBackupComp.h>
#include <imtdb/CRestoringDatabaseControllerComp.h>
#include <imtdb/CSqlStructureDelegateComp.h>
#include <imtdb/CSqliteJsonDatabaseDelegateComp.h>
#include <imtdb/TMessageDatabaseDelegateComp.h>
#include <imtdb/CMigrationControllerComp.h>
#include <imtdb/CCompositeMigrationControllerComp.h>
#include <imtdb/CObjectCollectionMigrationControllerComp.h>
#include <imtdb/CDatabaseConnectorComp.h>
#include <imtdb/CDependentTableMetaInfoControllerComp.h>
#include <imtdb/CSqliteDatabaseDocumentDelegateComp.h>
#include <imtdb/CPostgresXPathExtractorComp.h>
#include <imtdb/CSqliteXPathExtractorComp.h>
#include <imtdb/CSqlDatabaseTransactionManagerComp.h>


/**
	ImtDatabasePck package
*/
namespace ImtDatabasePck
{


typedef imtdb::CDatabaseEngineComp SqlDatabaseEngine;
typedef icomp::TModelCompWrap<imtdb::CSqlDatabaseObjectCollectionComp> SqlDatabaseCollection;
typedef icomp::TModelCompWrap<imtdb::CDatabaseAccessSettingsComp> DatabaseAccessSettings;
typedef imtdb::CSqlDatabaseDocumentDelegateLegacyComp SqlDatabaseDocumentDelegateLegacy;
typedef imtdb::CSqlDatabaseDocumentDelegateComp SqlDatabaseDocumentDelegate;
typedef imtdb::CSqlJsonDatabaseDelegateComp SqlDatabaseJsonDelegate;
typedef imtdb::CStandardSqlMetaInfoTableDelegateComp StandardMetaInfoTableDelegate;
typedef imtdb::CDatabaseAutomaticBackupComp DatabaseAutomaticBackup;
typedef imtdb::CRestoringDatabaseControllerComp RestoringDatabaseController;
typedef imtdb::CSqlStructureDelegateComp SqlStructureDelegate;
typedef imtdb::CSqliteJsonDatabaseDelegateComp SqliteJsonDatabaseDelegate;
typedef imtdb::TMessageDatabaseDelegateComp<imtdb::CSqliteDatabaseDocumentDelegateComp> SqliteJsonMessageDatabaseDelegate;
typedef imtdb::CMigrationControllerComp MigrationController;
typedef imtdb::CCompositeMigrationControllerComp CompositeMigrationController;
typedef imtdb::CObjectCollectionMigrationControllerComp ObjectCollectionMigrationController;
typedef imtdb::CDatabaseConnectorComp DatabaseConnector;
typedef imtdb::CDependentTableMetaInfoControllerComp DependentTableMetaInfoController;
typedef imtdb::CSqliteDatabaseDocumentDelegateComp SQLiteDatabaseDocumentDelegate;
typedef imtdb::CPostgresXPathExtractorComp PostgresXPathExtractor;
typedef imtdb::CSqliteXPathExtractorComp SqliteXPathExtractor;
typedef imtdb::CSqlDatabaseTransactionManagerComp SqlDatabaseTransactionManager;


} // namespace ImtDatabasePck


