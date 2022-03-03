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
		\param offset
		\param count
		\param selectionParamsPtr	 [optional] Additional parameters for filtering/ordering elements.
	*/
	virtual Ids GetElementIds(
				int offset = -1,
				int count = 0,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
};


} // namespace imtbase


