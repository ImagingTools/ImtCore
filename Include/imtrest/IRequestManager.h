#pragma once


// ACF includes
#include <imtrest/INetworkObject.h>
#include <imtrest/IRequest.h>
#include <imtrest/ISender.h>


namespace imtrest
{


/**
	Common interface for a request manager object.
*/
class IRequestManager: virtual public istd::IPolymorphic
{
public:
	virtual const IRequest* GetRequest(const QByteArray& requestId) const = 0;
	/**
		Get sender instance using for sending responses to the clients.
	*/
	virtual const ISender* GetSender(const QByteArray& requestId) const = 0;
};


} // namespace imtrest


