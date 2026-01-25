#include <imthype/CJobExecutionMessage.h>


// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/CClassInfo.h>
#include <istd/CChangeNotifier.h>
#include <ilog/CMessageContainer.h>


namespace imthype
{


// public methods

CJobExecutionMessage::CJobExecutionMessage()
	:m_eventType(IJobExecutionLog::EET_JOB_CREATED)
{
}


CJobExecutionMessage::CJobExecutionMessage(
			IJobExecutionLog::ExecutionEventType eventType,
			istd::IInformationProvider::InformationCategory category,
			const QString& text,
			const QString& source,
			int flags,
			const QDateTime* timeStampPtr)
	:BaseClass(category, MI_JOB_EXECUTION_EVENT, text, source, flags, timeStampPtr),
	m_eventType(eventType)
{
}


IJobExecutionLog::ExecutionEventType CJobExecutionMessage::GetEventType() const
{
	return m_eventType;
}


// reimplemented (iser::ISerializable)

bool CJobExecutionMessage::Serialize(iser::IArchive& archive)
{
	bool retVal = BaseClass::Serialize(archive);

	static iser::CArchiveTag eventTypeTag("EventType", "Execution event type", iser::CArchiveTag::TT_LEAF);

	retVal = retVal && archive.BeginTag(eventTypeTag);
	int eventTypeInt = static_cast<int>(m_eventType);
	retVal = retVal && archive.Process(eventTypeInt);
	if (archive.IsReading())
		m_eventType = static_cast<IJobExecutionLog::ExecutionEventType>(eventTypeInt);
	retVal = retVal && archive.EndTag(eventTypeTag);

	return retVal;
}


// reimplemented (iser::IObject)

QByteArray CJobExecutionMessage::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (istd::IChangeable)

bool CJobExecutionMessage::CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode /*mode*/)
{
	const CJobExecutionMessage* sourcePtr = dynamic_cast<const CJobExecutionMessage*>(&object);
	if (sourcePtr != nullptr){
		m_eventType = sourcePtr->m_eventType;
		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CJobExecutionMessage::CloneMe(istd::IChangeable::CompatibilityMode /*mode*/) const
{
	return new CJobExecutionMessage(
				m_eventType,
				m_category,
				m_text,
				m_source,
				m_flags,
				&m_timeStamp);
}


// public static methods

QByteArray CJobExecutionMessage::GetTypeId()
{
	return istd::CClassInfo::GetName<CJobExecutionMessage>();
}


I_REGISTER_MESSAGE_TYPE(CJobExecutionMessage, CJobExecutionMessage::GetTypeId());


} // namespace imthype


