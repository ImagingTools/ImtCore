#include <imthype/CJobExecutionLog.h>


// ACF includes
#include <ilog/CMessage.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imthype
{


CJobExecutionLog::CJobExecutionLog()
{
}


void CJobExecutionLog::LogExecutionEvent(
	ExecutionEventType eventType,
	const QString& message,
	const QDateTime& timestamp)
{
	istd::CChangeNotifier changeNotifier(this);
	
	// Create a log message with appropriate category based on event type
	istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::IC_INFO;
	
	switch (eventType) {
		case EET_JOB_FAILED:
			category = istd::IInformationProvider::IC_ERROR;
			break;
		case EET_JOB_CANCELLED:
			category = istd::IInformationProvider::IC_WARNING;
			break;
		case EET_JOB_COMPLETED:
			category = istd::IInformationProvider::IC_INFO;
			break;
		default:
			category = istd::IInformationProvider::IC_INFO;
			break;
	}
	
	// Create and add the message
	int messageIndex = GetMessagesCount();
	ilog::CMessage logMessage(category, 0, message, QString(), timestamp);
	ilog::CMessageContainer::AddMessage(logMessage);
	
	// Track the event type for fast lookup
	m_eventTypeMap.insert(eventType, messageIndex);
}


QList<ilog::IMessage> CJobExecutionLog::GetExecutionEvents(ExecutionEventType eventType) const
{
	QList<ilog::IMessage> events;
	
	// Get all message indices for this event type
	QList<int> indices = m_eventTypeMap.values(eventType);
	
	// Retrieve the actual messages
	for (int index : indices) {
		if (index < GetMessagesCount()) {
			events.append(GetMessage(index));
		}
	}
	
	return events;
}


bool CJobExecutionLog::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	
	bool retVal = true;
	
	// Serialize the message container
	static iser::CArchiveTag messagesTag("Messages", "Execution log messages");
	retVal = retVal && archive.BeginTag(messagesTag);
	retVal = retVal && ilog::CMessageContainer::Serialize(archive);
	retVal = retVal && archive.EndTag(messagesTag);
	
	// Serialize the event type map
	static iser::CArchiveTag eventMapTag("EventMap", "Event type mapping");
	retVal = retVal && archive.BeginTag(eventMapTag);
	
	if (archive.IsStoring()) {
		int count = m_eventTypeMap.size();
		retVal = retVal && archive.Process(count);
		
		for (auto it = m_eventTypeMap.begin(); it != m_eventTypeMap.end(); ++it) {
			int eventType = static_cast<int>(it.key());
			int messageIndex = it.value();
			retVal = retVal && archive.Process(eventType);
			retVal = retVal && archive.Process(messageIndex);
		}
	} else {
		m_eventTypeMap.clear();
		int count = 0;
		retVal = retVal && archive.Process(count);
		
		for (int i = 0; i < count; ++i) {
			int eventType = 0;
			int messageIndex = 0;
			retVal = retVal && archive.Process(eventType);
			retVal = retVal && archive.Process(messageIndex);
			m_eventTypeMap.insert(static_cast<ExecutionEventType>(eventType), messageIndex);
		}
	}
	
	retVal = retVal && archive.EndTag(eventMapTag);
	
	return retVal;
}


bool CJobExecutionLog::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CJobExecutionLog* sourcePtr = dynamic_cast<const CJobExecutionLog*>(&object);
	if (sourcePtr) {
		istd::CChangeNotifier changeNotifier(this);
		
		ilog::CMessageContainer::CopyFrom(*sourcePtr);
		m_eventTypeMap = sourcePtr->m_eventTypeMap;
		
		return true;
	}
	
	return false;
}


bool CJobExecutionLog::IsEqual(const istd::IChangeable& object) const
{
	const CJobExecutionLog* sourcePtr = dynamic_cast<const CJobExecutionLog*>(&object);
	if (sourcePtr) {
		bool retVal = ilog::CMessageContainer::IsEqual(*sourcePtr);
		retVal = retVal && (m_eventTypeMap == sourcePtr->m_eventTypeMap);
		
		return retVal;
	}
	
	return false;
}


istd::IChangeableUniquePtr CJobExecutionLog::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CJobExecutionLog);
	if (clonePtr->CopyFrom(*this, mode)) {
		return clonePtr;
	}
	
	return nullptr;
}


bool CJobExecutionLog::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);
	
	ilog::CMessageContainer::Clear();
	m_eventTypeMap.clear();
	
	return true;
}


} // namespace imthype


