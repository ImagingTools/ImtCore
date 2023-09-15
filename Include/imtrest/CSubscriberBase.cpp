#include <imtrest/CSubscriberBase.h>


// ACF includes
#include <imtrest/INetworkObject.h>


namespace imtrest
{

// public methods

CSubscriberBase::CSubscriberBase(const IRequestServlet& requestHandler, const IProtocolEngine &engine)
			:m_state(SS_NON_AUTHORIZED),
			m_engine(engine),
			m_requestHandler(requestHandler)
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


} // namespace imtrest


