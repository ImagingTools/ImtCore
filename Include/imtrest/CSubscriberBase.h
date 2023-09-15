#pragma once


// ImtCore includes
#include <imtrest/ISubscriber.h>


namespace imtrest
{


/**
	Common interface for a subscriber object.
*/
class CSubscriberBase: virtual public ISubscriber
{
public:
	CSubscriberBase(const IRequestServlet& requestHandler,const IProtocolEngine& engine);

	// reimplemented (ISubscriber)
	virtual SubscriberState GetState() const override;
	virtual void SetState(const SubscriberState& subscriberState) override;
	virtual QByteArray GetTokenId() const override;
	virtual QByteArray GetSubscriberId() const override;
	virtual const IRequestServlet &GetRequestHandler() const override;

	// reimplemented (INetworkObject)
	virtual const IProtocolEngine& GetProtocolEngine() const override;

private:
	SubscriberState m_state;
	const IProtocolEngine& m_engine;
	const IRequestServlet& m_requestHandler;
};


} // namespace imtrest


