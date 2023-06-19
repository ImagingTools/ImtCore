#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlContextController: virtual public istd::IPolymorphic
{
public:
	virtual imtgql::IGqlContext* GetGqlContext(const imtgql::CGqlRequest& gqlRequest, const QByteArray& token) const = 0;
};


} // namespace imtgql


