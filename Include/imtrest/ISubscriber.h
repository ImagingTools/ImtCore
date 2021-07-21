#pragma once


// ACF includes
#include <imtrest/INetworkObject.h>


namespace imtrest
{

class IRequestServlet;

/**
	Common interface for a subscriber object.
*/
class ISubscriber: virtual public INetworkObject
{
public:
	enum SubscriberState
	{
		SS_NON_AUTHORIZED,
		SS_AUTHORIZED
	};

	virtual SubscriberState GetState() const = 0;
	virtual void SetState(const SubscriberState& subscriberState) = 0;
	virtual QByteArray GetTokenId() const = 0;
	virtual QByteArray GetSubscriberId() const = 0;
	virtual const IRequestServlet& GetRequestHandler() const = 0;
};


} // namespace imtrest


