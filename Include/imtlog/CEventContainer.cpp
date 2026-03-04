// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlog/CEventContainer.h>


namespace imtlog
{


int CEventContainer::GetMessagesCount() const
{
	QMutexLocker locker(&m_mutex);
	return int(m_messages.size());
}


imtbase::CTimeRange CEventContainer::GetTimeRange() const
{
	QMutexLocker locker(&m_mutex);

	if (m_messages.empty()){
		return imtbase::CTimeRange();
	}

	return imtbase::CTimeRange(
				m_messages.back()->GetInformationTimeStamp(),
				m_messages.front()->GetInformationTimeStamp());
}


// reimplemented (ilog::IMessageContainer)

istd::IInformationProvider::InformationCategory CEventContainer::GetWorstCategory() const
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


