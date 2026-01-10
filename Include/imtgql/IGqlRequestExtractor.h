#pragma once


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlRequestExtractor: virtual public istd::IPolymorphic
{
public:

	/**
		Create object from the GraphQL
	*/
	virtual istd::IChangeableUniquePtr ExtractObject(const CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const = 0;
};


} // namespace imtgql


