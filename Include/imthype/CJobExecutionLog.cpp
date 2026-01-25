#include <imthype/CJobExecutionLog.h>


// ACF includes
#include <ilog/CMessage.h>


namespace imthype
{


CJobExecutionLog::CJobExecutionLog()
{
}


CJobExecutionLog::CJobExecutionLog(const QByteArray& jobId)
	:m_jobId(jobId)
{
}


void CJobExecutionLog::LogExecutionEvent(
	ExecutionEventType eventType,
	const QString& message,
	const QDateTime& timestamp)
{
	// Create a log message with appropriate severity based on event type
	ilog::IMessage::MessageSeverity severity = ilog::IMessage::MS_INFO;
	
	switch (eventType) {
		case EET_JOB_FAILED:
			severity = ilog::IMessage::MS_ERROR;
			break;
		case EET_JOB_CANCELLED:
			severity = ilog::IMessage::MS_WARNING;
			break;
		case EET_JOB_COMPLETED:
			severity = ilog::IMessage::MS_SUCCESS;
			break;
		default:
			severity = ilog::IMessage::MS_INFO;
			break;
	}
	
	// Create and add the message
	ilog::CMessage logMessage(severity, message, timestamp);
	int messageIndex = GetMessagesCount();
	AddMessage(logMessage);
	
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


} // namespace imthype


