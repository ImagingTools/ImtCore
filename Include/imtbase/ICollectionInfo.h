#pragma once


// ACF includes
#include <iprm/IParamsSet.h>

// ACF includes
#include <imtbase/ICollectionInfoProvider.h>


namespace imtbase
{


/**
	Interface for non-iterable colllection info.
	\ingroup Collection
*/
class ICollectionInfo: virtual public ICollectionInfoProvider
{
public:
	/**
		Get all IDs of the elements inside the collection.
		\param selectionParamsPtr
	*/
	virtual Ids GetElementIds(const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
};


} // namespace imtbase


