#include <imtlog/CMessageHistoryContainer.h>


// ACF includes
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlog
{


const QDateTime& CMessageHistoryContainer::GetBeginTime()
{
	QMutexLocker locker(&m_mutex);
	return m_beginTime;
}


bool CMessageHistoryContainer::SetBeginTime(const QDateTime& time)
{
	if (time.isValid()){
		QMutexLocker locker(&m_mutex);
		m_beginTime = time;

		return true;
	}

	return false;
}


const QDateTime& CMessageHistoryContainer::GetEndTime()
{
	QMutexLocker locker(&m_mutex);
	return m_endTime;
}


bool CMessageHistoryContainer::SetEndTime(const QDateTime& time)
{
	if (time.isValid()){
		QMutexLocker locker(&m_mutex);
		m_endTime = time;

		return true;
	}

	return false;
}


CTimeRange CMessageHistoryContainer::GetTimeRange() const
{
	return CTimeRange(m_beginTime, m_endTime);
}


bool CMessageHistoryContainer::SetTimeRange(const CTimeRange& timeRange)
{
	QDateTime beginTime = timeRange.GetBeginTime();
	QDateTime endTime = timeRange.GetEndTime();

	if (beginTime.isValid() && endTime.isValid()){
		QMutexLocker locker(&m_mutex);
		m_beginTime = beginTime;
		m_endTime = endTime;

		return true;
	}

	return false;
}


int CMessageHistoryContainer::GetMessagesCount()
{
	QMutexLocker locker(&m_mutex);
	return m_messageContainer.GetMessagesCount();
}


int CMessageHistoryContainer::GetWorstCategory() const
{
	QMutexLocker locker(&m_mutex);
	return m_messageContainer.GetWorstCategory();
}


imtlog::CMessageHistoryContainer::Messages CMessageHistoryContainer::GetMessages() const
{
	QMutexLocker locker(&m_mutex);
	Q_ASSERT(m_messageContainer.GetMessagesCount() == m_messageIds.count());

	ilog::IMessageContainer::Messages container = m_messageContainer.GetMessages();

	IMessageHistoryConsumer::Messages messages;
	messages.reserve(container.count());
	for (int i = container.count() - 1; i >= 0; i--){
		IMessageHistoryConsumer::MessagePtr messagePtr =
					istd::TSmartPtr<IMessageHistoryConsumer::Message>(new IMessageHistoryConsumer::Message);
		messagePtr->id = m_messageIds[i];
		messagePtr->messagePtr = container[i];
		messages.append(messagePtr);
	}

	return messages;
}


void CMessageHistoryContainer::ClearMessages()
{
	QMutexLocker locker(&m_mutex);
	m_messageContainer.ClearMessages();
	m_messageIds.clear();
}


// reimplemented (imtlog::IMessageHistoryConsumer)

bool CMessageHistoryContainer::IsMessageSupported(const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CMessageHistoryContainer::AddMessage(const MessagePtr& messagePtr)
{
	QMutexLocker locker(&m_mutex);
	m_messageContainer.AddMessage(messagePtr->messagePtr);
	m_messageIds.prepend(messagePtr->id);
}


// reimplemented (iser::ISerializable)

bool CMessageHistoryContainer::Serialize(iser::IArchive& archive)
{
	Q_ASSERT(m_messageContainer.GetMessagesCount() == m_messageIds.count());

	QMutexLocker locker(&m_mutex);

	int messageCount = m_messageContainer.GetMessagesCount();
	int idsCount = m_messageIds.count();

	QDateTime beginTime;
	QDateTime endTime;

	if (archive.IsStoring()){
		beginTime = m_beginTime;
		endTime = m_endTime;
	}
	else{
		messageCount = 0;
		idsCount = 0;
		m_messageContainer.ClearMessages();
		m_messageIds.clear();
	}

	bool retVal = true;

	static iser::CArchiveTag beginTimeTag("BeginTime", "Container begin time", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag endTimeTag("EndTime", "Container end time", iser::CArchiveTag::TT_LEAF);

	retVal = retVal && archive.BeginTag(beginTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, beginTime);
	retVal = retVal && archive.EndTag(beginTimeTag);

	retVal = retVal && archive.BeginTag(endTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, endTime);
	retVal = retVal && archive.EndTag(endTimeTag);

	if (!archive.IsStoring()){
		m_beginTime = beginTime;
		m_endTime = endTime;
	}

	retVal = retVal && m_messageContainer.Serialize(archive);

	static iser::CArchiveTag idsTag("MessageIds", "", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag idTag("Id", "", iser::CArchiveTag::TT_LEAF);

	retVal = retVal && archive.BeginMultiTag(idsTag, idTag, idsCount);

	uint64_t id;

	if (archive.IsStoring()){
		for (int i = 0; i < m_messageIds.count(); i++){
			retVal = retVal && archive.BeginTag(idTag);
			retVal = retVal && archive.Process(m_messageIds[i]);
			retVal = retVal && archive.EndTag(idTag);
		}
	}
	else{
		m_messageIds.reserve(idsCount);
		for (int i = 0; i < idsCount; i++){
			retVal = retVal && archive.BeginTag(idTag);
			retVal = retVal && archive.Process(id);
			retVal = retVal && archive.EndTag(idTag);
			m_messageIds.append(id);
		}
	}

	retVal = retVal && archive.EndTag(idsTag);

	Q_ASSERT(m_messageContainer.GetMessagesCount() == m_messageIds.count());

	return retVal;
}


} // namespace imtlog


