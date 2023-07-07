#pragma once


// ACF includes
#include <istd/TIFactory.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>
#include <imtbase/IOperationContext.h>


namespace imtbase
{


class ICollectionStructure;
class IRevisionController;
class ICollectionDataController;
class IObjectCollectionIterator;


/**
	Common interface for a data object collection.
	\ingroup Collection
*/
class IObjectCollection: virtual public IObjectCollectionInfo
{
public:
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

	class DataPtr
	{
	public:
		typedef std::shared_ptr<istd::IPolymorphic> RootObjectPtr;
		typedef std::function<istd::IChangeable*()> ExtractInterfaceFunc;

		DataPtr() : m_objectPtr(nullptr) {}

		DataPtr(const istd::IChangeable* ptr) : m_objectPtr(ptr) {}// pure pointer

		DataPtr(const RootObjectPtr& rootObjPtr, const ExtractInterfaceFunc& extractInterface)
			: m_rootPtr(rootObjPtr),
			m_objectPtr(extractInterface())
		{
		}

		bool IsValid() const { return m_objectPtr != nullptr; }
		const istd::IChangeable* GetPtr() const { return m_objectPtr; }
		const istd::IChangeable* operator->() const { return m_objectPtr; }
		const istd::IChangeable& operator*() const { return *m_objectPtr; }

		istd::IChangeable* GetPtr() { return const_cast<istd::IChangeable*>(m_objectPtr); }
		istd::IChangeable& operator*() { return *const_cast<istd::IChangeable*>(m_objectPtr); }
		istd::IChangeable* operator->() { return const_cast<istd::IChangeable*>(m_objectPtr); }

	private:
		RootObjectPtr m_rootPtr;
		const istd::IChangeable* m_objectPtr;
	};

	class IDataFactory
	{
	public:
		virtual DataPtr CreateInstance(const QByteArray& keyId = QByteArray()) const = 0;
		virtual istd::IFactoryInfo::KeyList GetFactoryKeys() const = 0;
	};

	virtual ICollectionStructure* GetCollectionStructure() const;

	/**
		Get access to the revision controller of the collection, if available.
	*/
	virtual const IRevisionController* GetRevisionController() const = 0;

	/**
		Get access to the revision controller of the collection, if available.
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
				DataPtr defaultValuePtr = DataPtr(),
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Remove element with the given ID.
	*/
	virtual bool RemoveElement(
				const Id& elementId,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Get access to the object instance inside of collecton.
		\note This method should return a valid instance for an object only for objects that are permanent in the collection (fixed).
	*/
	virtual const istd::IChangeable* GetObjectPtr(const Id& objectId) const = 0;

	/**
		Get object data instance for the entry with the given ID.
	*/
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr) const = 0;

	/**
		Set data for the entry with the given ID.
	*/
	virtual bool SetObjectData(
				const Id& objectId, 
				const istd::IChangeable& object, 
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Create sub collection of the whole collection according to the given filtering/sorting parameters.
		\param offset				[optional] Index offset of the first element
		\param count				[optional] If positive, the number of elements should be returned.
		\param selectionParamsPtr	[optional] Additional parameters for filtering/ordering elements.
		\param parentId				[optional] Parent element ID. Entire filtering operation applies to the childs of this element only.
	*/
	virtual imtbase::IObjectCollection* CreateSubCollection(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
};


} // namespace imtbase


