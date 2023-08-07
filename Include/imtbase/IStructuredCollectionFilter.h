#pragma once


// ImtCore includes
#include <imtbase/ICollectionFilter.h>


namespace imtbase
{


class IStructuredCollectionFilter: virtual public ICollectionFilter
{
	enum FilterTypeFlags
	{
		FTF_NODE = 1,
		FTF_OBJECT = 2
	};

	virtual int GetFilterTypeFlags() = 0;
	virtual void SetFilterTypeFlags(int flags) = 0;
};


} // namespace imtbase


