#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlContextCreator: virtual public istd::IPolymorphic
{
public:
	virtual IGqlContextUniquePtr CreateGqlContext(
				const QByteArray& token,
				const QByteArray& productId,
				const QByteArray& userId,
				const IGqlContext::Headers& headers,
				QString& errorMessage) const = 0;
};


} // namespace imtgql


