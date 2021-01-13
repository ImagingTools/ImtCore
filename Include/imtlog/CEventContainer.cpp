#include <imtlog/CEventContainer.h>


// ACF includes
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlog
{


const CTimeRange CEventContainer::GetMessagesTimeRange() const
{
	if (m_messages.isEmpty()){
		return CTimeRange();
	}

	return CTimeRange(
				m_messages.last()->GetInformationTimeStamp(),
				m_messages.first()->GetInformationTimeStamp());
}


const CTimeRange& CEventContainer::GetTimeRange() const
{
	return m_timeRange;
}


bool CEventContainer::SetTimeRange(const CTimeRange& timeRange)
{
	if (timeRange.IsClosed()){
		QMutexLocker locker(&m_mutex);
		m_timeRange = timeRange;

		return true;
	}

	return false;
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
	QDateTime timestamp = messagePtr->GetInformationTimeStamp();

	Q_ASSERT(m_timeRange.Contains(timestamp));

	BaseClass::AddMessage(messagePtr);
}


// reimplemented (iser::ISerializable)

bool CEventContainer::Serialize(iser::IArchive& archive)
{
	QMutexLocker locker(&m_mutex);

	QDateTime beginTime;
	QDateTime endTime;

	if (archive.IsStoring()){
		Q_ASSERT(m_timeRange.IsClosed());

		beginTime = m_timeRange.GetBeginTime();
		endTime = m_timeRange.GetEndTime();
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
		Q_ASSERT(beginTime.isValid() && endTime.isValid() && endTime >= beginTime);
		m_timeRange = CTimeRange(beginTime, endTime);
	}

	retVal = retVal && BaseClass::Serialize(archive);

	return retVal;
}


} // namespace imtlog


