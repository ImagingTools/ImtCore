#pragma once


// ACF includes
#include <iser/IObject.h>


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
	virtual QByteArrayList GetFields() const = 0;
	virtual QByteArrayList GetFieldArguments(const QByteArray& fieldId) const = 0;
};


} // namespace imtgql


