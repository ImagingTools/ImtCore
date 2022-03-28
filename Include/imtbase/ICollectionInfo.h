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
		Get number of elements in the collection
	*/
	virtual int GetElementsCount() const = 0;

	/**
		Get IDs of the elements inside the collection.
		\param offset	Index offset of the first element
		\param count	If positive, the number of elements should be returned.
		\param selectionParamsPtr	 [optional] Additional parameters for filtering/ordering elements.
	*/
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
};


} // namespace imtbase


