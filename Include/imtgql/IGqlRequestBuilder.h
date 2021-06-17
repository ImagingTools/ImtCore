#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


class IGqlRequestBuilder: virtual public istd::IPolymorphic
{
public:
	virtual const IGqlRequest* CreateRequest(const QByteArray& commandId) const = 0;
};


} // namespace imtgql


