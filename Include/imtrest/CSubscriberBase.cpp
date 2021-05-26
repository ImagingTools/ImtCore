#include <imtrest/CSubscriberBase.h>


// ACF includes
#include <imtrest/INetworkObject.h>


namespace imtrest
{

CSubscriberBase::CSubscriberBase(QObject &socket, const IRequestHandler& requestHandler, const IProtocolEngine &engine)
	:m_engine(engine),
	m_requestHandler(requestHandler),
	m_socket(socket),
	m_state(SS_NON_AUTHORIZED)
{

}


ISubscriber::SubscriberState CSubscriberBase::GetState() const
{
	return m_state;
}

void CSubscriberBase::SetState(const SubscriberState &subscriberState)
{
	m_state = subscriberState;
}


QByteArray CSubscriberBase::GetTokenId() const
{
	return QByteArray();
}


QByteArray CSubscriberBase::GetSubscriberId() const
{
	return QByteArray();
}


const IRequestHandler& CSubscriberBase::GetRequestHandler() const
{
	return m_requestHandler;
}


const IProtocolEngine& CSubscriberBase::GetProtocolEngine() const
{
	return m_engine;
}


QObject& CSubscriberBase::GetSocketObject() const
{
	return m_socket;
}




} // namespace imtrest


