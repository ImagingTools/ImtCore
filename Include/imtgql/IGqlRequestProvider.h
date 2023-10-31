#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


class IGqlRequestProvider: virtual public istd::IPolymorphic
{
public:
	virtual const IGqlRequest* GetGqlRequest() const = 0;
};


} // namespace imtgql


