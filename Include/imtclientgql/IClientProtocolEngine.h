// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>
#include <imtbase/IUrlParam.h>


class QNetworkRequest;


namespace imtclientgql
{


/**
	Common interface for the protocol-related communication on the client side.
	This interface represents the transport layer in the communication.
*/
class IClientProtocolEngine: virtual public istd::IPolymorphic
{
public:
	/**
		Create network (transport) request based on the GraphQL-request.
	*/
	virtual QNetworkRequest* CreateNetworkRequest(const imtgql::IGqlRequest& request, imtbase::IUrlParam* = nullptr) const = 0;
};


} // namespace imtclientgql


