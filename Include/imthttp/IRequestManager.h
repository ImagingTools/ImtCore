#pragma once


// ACF includes
#include <imthttp/ISender.h>


namespace imthttp
{


/**
	Common interface for a request manager object.
*/
class IRequestManager: virtual public istd::IPolymorphic
{
public:
	/**
		Get sender instance using for sending responses to the clients.
	*/
	virtual const ISender* GetSender(const QByteArray& requestId) const = 0;
};


} // namespace imthttp


