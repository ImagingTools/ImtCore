// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <QtCore/qbytearray.h>
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
	virtual const IGqlContext* GetRequestContext() const = 0;
	/**
		The version of the exchange protocol used to determine the message version. X-Protocol-Version HTTP header \sa imtbase::s_protocolVersionHeaderId
	*/
	virtual QByteArray GetProtocolVersion() const = 0;
};


} // namespace imtgql


