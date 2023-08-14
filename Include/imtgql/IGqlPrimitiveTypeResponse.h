#pragma once


// ImtCore includes
#include <imtgql/IGqlResponse.h>


namespace imtgql
{


class IGqlPrimitiveTypeResponse: virtual public IGqlResponse
{
public:
	virtual bool GetValue(QVariant& out) const = 0;
};


} // namespace imtgql


