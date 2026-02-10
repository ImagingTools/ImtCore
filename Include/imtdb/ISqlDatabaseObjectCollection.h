// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtdb
{


/**
 * @interface ISqlDatabaseObjectCollection
 * @brief SQL-specific extension of IObjectCollection for database-backed object collections
 * 
 * ISqlDatabaseObjectCollection extends the base IObjectCollection interface with SQL-specific
 * functionality, particularly transaction management control. This interface is used by
 * collections that store objects in SQL databases (PostgreSQL, SQLite, etc.).
 * 
 * Key features:
 * - Database identification for multi-database scenarios
 * - Fine-grained transaction control (enable/disable internal transactions)
 * - Integration with CSqlDatabaseTransactionManagerComp for coordinated transactions
 * 
 * @section isql_coll_transactions Transaction Control
 * 
 * The collection supports two transaction modes:
 * 
 * **Internal Transactions Enabled** (default):
 * - Each operation (insert, update, delete) runs in its own transaction
 * - Automatic commit/rollback per operation
 * - Suitable for independent operations
 * 
 * **Internal Transactions Disabled**:
 * - Operations do not create their own transactions
 * - Must be used within an external transaction
 * - Required for multi-collection transactions via CSqlDatabaseTransactionManagerComp
 * 
 * @code{.cpp}
 * // Single collection operations (internal transactions enabled)
 * collection->InsertNewObject(object, id);  // Auto-commit
 * 
 * // Multi-collection transactions (internal transactions disabled)
 * auto txManager = acf::CreateComponent<CSqlDatabaseTransactionManagerComp>();
 * txManager->AddCollection(collection1);
 * txManager->AddCollection(collection2);
 * 
 * txManager->StartTransaction();  // Disables internal transactions
 * collection1->InsertNewObject(obj1, id1);
 * collection2->InsertNewObject(obj2, id2);
 * txManager->EndTransaction(true);  // Commits both, re-enables internal transactions
 * @endcode
 * 
 * @see imtbase::IObjectCollection for base collection operations
 * @see CSqlDatabaseObjectCollectionComp for primary implementation
 * @see CSqlDatabaseTransactionManagerComp for coordinated multi-collection transactions
 */
class ISqlDatabaseObjectCollection: virtual public imtbase::IObjectCollection
{
public:
	/**
	 * @brief Gets the unique identifier of the database hosting this collection
	 * 
	 * Returns a unique identifier for the database connection used by this collection.
	 * Useful for:
	 * - Determining if multiple collections share the same database
	 * - Coordinating transactions across collections in the same database
	 * - Debugging and logging database operations
	 * 
	 * @return Unique database identifier (typically connection name or database name hash)
	 * 
	 * @note Collections with the same database ID can participate in shared transactions
	 * @see CSqlDatabaseTransactionManagerComp
	 */
	virtual QByteArray GetDatabaseId() const = 0;

	/**
	 * @brief Checks if internal transactions are enabled for this collection
	 * 
	 * Returns whether the collection automatically wraps each operation (insert, update,
	 * delete) in its own transaction.
	 * 
	 * @return true if internal transactions are enabled (default), false if disabled
	 * 
	 * @note Internal transactions are temporarily disabled when participating in
	 *       multi-collection transactions via CSqlDatabaseTransactionManagerComp
	 * 
	 * @see SetInternalTransactionsEnabled()
	 */
	virtual bool AreInternalTransactionsEnabled() const = 0;

	/**
	 * @brief Enables or disables internal transaction wrapping
	 * 
	 * Controls whether each collection operation runs in its own transaction.
	 * 
	 * When to disable internal transactions:
	 * - Coordinating operations across multiple collections
	 * - Using CSqlDatabaseTransactionManagerComp
	 * - Manual transaction management via IDatabaseEngine
	 * - Batch operations that should be atomic
	 * 
	 * When to enable internal transactions:
	 * - Default mode for independent operations
	 * - Single-collection scenarios
	 * - Automatic commit/rollback per operation desired
	 * 
	 * @param isEnabled true to enable internal transactions, false to disable
	 * @return true if setting changed successfully, false on error
	 * 
	 * @warning Disabling internal transactions without an external transaction
	 *          means operations will run without transaction protection (not recommended)
	 * 
	 * @note CSqlDatabaseTransactionManagerComp automatically manages this setting
	 * 
	 * @see AreInternalTransactionsEnabled()
	 * @see CSqlDatabaseTransactionManagerComp
	 */
	virtual bool SetInternalTransactionsEnabled(bool isEnabled) = 0;
};


} // namespace imtdb


