#include <imtrest/CSubscriberBase.h>


// ACF includes
#include <imtrest/INetworkObject.h>


namespace imtrest
{

// public methods

CSubscriberBase::CSubscriberBase(QObject &socket, const IRequestServlet& requestHandler, const IProtocolEngine &engine)
	:m_engine(engine),
	m_requestHandler(requestHandler),
	m_socket(socket),
	m_state(SS_NON_AUTHORIZED)
{
}


// reimplemented (ISubscriber)

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


const IRequestServlet& CSubscriberBase::GetRequestHandler() const
{
	return m_requestHandler;
}


// reimplemented (INetworkObject)

const IProtocolEngine& CSubscriberBase::GetProtocolEngine() const
{
	return m_engine;
}


QObject& CSubscriberBase::GetSocketObject() const
{
	return m_socket;
}


} // namespace imtrest


