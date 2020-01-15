#pragma once


// ACF includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>


namespace imtbase
{


/**
	Common interface for a data object collection.
	\ingroup Collection
*/
class IObjectCollection: virtual public IObjectCollectionInfo
{
public:
	typedef istd::TSmartPtr<istd::IChangeable> DataPtr;
	typedef istd::TSmartPtr<idoc::IDocumentMetaInfo> MetaInfoPtr;

	/**
		Change notification flags.
	*/
	enum ChangeFlags
	{
		/**
			Item was added.
		*/
		CF_ADDED = 10000,

		/**
			Item was removed.
		*/
		CF_REMOVED,

		/**
			Data object was changed.
		*/
		CF_UPDATED
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
			All options are enabled.
		*/
		OF_ALL = -1
	};

	I_DECLARE_FLAGS(OperationalFlags, OF_SUPPORT_RENAME, OF_SUPPORT_INSERT, OF_SUPPORT_DELETE, OF_SUPPORT_EDIT, OF_SUPPORT_USING, OF_ALL);

	/**
		Get binary flags describing the possible operations on the single object or on the whole collection.
	*/
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const = 0;


	/**
		Get meta-information of the data object.
		In difference to GetCollectionItemMetaInfo, this method get the meta-informations related to the data itself.
		\param objectId			ID of the object in the collection.
		\param metaInfoPtr		Meta-info pointer to be created and filled by this method.
		\return \c true if the operation was successful, and \c false if no information could be provided.
	*/
	virtual bool GetDataMetaInfo(const QByteArray& objectId, MetaInfoPtr& metaInfoPtr) const = 0;

	/**
		Create a new object in the container.
		\param typeId			Type-ID of the object to be created.
		\param name				Name of the object in the collection.
		\param description		Descrpition of the object.
		\param defaultValuePtr	Optional instance used for the object data initialization.
		\return If the operation was successful the method will return ID of the created data object in the collection or an empty ID otherwise.
	*/
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr) = 0;

	/**
		Remove element with the given ID.
	*/
	virtual bool RemoveObject(const QByteArray& objectId) = 0;

	/**
		Get access to the object instance inside of collecton.
		\note This method should returns objects that are fixed in the collection.
	*/
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const = 0;

	/**
		Get object data instance for the entry with the given ID.
	*/
	virtual bool GetObjectData( const QByteArray& objectId, DataPtr& dataPtr) const = 0;

	/**
		Set data for the entry with the given ID.
	*/
	virtual bool SetObjectData( const QByteArray& objectId, const istd::IChangeable& object) = 0;

	/**
		Set name of the element with the given ID.
	*/
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) = 0;

	/**
		Set description of the element with the given ID.
	*/
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) = 0;

	/**
		Enable/Disable element with the given ID.
	*/
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) = 0;
};


} // namespace imtbase


