#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


/**
	\todo Rename it to IGqlContextProvider and replace CGqlRequest by IGqlRequest in GetRequestContext
*/
class IGqlContextController: virtual public istd::IPolymorphic
{
public:
	virtual imtgql::IGqlContext* GetRequestContext(const imtgql::CGqlRequest& gqlRequest, const QByteArray& token) const = 0;
};


} // namespace imtgql


