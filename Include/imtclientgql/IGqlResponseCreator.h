#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


class IGqlResponseCreator : virtual public istd::IPolymorphic
{
	virtual imtgql::IGqlResponse* CreateResponse(const imtgql::IGqlRequest& request) const = 0;
};


} // namespace imtclientgql


