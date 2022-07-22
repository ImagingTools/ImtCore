#pragma once


// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace iprm
{
	class IOptionsList;
}


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
		Get the list of the available object types.
	*/
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const = 0;

	/**
		Get the type-ID of the object.
	*/
	virtual Id GetObjectTypeId(const Id& objectId) const = 0;

	/**
		Get meta-information of the data object.
		In difference to GetCollectionItemMetaInfo, this method get the meta-informations related to the data itself.
		\param objectId			ID of the object in the collection.
		\return \c true if the operation was successful, and \c false if no information could be provided.
	*/
	virtual MetaInfoPtr GetDataMetaInfo(const Id& objectId) const = 0;
};


} // namespace imtbase


