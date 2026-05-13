// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtdb
{


/**
 * @interface IDatabaseObjectDelegate
 * @brief Core ORM interface for object-to-database mapping and SQL query generation
 * 
 * IDatabaseObjectDelegate is the heart of the imtdb ORM system. It acts as a bridge between
 * C++ object representations and database tables, generating SQL queries for all CRUD operations.
 * 
 * @section idod_purpose Purpose
 * 
 * The delegate pattern separates:
 * - **Collection logic** (CSqlDatabaseObjectCollectionComp) - manages object lifecycle
 * - **Database mapping** (IDatabaseObjectDelegate) - generates SQL queries
 * 
 * This separation enables:
 * - Multiple delegate implementations for different object types
 * - Database-specific query generation (PostgreSQL vs SQLite)
 * - Specialized delegates (documents, messages, hierarchies)
 * - Easy testing through mock delegates
 * 
 * @section idod_query_types Query Types
 * 
 * The delegate generates queries for:
 * - **Selection**: Retrieve objects with filtering, sorting, pagination
 * - **Insertion**: Create new objects with metadata
 * - **Update**: Modify existing objects
 * - **Deletion**: Soft or hard delete objects
 * - **Restoration**: Undelete soft-deleted objects
 * - **Metadata**: Update object metadata without full object modification
 * - **Count**: Efficient counting with filters
 * 
 * @section idod_implementations Common Implementations
 * 
 * - **CSqlDatabaseObjectDelegateCompBase**: Base implementation for standard objects
 * - **CSqlDatabaseDocumentDelegateComp**: Document objects with JSON metadata and revisions
 * - **CSqlJsonDatabaseDelegateComp**: JSON-serialized objects
 * - **CSqlStructureDelegateComp**: Hierarchical tree structures
 * - **TMessageDatabaseDelegateComp**: Template-based message persistence
 * 
 * @section idod_usage Usage Example
 * 
 * @code{.cpp}
 * // Create delegate
 * auto delegate = acf::CreateComponent<CSqlDatabaseDocumentDelegateComp>();
 * delegate->SetTableName("users");
 * delegate->SetDatabaseEngine(engine);
 * 
 * // Use with collection
 * auto collection = acf::CreateComponent<CSqlDatabaseObjectCollectionComp>();
 * collection->SetDelegate(delegate);
 * 
 * // Delegate automatically generates SQL for:
 * collection->InsertNewObject(user, userId);     // INSERT query
 * collection->GetObject(userId);                 // SELECT query
 * collection->UpdateObject(userId, user);        // UPDATE query
 * collection->RemoveElements({userId});          // DELETE/UPDATE query
 * @endcode
 * 
 * @note Delegates are typically created once and reused for a collection's lifetime
 * @see CSqlDatabaseObjectDelegateCompBase for base implementation
 * @see CSqlDatabaseObjectCollectionComp for usage context
 */
class IDatabaseObjectDelegate: virtual public istd::IPolymorphic
{
public:
	/**
	 * @struct NewObjectQuery
	 * @brief Result of CreateNewObjectQuery containing SQL and metadata for object insertion
	 */
	struct NewObjectQuery
	{
		/**
		 * @brief SQL INSERT query string
		 * 
		 * The generated SQL query for inserting the object into the database.
		 * May contain placeholders for parameterized values.
		 */
		QByteArray query;

		/**
		 * @brief Name of the object being inserted
		 * 
		 * Human-readable name used for display and identification.
		 */
		QString objectName;
		
		/**
		 * @brief Parameter bindings for the query
		 * 
		 * Map of placeholder names to values for parameterized query execution.
		 * Keys typically use `:name` syntax (e.g., `:objectId`, `:userName`).
		 */
		QVariantMap bindValues;
	};

	/**
	 * @brief Gets information about object types supported by this delegate
	 * 
	 * Returns a list of object types that this delegate can handle. Each type
	 * typically has a unique type ID, name, and associated metadata.
	 * 
	 * @return Pointer to options list containing type information, or nullptr if not applicable
	 * 
	 * @note Used for polymorphic collections where multiple object types share a table
	 * @note Type IDs are used in the TypeId column to distinguish object types
	 */
	virtual const iprm::IOptionsList* GetObjectTypeInfos() const = 0;

	/**
	 * @brief Gets the type ID for a specific object
	 * 
	 * Retrieves the type identifier for an object, used in polymorphic collections
	 * where multiple object types are stored in the same table.
	 * 
	 * @param objectId Unique identifier of the object
	 * @return Type ID as a byte array
	 * 
	 * @see GetObjectTypeInfos()
	 */
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const = 0;

	/**
	 * @brief Generates SQL query to count objects in collection
	 * 
	 * Creates a COUNT query, optionally filtered by criteria specified in paramsPtr.
	 * This is more efficient than retrieving all objects and counting in application code.
	 * 
	 * @param paramsPtr Optional filter parameters (WHERE clause conditions)
	 * @return SQL COUNT query string
	 * 
	 * @code{.cpp}
	 * // Count all users
	 * QByteArray countAll = delegate->GetCountQuery();
	 * 
	 * // Count users over 18
	 * auto params = acf::CreateComponent<iprm::CParamsSetComp>();
	 * params->SetValue("Age", 18, iprm::CT_GREATER_THAN);
	 * QByteArray countFiltered = delegate->GetCountQuery(params.get());
	 * @endcode
	 * 
	 * @see GetSelectionQuery()
	 */
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const = 0;

	/**
	 * @brief Generates SQL SELECT query for retrieving objects
	 * 
	 * Creates a SELECT query with optional filtering, pagination, and specific object selection.
	 * This is the primary method for retrieving objects from the database.
	 * 
	 * @param objectId If not empty, retrieves only the specified object (WHERE Id = objectId)
	 * @param offset Number of rows to skip (for pagination, e.g., OFFSET 20)
	 * @param count Maximum number of rows to return (for pagination, e.g., LIMIT 10)
	 * @param paramsPtr Additional query parameters:
	 *                  - Filtering: WHERE clauses (e.g., Name LIKE 'John%')
	 *                  - Sorting: ORDER BY clauses (e.g., Name ASC, Age DESC)
	 *                  - Searching: Full-text or pattern matching
	 * @return SQL SELECT query string
	 * 
	 * @code{.cpp}
	 * // Get specific object
	 * QByteArray query1 = delegate->GetSelectionQuery("user-123");
	 * 
	 * // Get page 3 (rows 20-29) with filter
	 * auto params = acf::CreateComponent<iprm::CParamsSetComp>();
	 * params->SetValue("Status", "active");
	 * QByteArray query2 = delegate->GetSelectionQuery(QByteArray(), 20, 10, params.get());
	 * @endcode
	 * 
	 * @note Empty objectId with offset/count=0/-1 returns all objects
	 * @see GetCountQuery(), CreateFilterQuery()
	 */
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const = 0;

	/**
	 * @brief Creates SQL query for inserting a new object
	 * 
	 * Generates INSERT query with all necessary fields: ID, TypeId, Name, Description,
	 * serialized data, metadata, timestamps, and operation context.
	 * 
	 * @param typeId Object type identifier (for polymorphic collections)
	 * @param proposedObjectId Suggested ID (may be auto-generated if empty)
	 * @param objectName Display name for the object
	 * @param objectDescription Optional description text
	 * @param valuePtr Object data to serialize into database
	 * @param operationContextPtr Context information (user, timestamp, operation description)
	 * @return NewObjectQuery struct containing SQL, object name, and parameter bindings
	 * 
	 * @code{.cpp}
	 * auto user = std::make_shared<User>();
	 * auto context = acf::CreateComponent<COperationContextComp>();
	 * context->SetUserId("admin");
	 * 
	 * auto newQuery = delegate->CreateNewObjectQuery(
	 *     "user",                    // typeId
	 *     QByteArray(),              // auto-generate ID
	 *     "John Doe",                // name
	 *     "System administrator",    // description
	 *     user.get(),                // object data
	 *     context.get()              // operation context
	 * );
	 * 
	 * engine->ExecSqlQuery(newQuery.query, newQuery.bindValues);
	 * @endcode
	 * 
	 * @see CreateUpdateObjectQuery()
	 */
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const = 0;

	/**
	 * @brief Creates SQL query for deleting objects
	 * 
	 * Generates DELETE or UPDATE query (for soft delete) to remove objects by their IDs.
	 * Soft delete typically sets a State column to indicate deletion without removing data.
	 * 
	 * @param collection The collection containing the objects
	 * @param objectIds List of object IDs to delete
	 * @param operationContextPtr Context for audit trail (who, when, why)
	 * @return SQL DELETE or UPDATE query string
	 * 
	 * @code{.cpp}
	 * imtbase::ICollectionInfo::Ids idsToDelete = {"user-1", "user-2", "user-3"};
	 * QByteArray deleteQuery = delegate->CreateDeleteObjectsQuery(
	 *     collection,
	 *     idsToDelete,
	 *     operationContext
	 * );
	 * @endcode
	 * 
	 * @note Many implementations use soft delete (UPDATE State = 'deleted')
	 * @see CreateRestoreObjectsQuery(), CreateDeleteObjectSetQuery()
	 */
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const = 0;

	/**
	 * @brief Creates SQL query for deleting objects matching filter criteria
	 * 
	 * Generates DELETE or UPDATE query that removes objects matching the specified
	 * filter parameters, without needing to know specific object IDs.
	 * 
	 * @param collection The collection containing the objects
	 * @param paramsPtr Filter parameters defining which objects to delete
	 * @param operationContextPtr Context for audit trail
	 * @return SQL DELETE or UPDATE query string
	 * 
	 * @code{.cpp}
	 * // Delete all inactive users older than 1 year
	 * auto params = acf::CreateComponent<iprm::CParamsSetComp>();
	 * params->SetValue("Status", "inactive");
	 * params->SetValue("LastLogin", oneYearAgo, iprm::CT_LESS_THAN);
	 * QByteArray deleteQuery = delegate->CreateDeleteObjectSetQuery(
	 *     collection, params.get(), operationContext
	 * );
	 * @endcode
	 * 
	 * @warning Be careful with filters - incorrect criteria could delete wrong objects
	 * @see CreateDeleteObjectsQuery()
	 */
	virtual QByteArray CreateDeleteObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	/**
	 * @brief Creates SQL query for restoring soft-deleted objects
	 * 
	 * Generates UPDATE query to restore objects that were previously soft-deleted,
	 * typically by changing State column back to active.
	 * 
	 * @param collection The collection containing the objects
	 * @param objectIds List of object IDs to restore
	 * @param operationContextPtr Context for audit trail
	 * @return SQL UPDATE query string
	 * 
	 * @note Only applicable if soft delete is used
	 * @see CreateDeleteObjectsQuery()
	 */
	virtual QByteArray CreateRestoreObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	/**
	 * @brief Creates SQL query for restoring objects matching filter criteria
	 * 
	 * Generates UPDATE query to restore soft-deleted objects that match the
	 * specified filter parameters.
	 * 
	 * @param collection The collection containing the objects
	 * @param paramsPtr Filter parameters defining which objects to restore
	 * @param operationContextPtr Context for audit trail
	 * @return SQL UPDATE query string
	 * 
	 * @see CreateRestoreObjectsQuery(), CreateDeleteObjectSetQuery()
	 */
	virtual QByteArray CreateRestoreObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	/**
	 * @brief Creates SQL query for updating an existing object
	 * 
	 * Generates UPDATE query to modify object data, metadata, and timestamps.
	 * 
	 * @param collection The collection containing the object
	 * @param objectId ID of the object to update
	 * @param object New object data
	 * @param operationContextPtr Context for audit trail
	 * @param useExternDelegate Whether to use external delegate for serialization (advanced)
	 * @return SQL UPDATE query string
	 * 
	 * @code{.cpp}
	 * user->SetEmail("newemail@example.com");
	 * QByteArray updateQuery = delegate->CreateUpdateObjectQuery(
	 *     collection,
	 *     userId,
	 *     *user,
	 *     operationContext
	 * );
	 * @endcode
	 * 
	 * @see CreateNewObjectQuery()
	 */
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const = 0;

	/**
	 * @brief Creates SQL query for renaming an object
	 * 
	 * Generates UPDATE query to change only the Name field of an object,
	 * without modifying other data.
	 * 
	 * @param collection The collection containing the object
	 * @param objectId ID of the object to rename
	 * @param newObjectName New name for the object
	 * @param operationContextPtr Context for audit trail
	 * @return SQL UPDATE query string
	 * 
	 * @note More efficient than full update when only name changes
	 */
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName,
				const imtbase::IOperationContext* operationContextPtr) const = 0;

	/**
	 * @brief Creates SQL query for updating object description
	 * 
	 * Generates UPDATE query to change only the Description field of an object.
	 * 
	 * @param collection The collection containing the object
	 * @param objectId ID of the object to update
	 * @param description New description text
	 * @param operationContextPtr Context for audit trail
	 * @return SQL UPDATE query string
	 */
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr) const = 0;

	/**
	 * @brief Creates SQL query for clearing all objects from collection
	 * 
	 * Generates TRUNCATE or DELETE query to remove all objects from the collection's table.
	 * 
	 * @param collection The collection to reset
	 * @return SQL TRUNCATE or DELETE query string
	 * 
	 * @warning This is destructive and typically cannot be undone
	 * @warning May not work if foreign key constraints exist
	 */
	virtual QByteArray CreateResetQuery(const imtbase::IObjectCollection& collection) const = 0;

	/**
	 * @brief Creates SQL query for updating object data metadata
	 * 
	 * Generates UPDATE query to modify the DataMetaInfo JSON field without
	 * changing the object's core data.
	 * 
	 * @param collection The collection containing the object
	 * @param objectId ID of the object
	 * @param dataMetaInfoPtr New metadata to store
	 * @return SQL UPDATE query string
	 * 
	 * @note Metadata typically includes custom fields, tags, dependent references
	 * @see CreateCollectionItemMetaInfoQuery()
	 */
	virtual QByteArray CreateDataMetaInfoQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr) const = 0;

	/**
	 * @brief Creates SQL query for updating collection-related metadata
	 * 
	 * Generates UPDATE query to modify metadata specific to the object's
	 * membership in this collection (distinct from the object's inherent metadata).
	 * 
	 * @param collection The collection containing the object
	 * @param objectId ID of the object
	 * @param collectionItemMetaInfoPtr Collection-specific metadata
	 * @return SQL UPDATE query string
	 * 
	 * @see CreateDataMetaInfoQuery()
	 */
	virtual QByteArray CreateCollectionItemMetaInfoQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr) const = 0;

	/**
	 * @brief Creates SQL query for finding objects by metadata field value
	 * 
	 * Generates SELECT query that searches within JSON metadata fields for
	 * specific values, enabling queries on dynamic metadata.
	 * 
	 * @param metaInfoId JSON field path within metadata (e.g., "tags[0]", "customField")
	 * @param metaInfoValue Value to search for
	 * @return SQL SELECT query string using JSON operators
	 * 
	 * @code{.cpp}
	 * // Find objects with specific tag
	 * QByteArray query = delegate->GetSelectionByMetaInfoQuery("tags", "important");
	 * @endcode
	 * 
	 * @note Uses database-specific JSON operators (PostgreSQL JSONB or SQLite JSON functions)
	 * @see ISqlJsonXPathExtractor
	 */
	virtual QByteArray GetSelectionByMetaInfoQuery(const QByteArray& metaInfoId, const QVariant& metaInfoValue) const = 0;
};


} // namespace imtdb


