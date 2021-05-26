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
	CSubscriberBase(QObject& socket, const IRequestHandler& requestHandler,const IProtocolEngine& engine);

	// reimplemented (ISubscriber)
	virtual SubscriberState GetState() const override;
	virtual void SetState(const SubscriberState& subscriberState) override;
	virtual QByteArray GetTokenId() const override;
	virtual QByteArray GetSubscriberId() const override;
	virtual const IRequestHandler &GetRequestHandler() const override;

	// reimplemented (INetworkObject)
	virtual const IProtocolEngine& GetProtocolEngine() const override;
	virtual QObject& GetSocketObject() const override;

private:
	SubscriberState m_state;
	const IProtocolEngine& m_engine;
	const IRequestHandler& m_requestHandler;
	QObject& m_socket;
};


} // namespace imtrest


