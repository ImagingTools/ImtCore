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


