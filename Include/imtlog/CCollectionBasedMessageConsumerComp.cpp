#include <imtlog/CCollectionBasedMessageConsumerComp.h>


namespace imtlog
{

CCollectionBasedMessageConsumerComp::CCollectionBasedMessageConsumerComp()
	: m_isSystemStarted(false)
{

}


// reimplemented (ilog::IMessageConsumer)
void CCollectionBasedMessageConsumerComp::OnSystemStarted()
{
	BaseClass::OnSystemStarted();

	m_isSystemStarted = true;
}

// reimplemented (ilog::IMessageConsumer)
void CCollectionBasedMessageConsumerComp::WriteMessageToLog(const MessagePtr& messagePtr)
{
	if (m_isSystemStarted){
		if (!m_messageCollection.IsValid()){
			Q_ASSERT(false);

			return;
		}
		while (!m_startMessages.isEmpty()){
		const MessagePtr startMessage = m_startMessages.takeFirst();
			m_messageCollection->InsertNewObject("Message", "", "", startMessage.GetPtr());
		}

		m_messageCollection->InsertNewObject("Message", "", "", messagePtr.GetPtr());
	}
	else {
		m_startMessages.append(messagePtr);
	}
}


} // namespace imtlog


