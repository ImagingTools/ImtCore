#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGrahpQlWrittable: virtual public istd::IPolymorphic
{
	virtual bool WriteToGraphQlObject(imtgql::CGqlObject& request) const = 0;
};


} // namespace imtqml


