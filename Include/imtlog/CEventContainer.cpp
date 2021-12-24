#include <imtlog/CEventContainer.h>


namespace imtlog
{


int CEventContainer::GetMessagesCount() const
{
	QMutexLocker locker(&m_mutex);
	return m_messages.count();
}


imtbase::CTimeRange CEventContainer::GetTimeRange() const
{
	QMutexLocker locker(&m_mutex);

	if (m_messages.isEmpty()){
		return imtbase::CTimeRange();
	}

	return imtbase::CTimeRange(
				m_messages.last()->GetInformationTimeStamp(),
				m_messages.first()->GetInformationTimeStamp());
}


// reimplemented (ilog::IMessageContainer)

int CEventContainer::GetWorstCategory() const
{
	QMutexLocker locker(&m_mutex);

	return BaseClass::GetWorstCategory();
}


ilog::IMessageContainer::Messages CEventContainer::GetMessages() const
{
	QMutexLocker locker(&m_mutex);

	return BaseClass::GetMessages();
}


void CEventContainer::ClearMessages()
{
	QMutexLocker locker(&m_mutex);

	BaseClass::ClearMessages();
}


// reimplemented (ilog::IMessageConsumer)

bool CEventContainer::IsMessageSupported(
			int messageCategory,
			int messageId,
			const istd::IInformationProvider* messagePtr) const
{
	QMutexLocker locker(&m_mutex);

	return BaseClass::IsMessageSupported(messageCategory, messageId, messagePtr);
}


void CEventContainer::AddMessage(const IMessageConsumer::MessagePtr& messagePtr)
{
	QMutexLocker locker(&m_mutex);

	BaseClass::AddMessage(messagePtr);
}


// reimplemented (iser::ISerializable)

bool CEventContainer::Serialize(iser::IArchive& archive)
{
	QMutexLocker locker(&m_mutex);

	return BaseClass::Serialize(archive);
}


} // namespace imtlog


