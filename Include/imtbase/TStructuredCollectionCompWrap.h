#pragma once


// ImtCore includes
#include <imtbase/ICollectionStructure.h>
#include <imtbase/IStructuredCollectionInserter.h>


namespace imtbase
{


template<class BaseCollection>
class TStructuredCollectionCompWrap:
			public BaseCollection,
			virtual public ICollectionStructure,
			virtual public IStructuredCollectionInserter
{
public:
	typedef BaseCollection BaseClass;

	I_BEGIN_COMPONENT(TStructuredCollectionCompWrap);
	I_END_COMPONENT;
};


} // namespace imtbase


