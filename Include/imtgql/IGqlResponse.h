#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


class IGqlResponse : virtual public istd::IChangeable
{
public:
	typedef QSharedPointer<IGqlRequest> GqlRequestPtr;

	virtual GqlRequestPtr GetOriginalRequest() const = 0;
	virtual QByteArray GetResponseData() const = 0;
};


} // namespace imtgql


