#pragma once


// ACF includes
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


/**
	Interface for getting information about a collection of objects.
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
	virtual Id GetObjectTypeId(const QByteArray& objectId) const = 0;
};


} // namespace imtbase


