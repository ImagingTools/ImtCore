// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtdb
{


/**
 * @interface IMigrationController
 * @brief Controls database schema versioning and migration execution
 * 
 * IMigrationController manages the evolution of database schemas through versioned
 * migration scripts. It tracks the current database version and executes SQL scripts
 * to upgrade (or downgrade) the schema incrementally.
 * 
 * @section imc_overview Migration Concept
 * 
 * Migrations are SQL scripts that transform a database schema from one version to another.
 * Each migration has:
 * - A source version number (e.g., v001)
 * - A target version number (e.g., v002)
 * - SQL statements to perform the transformation
 * 
 * Typical migration file structure:
 * @verbatim
 * migrations/
 *   migration_v001_to_v002.sql  -- Create initial tables
 *   migration_v002_to_v003.sql  -- Add user email column
 *   migration_v003_to_v004.sql  -- Create indexes
 * @endverbatim
 * 
 * @section imc_usage Usage Pattern
 * 
 * @code{.cpp}
 * auto migrationCtrl = acf::CreateComponent<imtdb::CMigrationControllerComp>();
 * migrationCtrl->SetDatabaseEngine(engine);
 * migrationCtrl->SetMigrationFolder("./migrations/");
 * 
 * // Check what migrations are needed
 * istd::CIntRange range = migrationCtrl->GetMigrationRange();
 * // range.first() = current database version
 * // range.last() = latest available migration version
 * 
 * if (range.first() < range.last()) {
 *     // Migrations needed
 *     int resultVersion = 0;
 *     if (migrationCtrl->DoMigration(resultVersion)) {
 *         // Database upgraded to resultVersion
 *     }
 * }
 * @endcode
 * 
 * @section imc_composite Composite Migrations
 * 
 * For complex systems with multiple database schemas or modules, use
 * CCompositeMigrationControllerComp to coordinate multiple migration controllers:
 * 
 * @code{.cpp}
 * auto composite = acf::CreateComponent<CCompositeMigrationControllerComp>();
 * composite->AddController(authMigrations);
 * composite->AddController(dataMigrations);
 * composite->DoMigration(resultVersion);  // Executes all sequentially
 * @endcode
 * 
 * @note Migrations should be idempotent (safe to run multiple times)
 * @note Always backup database before running migrations on production data
 * @see CMigrationControllerComp for implementation
 * @see CCompositeMigrationControllerComp for multi-controller coordination
 */
class IMigrationController: virtual public istd::IPolymorphic
{
public:
	/**
	 * @brief Gets the range of database versions for migration
	 * 
	 * Returns the version range from current database version to the latest
	 * available migration version.
	 * 
	 * @return CIntRange where:
	 *         - first() = current database schema version
	 *         - last() = latest available migration version
	 * 
	 * If first() == last(), no migrations are needed (database is up-to-date).
	 * If first() < last(), migrations are available and should be executed.
	 * 
	 * @code{.cpp}
	 * istd::CIntRange range = controller->GetMigrationRange();
	 * if (range.first() < range.last()) {
	 *     std::cout << "Migrations available: " 
	 *               << range.first() << " -> " << range.last() << std::endl;
	 * }
	 * @endcode
	 * 
	 * @see DoMigration()
	 */
	virtual istd::CIntRange GetMigrationRange() const = 0;
	
	/**
	 * @brief Executes database migrations
	 * 
	 * Runs migration scripts to transform the database schema. Can execute all pending
	 * migrations or a specific subset defined by subRange.
	 * 
	 * The migration process:
	 * 1. Determines current database version
	 * 2. Identifies migration scripts to execute
	 * 3. For each migration:
	 *    - Begins transaction
	 *    - Executes SQL script
	 *    - Updates version table
	 *    - Commits (or rolls back on error)
	 * 
	 * @param[out] resultRevision Receives the final database version after migration
	 * @param subRange Optional range limiting which migrations to execute:
	 *                 - Empty (default): Execute all pending migrations
	 *                 - Specified: Execute only migrations within this version range
	 * @return true if all migrations succeeded, false if any migration failed
	 * 
	 * @note If migration fails, database is rolled back to pre-migration state (for that step)
	 * @note Migrations execute sequentially in version order
	 * @note Each migration runs in its own transaction for atomicity
	 * 
	 * @warning Always backup production databases before migrations
	 * @warning Failed migrations may leave database in partially migrated state
	 * 
	 * @code{.cpp}
	 * int finalVersion = 0;
	 * if (controller->DoMigration(finalVersion)) {
	 *     std::cout << "Migrated to version: " << finalVersion << std::endl;
	 * } else {
	 *     std::cerr << "Migration failed!" << std::endl;
	 * }
	 * 
	 * // Migrate to specific version
	 * istd::CIntRange partialRange(currentVer, targetVer);
	 * controller->DoMigration(finalVersion, partialRange);
	 * @endcode
	 * 
	 * @see GetMigrationRange()
	 */
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const = 0;
};


} // namespace imtdb


