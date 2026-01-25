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


ilog::IMessageContainer::Messages CJobExecutionLog::GetExecutionEvents(ExecutionEventType eventType) const
{
	ilog::IMessageContainer::Messages events;
	
	// Get all message indices for this event type
	QList<int> indices = m_eventTypeMap.values(eventType);
	
	// Retrieve the actual messages as shared pointers
	for (int index : indices) {
		if (index < GetMessagesCount()) {
			events.append(GetMessagePtr(index));
		}
	}
	
	return events;
}


bool CJobExecutionLog::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	
	bool retVal = true;
	
	// Serialize the message container (parent class)
	static iser::CArchiveTag messagesTag("Messages", "Execution log messages", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(messagesTag);
	retVal = retVal && ilog::CMessageContainer::Serialize(archive);
	retVal = retVal && archive.EndTag(messagesTag);
	
	// Serialize the event type map
	int eventMapCount = m_eventTypeMap.size();
	
	static iser::CArchiveTag eventMapListTag("EventMapList", "Event type mapping list", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag eventMapEntryTag("EventMapEntry", "Event map entry", iser::CArchiveTag::TT_GROUP, &eventMapListTag);
	
	retVal = retVal && archive.BeginMultiTag(eventMapListTag, eventMapEntryTag, eventMapCount);
	
	if (archive.IsStoring()) {
		auto it = m_eventTypeMap.begin();
		for (int i = 0; i < eventMapCount; ++i, ++it) {
			retVal = retVal && archive.BeginTag(eventMapEntryTag);
			
			int eventType = static_cast<int>(it.key());
			int messageIndex = it.value();
			
			static iser::CArchiveTag eventTypeTag("EventType", "Event type", iser::CArchiveTag::TT_LEAF, &eventMapEntryTag);
			retVal = retVal && archive.BeginTag(eventTypeTag);
			retVal = retVal && archive.Process(eventType);
			retVal = retVal && archive.EndTag(eventTypeTag);
			
			static iser::CArchiveTag messageIndexTag("MessageIndex", "Message index", iser::CArchiveTag::TT_LEAF, &eventMapEntryTag);
			retVal = retVal && archive.BeginTag(messageIndexTag);
			retVal = retVal && archive.Process(messageIndex);
			retVal = retVal && archive.EndTag(messageIndexTag);
			
			retVal = retVal && archive.EndTag(eventMapEntryTag);
		}
	} else {
		m_eventTypeMap.clear();
		
		for (int i = 0; i < eventMapCount; ++i) {
			retVal = retVal && archive.BeginTag(eventMapEntryTag);
			
			int eventType = 0;
			int messageIndex = 0;
			
			static iser::CArchiveTag eventTypeTag("EventType", "Event type", iser::CArchiveTag::TT_LEAF, &eventMapEntryTag);
			retVal = retVal && archive.BeginTag(eventTypeTag);
			retVal = retVal && archive.Process(eventType);
			retVal = retVal && archive.EndTag(eventTypeTag);
			
			static iser::CArchiveTag messageIndexTag("MessageIndex", "Message index", iser::CArchiveTag::TT_LEAF, &eventMapEntryTag);
			retVal = retVal && archive.BeginTag(messageIndexTag);
			retVal = retVal && archive.Process(messageIndex);
			retVal = retVal && archive.EndTag(messageIndexTag);
			
			retVal = retVal && archive.EndTag(eventMapEntryTag);
			
			m_eventTypeMap.insert(static_cast<ExecutionEventType>(eventType), messageIndex);
		}
	}
	
	retVal = retVal && archive.EndTag(eventMapListTag);
	
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


