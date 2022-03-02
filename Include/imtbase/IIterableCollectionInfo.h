#pragma once


// ACF includes
#include <imtbase/ICollectionInfoProvider.h>


namespace imtbase
{


/**
	\ingroup Collection
*/
class IIterableCollectionInfo: virtual public ICollectionInfoProvider
{
public:
	class Iterator
	{
		virtual bool HasNext() const = 0;
		virtual Id GetNext() const = 0;
	};


	virtual Iterator& GetIterator() const = 0;
};


} // namespace imtbase


