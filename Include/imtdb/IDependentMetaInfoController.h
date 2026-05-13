// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtdb
{


/**
 * @interface IDependentMetaInfoController
 * @brief Manages cascading metadata updates when related objects are deleted
 * 
 * IDependentMetaInfoController maintains referential integrity in JSON-based metadata
 * when referenced objects are deleted. It identifies and cleans up dependent metadata
 * fields that reference deleted objects, preventing stale references.
 * 
 * @section idmic_problem The Problem
 * 
 * Consider this scenario:
 * - **Customer** objects stored in `customers` table
 * - **Order** objects stored in `orders` table with metadata:
 *   ```json
 *   {
 *     "CustomerId": "cust-123",
 *     "CustomerName": "John Doe",
 *     "CustomerEmail": "john@example.com"
 *   }
 *   ```
 * - When Customer "cust-123" is deleted, the Order metadata has stale references
 * 
 * @section idmic_solution The Solution
 * 
 * The controller:
 * 1. Observes deletion events in source collections (Customers)
 * 2. Identifies dependent collections (Orders) with references
 * 3. Finds objects with metadata referencing deleted IDs
 * 4. Clears or nullifies the dependent metadata fields
 * 5. Updates timestamps and audit trails
 * 
 * @section idmic_workflow Cleanup Workflow
 * 
 * @verbatim
 * 1. Delete Customer "cust-123"
 *    ↓
 * 2. Controller identifies dependent metadata:
 *    - Table: orders
 *    - Key: CustomerId
 *    - Dependent fields: CustomerName, CustomerEmail
 *    ↓
 * 3. Find orders with CustomerId = "cust-123"
 *    ↓
 * 4. Update metadata:
 *    {
 *      "CustomerId": null,         // Clear reference
 *      "CustomerName": null,       // Clear dependent field
 *      "CustomerEmail": null       // Clear dependent field
 *    }
 *    ↓
 * 5. Update LastModified timestamp
 * @endverbatim
 * 
 * @section idmic_usage Usage Example
 * 
 * @code{.cpp}
 * auto controller = acf::CreateComponent<CDependentTableMetaInfoControllerComp>();
 * 
 * // Configure which fields depend on which references
 * controller->AddDependentMapping("orders", "CustomerId", 
 *                                  QStringList{"CustomerName", "CustomerEmail"});
 * 
 * // When deleting customers
 * imtbase::ICollectionInfo::Ids deletedCustomerIds = {"cust-123", "cust-456"};
 * 
 * MetaFieldCleanupPlan plan;
 * plan.objectIds = deletedCustomerIds;
 * plan.dependentKey = "CustomerId";
 * plan.metaInfoIds = QStringList{"CustomerName", "CustomerEmail"};
 * 
 * controller->ClearDependentMetaInfo(plan);
 * // All orders referencing deleted customers now have cleaned metadata
 * @endcode
 * 
 * @note Cleanup is typically automatic via Observer pattern
 * @note Works with JSON/JSONB metadata columns
 * @see CDependentTableMetaInfoControllerComp for implementation
 * @see CSqlDatabaseDocumentDelegateComp for integration
 */
class IDependentMetaInfoController: virtual public istd::IPolymorphic
{
public:
	/**
	 * @struct DependentMetaInfo
	 * @brief Information for updating dependent metadata fields
	 * 
	 * Used when updating metadata in dependent objects to reflect changes
	 * in referenced objects (e.g., updating CustomerName when Customer is renamed).
	 */
	struct DependentMetaInfo
	{
		/**
		 * @brief ID of the object whose metadata should be updated
		 * 
		 * For example, Order ID when updating customer reference in order metadata.
		 */
		QString objectId;
		
		/**
		 * @brief The metadata key that holds the reference to another object
		 * 
		 * For example, "CustomerId" in Order metadata referencing Customer.
		 */
		QString dependentKey;
		
		/**
		 * @brief List of metadata field names to update
		 * 
		 * For example, ["CustomerName", "CustomerEmail"] - fields derived
		 * from the referenced object.
		 */
		QStringList metaInfoIds;
		
		/**
		 * @brief New values for the metadata fields
		 * 
		 * Corresponding values for each field in metaInfoIds.
		 * For example, ["Updated Name", "newemail@example.com"].
		 */
		QStringList metaInfoValues;
	};

	/**
	 * @struct MetaFieldCleanupPlan
	 * @brief Plan for cleaning up metadata fields when referenced objects are deleted
	 * 
	 * This structure defines how to remove or reset fields in JSON-based metadata
	 * (e.g., in an "Order" table) that reference entities which have been deleted
	 * (e.g., "Customer" records).
	 * 
	 * It ensures data consistency after deletion of related objects by identifying
	 * which fields (e.g., "CustomerName") should be cleared if their corresponding
	 * reference (e.g., "CustomerId") is no longer valid.
	 */
	struct MetaFieldCleanupPlan
	{
		/**
		 * @brief List of IDs of deleted objects (e.g., Customer IDs)
		 * 
		 * These IDs are used to find and update any referencing metadata fields
		 * in dependent entities.
		 * 
		 * Example: ["cust-123", "cust-456"] for deleted customers.
		 */
		imtbase::ICollectionInfo::Ids objectIds;

		/**
		 * @brief The metadata key that holds the reference to deleted objects
		 * 
		 * For example, if the JSONB field contains `{"CustomerId": "abc123"}`,
		 * then dependentKey is `"CustomerId"`.
		 * 
		 * The controller will find all objects where this key matches any of
		 * the deleted objectIds.
		 */
		QString dependentKey;

		/**
		 * @brief List of metadata keys to clear when reference is deleted
		 * 
		 * These are usually fields like "CustomerName", "CustomerEmail", or other
		 * descriptive values derived from the referenced object.
		 * 
		 * When an object with ID matching dependentKey is deleted, these fields
		 * are set to null or removed from the metadata.
		 * 
		 * Example: ["CustomerName", "CustomerEmail", "CustomerPhone"]
		 */
		QStringList metaInfoIds;
	};

	/**
	 * @brief Updates dependent metadata fields to reflect changes in referenced objects
	 * 
	 * When a referenced object changes (e.g., Customer renamed), this method updates
	 * all dependent metadata that includes derived fields from that object.
	 * 
	 * @param metaInfo Describes which object's metadata to update and what values to set
	 * @return true if update succeeded, false on error
	 * 
	 * Example use case:
	 * - Customer "cust-123" renamed from "John Doe" to "John Smith"
	 * - Update all Order metadata where CustomerId = "cust-123"
	 * - Set CustomerName = "John Smith"
	 * 
	 * @code{.cpp}
	 * DependentMetaInfo update;
	 * update.objectId = "order-789";
	 * update.dependentKey = "CustomerId";
	 * update.metaInfoIds = QStringList{"CustomerName"};
	 * update.metaInfoValues = QStringList{"John Smith"};
	 * 
	 * controller->UpdateDependentMetaInfo(update);
	 * @endcode
	 * 
	 * @see ClearDependentMetaInfo()
	 */
	virtual bool UpdateDependentMetaInfo(const DependentMetaInfo& metaInfo) const = 0;
	
	/**
	 * @brief Clears metadata fields that reference deleted objects
	 * 
	 * When objects are deleted, this method finds all dependent objects that have
	 * metadata referencing the deleted IDs and clears the relevant fields.
	 * 
	 * @param metaInfo Describes which objects were deleted and what fields to clean
	 * @return true if cleanup succeeded, false on error
	 * 
	 * Process:
	 * 1. Find all objects where metadata[dependentKey] is in objectIds
	 * 2. For each found object:
	 *    - Set metadata[dependentKey] = null
	 *    - For each field in metaInfoIds: set metadata[field] = null
	 *    - Update LastModified timestamp
	 * 
	 * @code{.cpp}
	 * // Customers "cust-123" and "cust-456" were deleted
	 * MetaFieldCleanupPlan plan;
	 * plan.objectIds = {"cust-123", "cust-456"};
	 * plan.dependentKey = "CustomerId";
	 * plan.metaInfoIds = {"CustomerName", "CustomerEmail"};
	 * 
	 * controller->ClearDependentMetaInfo(plan);
	 * // All orders with CustomerId in deleted list now have cleared metadata
	 * @endcode
	 * 
	 * @note This maintains referential integrity in JSON metadata
	 * @note Typically called automatically when objects are deleted
	 * @see UpdateDependentMetaInfo()
	 */
	virtual bool ClearDependentMetaInfo(const MetaFieldCleanupPlan& metaInfo) const = 0;
};


} // namespace imtdb


