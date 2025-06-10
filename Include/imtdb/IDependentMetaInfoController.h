#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtdb
{


class IDependentMetaInfoController: virtual public istd::IPolymorphic
{
public:
	struct DependentMetaInfo
	{
		QString objectId;
		QString dependentKey;
		QStringList metaInfoIds;
		QStringList metaInfoValues;
	};

	/**
		\brief Represents a plan for cleaning up meta fields in other entities after related objects are deleted.

		This structure defines how to remove or reset fields in JSON-based metadata
		(e.g., in an "Order" table) that reference entities which have been deleted
		(e.g., "Customer" records).

		It is typically used to ensure data consistency after deletion of related objects,
		by identifying which fields (e.g., "CustomerName") should be cleared if their
		corresponding reference (e.g., "CustomerId") is no longer valid.
	*/
	struct MetaFieldCleanupPlan
	{
		/**
			\brief A list of IDs of deleted objects (e.g., Customer IDs).

			These IDs are used to find and update any referencing metadata fields
			in dependent entities.
		*/
		imtbase::ICollectionInfo::Ids objectIds;

		/**
			\brief The key name in the metadata that holds the reference to the deleted object.

			For example, if the JSONB field contains {"CustomerId": "abc123"}, then
			dependentKey is "CustomerId".
		*/
		QString dependentKey;

		/**
			\brief A list of keys in the metadata that should be cleared if the referenced object is deleted.

			These are usually fields like "CustomerName" or other descriptive values
			derived from the referenced object.
		*/
		QStringList metaInfoIds;
	};

	virtual bool UpdateDependentMetaInfo(const DependentMetaInfo& metaInfo) const = 0;
	virtual bool ClearDependentMetaInfo(const MetaFieldCleanupPlan& metaInfo) const = 0;
};


} // namespace imtdb


