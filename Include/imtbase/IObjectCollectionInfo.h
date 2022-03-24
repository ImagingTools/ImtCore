#pragma once


// ACF includes
#include <iprm/IOptionsList.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


/**
	Interface for getting information about a collection of data objects.
	\ingroup Collection
*/
class IObjectCollectionInfo: virtual public ICollectionInfo
{
public:

	/**
		Type of the meta-informations supported by the collection.
	*/
	enum MetaInfoType
	{
		/**
			Name of the user who has added the element.
		*/
		MIT_INSERTION_USER = idoc::IDocumentMetaInfo::MIT_USER + 10000,

		/**
			Timestamp of the element inserting into the collection.
		*/
		MIT_INSERTION_TIME,

		/**
			Name of the user who has modified the element.
		*/
		MIT_LAST_OPERATION_USER,

		/**
			Timestamp of the last operation on the element in the collection.
		*/
		MIT_LAST_OPERATION_TIME,

		/**
			Tumbnail for the item preview.
		*/
		MIT_PREVIEW_THUMBNAIL,

		/**
			Current element revision.
		*/
		MIT_REVISION = idoc::IDocumentMetaInfo::MIT_USER + 20000
	};

	/**
		Get meta-information of the item in the collection.
		\param objectId			ID of the object in the collection.
		\param metaInfo			Meta-info to be filled by this method.
		\return \c true if the operation was successful, and \c false if no information could be provided.
	*/
	virtual bool GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const = 0;

	/**
		Get the list of the available object types.
	*/
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const = 0;

	/**
		Get the type-ID of the object.
	*/
	virtual Id GetObjectTypeId(const QByteArray& objectId) const = 0;
};


} // namespace imtbase


