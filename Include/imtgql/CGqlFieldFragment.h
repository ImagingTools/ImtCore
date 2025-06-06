#pragma once


// ImtCore includes
#include <imtgql/CGqlFieldObject.h>


namespace imtgql
{


class CGqlFieldFragment: public CGqlFieldObject
{
public:
	typedef CGqlFieldObject BaseClass;

	CGqlFieldFragment()
		:BaseClass()
	{
	}
};


} // namespace imtgql


