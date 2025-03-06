#include "ImtDatabasePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDatabasePck
{


I_EXPORT_PACKAGE(
			"ImtDatabasePck",
			"Database-related component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			SqlDatabaseEngine,
			"SQL-based database engine",
			"SQL Database");

I_EXPORT_COMPONENT(
			SqlDatabaseCollection,
			"SQL-based object collection",
			"SQL Database");

I_EXPORT_COMPONENT(
			SqlDatabaseCollectionSearchController,
			"SQL-based object collection",
			"Sql Database Collection Search Controller")

I_EXPORT_COMPONENT(
			DatabaseAccessSettings,
			"Settings for database access",
			"Database User Access Login Settings Parameters");

I_EXPORT_COMPONENT(
			SqlDatabaseDocumentDelegate,
			"Delegate for the document-oriented tables",
			"Database Document");

I_EXPORT_COMPONENT(
			SqlDatabaseJsonDelegate,
			"Delegate for the json document-oriented tables",
			"Database Document Json SQL");

I_EXPORT_COMPONENT(
			StandardMetaInfoTableDelegate,
			"Delegate for the meta-info table",
			"Database Document Meta");

I_EXPORT_COMPONENT(
			DatabaseAutomaticBackup,
			"Database automatic backup",
			"Database Automatic Backup PostgreSQL SQL");

I_EXPORT_COMPONENT(
			RestoringDatabaseController,
			"Restoring database controller",
			"Restoring Restore Database Controller PostreSQL psql");

I_EXPORT_COMPONENT(
			SqlStructureDelegate,
			"Delegate for the json-oriented hierarchical structure tables",
			"Database Delegate Structure Json SQL");

I_EXPORT_COMPONENT(
			SqliteJsonDatabaseDelegate,
			"Delegate for SQLite json-oriented hierarchical structure tables",
			"Database Delegate Structure Json SQLite SQL");

I_EXPORT_COMPONENT(
			SqliteJsonMessageDatabaseDelegate,
			"Message object json delegate for SQL table",
			"SQL JSON Message Delegate");

I_EXPORT_COMPONENT(
			MigrationController,
			"Migration controller",
			"Migration Controller");

I_EXPORT_COMPONENT(
			CompositeMigrationController,
			"Composite migration controller",
			"Composite Migration Controller");

I_EXPORT_COMPONENT(
			ObjectCollectionMigrationController,
			"ObjectCollection migration controller",
			"ObjectCollection Migration Controller");

I_EXPORT_COMPONENT(
			DatabaseConnector,
			"Database connector",
			"Database Connector");

I_EXPORT_COMPONENT(
			DependentTableMetaInfoController,
			"Dependent table meta info controller",
			"Dependent Table Meta Info Controller");


} // namespace ImtDatabasePck


