// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TIFactory.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>
#include <imtbase/IOperationContext.h>


namespace imtbase
{


class IHierarchicalStructure;
class IRevisionController;
class ICollectionDataController;
class IObjectCollectionIterator;
class IHierarchicalStructure;


/**
	Common interface for a data object collection.
	\ingroup Collection
*/
class IObjectCollection: virtual public IObjectCollectionInfo
{
public:
	typedef istd::IChangeableSharedPtr DataPtr;

	static const QByteArray CN_OBJECT_DATA_CHANGED;
	typedef NotifierInfo ObjectDataChanged;

	enum ChangeFlags
	{
		CF_OBJECT_DATA_CHANGED = CF_LAST
	};

	/**
		Flags for describing operational constraints on the collection items or the collection itself.
	*/
	enum OperationalFlags
	{
		/**
			If active, the obejects can be renamed inside of the collection.
		*/
		OF_SUPPORT_RENAME = 1,

		/**
			If active, the objects can be inserted into the collection.
		*/
		OF_SUPPORT_INSERT = 2,

		/**
			If active, deleting of objects in the collection is possible.
		*/
		OF_SUPPORT_DELETE = 4,

		/**
			If active, editing of objects in the collection is possible.
		*/
		OF_SUPPORT_EDIT = 8,

		/**
			If active, using of objects in the collection is possible.
		*/
		OF_SUPPORT_USING = 16,

		/**
			If active, the collection supports access to the ordered sub-set of the collection elements.
		*/
		OF_SUPPORT_PAGINATION = 32,

		/**
			All options are enabled.
		*/
		OF_ALL = OF_SUPPORT_RENAME | OF_SUPPORT_INSERT | OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_USING | OF_SUPPORT_PAGINATION
	};

	I_DECLARE_FLAGS(OperationalFlags, OF_SUPPORT_RENAME, OF_SUPPORT_INSERT, OF_SUPPORT_DELETE, OF_SUPPORT_EDIT, OF_SUPPORT_USING, OF_SUPPORT_PAGINATION, OF_ALL);

	class IDataFactory
	{
	public:
		virtual istd::IChangeableUniquePtr CreateInstance(const QByteArray& keyId = QByteArray()) const = 0;
		virtual istd::IFactoryInfo::KeyList GetFactoryKeys() const = 0;
	};

	virtual IHierarchicalStructure* GetCollectionStructure() const;

	/**
		Get access to the revision controller of the collection, if available.
	*/
	virtual const IRevisionController* GetRevisionController() const = 0;

	/**
		Get access to the data controller of the collection, if available.
	*/
	virtual const ICollectionDataController* GetDataController() const = 0;

	/**
		Get binary flags describing the possible operations on the single object or on the whole collection.
		\param objectId	ID of the object for quering flags. If this parameter is empty, the flags for entire collection will be returned.
	*/
	virtual int GetOperationFlags(const Id& elementId = Id()) const = 0;

	/**
		Create a new object in the container.
		\param typeId							Type-ID of the object to be created.
		\param name								Name of the object in the collection.
		\param description						Descrpition of the object.
		\param defaultValuePtr					Optional instance used for the object data initialization.
		\param proposedElementId				Optional-defined ID of the object in the collection. If the value is non-empty inserted object become specified ID if possible.
		\param dataMetaInfoPtr					Optional-defined meta-information for the data content.
		\param elementMetaInfoPtr				Optional-defined meta-information for the collection element.
		\return If the operation was successful the method will return ID of the created data object in the collection or an empty ID otherwise.
	*/
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Remove element with the given ID.
	*/
	virtual bool RemoveElements(
				const Ids& elementIds,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Remove element with the given filter.
		\param selectionParamsPtr				The filter by which the selection will be made for deletion. If it is null, then everything will be deleted.
	*/
	virtual bool RemoveElementSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Restore objects with the given IDs.
	*/
	virtual bool RestoreObjects(
				const Ids& objectIds,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Restore objects with the given filter.
		\param selectionParamsPtr				The filter by which the selection will be made for restoring. If it is null, then everything will be restored.
	*/
	virtual bool RestoreObjectSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Get access to the object instance inside of collecton.
		\note This method should return a valid instance for an object only for objects that are permanent in the collection (fixed).
	*/
	virtual const istd::IChangeable* GetObjectPtr(const Id& objectId) const = 0;

	/**
		Get access to the object instance inside of collecton.
		\note This method should return a valid instance for an object only for objects that are permanent in the collection and have
		the given interface.
	*/
	template<typename Interface>
	const Interface* GetObjectPtr(const Id& objectId) const
	{
		return dynamic_cast<const Interface*>(GetObjectPtr(objectId));
	}

	/**
		Get object data instance for the entry with the given ID.
	*/
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const = 0;

	/**
		Set data for the entry with the given ID.
	*/
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Create a sub-collection (a subset) of the whole collection according to the given filtering/sorting parameters.
		\param offset				[optional] Index offset of the first element
		\param count				[optional] If positive, the number of elements should be returned.
		\param selectionParamsPtr	[optional] Additional parameters for filtering/ordering elements.
	*/
	virtual istd::TUniqueInterfacePtr<IObjectCollection> CreateSubCollection(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	/**
		Create a collection iterator according to the given filtering/sorting parameters.
		\param offset				[optional] Index offset of the first element
		\param count				[optional] If positive, the number of elements should be returned.
		\param selectionParamsPtr	[optional] Additional parameters for filtering/ordering elements.
	*/
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	/**
		This method will re-create all object-IDs.
		\note Possible references to the objects in this collection will be lost after this operation.
	*/
	virtual void ResetObjectIds();
};


// public inline methods

inline void IObjectCollection::ResetObjectIds()
{
	Q_ASSERT_X(false, "IObjectCollection::ResetObjectIds", "Method not implemented");
}


typedef istd::TUniqueInterfacePtr<IObjectCollection> IObjectCollectionUniquePtr;
typedef istd::TSharedInterfacePtr<IObjectCollection> IObjectCollectionSharedPtr;


} // namespace imtbase


