#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtgql/IGqlContext.h>

namespace imtgql
{


/**
	Common interface for a GraphQL request.
*/
class IGqlRequest: virtual public iser::IObject
{
public:
	enum RequestType
	{
		/**
			Query request.
		*/
		RT_QUERY,

		/**
			Mutation request.
		*/
		RT_MUTATION,

		/**
			Subscription request.
		*/
		RT_SUBSCRIPTION
	};

	virtual QByteArray GetCommandId() const = 0;
	virtual RequestType GetRequestType() const = 0;
	virtual QByteArray GetQuery() const = 0;
	virtual imtgql::IGqlContext* GetGqlContext() const = 0;
};


} // namespace imtgql


