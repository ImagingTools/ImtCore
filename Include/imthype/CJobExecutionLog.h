#pragma once


// ImtCore includes
#include <imthype/IJobExecutionLog.h>

// ACF includes
#include <ilog/CMessageContainer.h>


namespace imthype
{


/**
	Concrete implementation of IJobExecutionLog.
	
	Stores system-level job controller events and lifecycle tracking.
*/
class CJobExecutionLog: 
	virtual public IJobExecutionLog,
	public ilog::CMessageContainer
{
public:
	CJobExecutionLog();
	explicit CJobExecutionLog(const QByteArray& jobId);
	
	// reimplemented (IJobExecutionLog)
	virtual void LogExecutionEvent(
		ExecutionEventType eventType,
		const QString& message,
		const QDateTime& timestamp = QDateTime::currentDateTime()) override;
	virtual QList<ilog::IMessage> GetExecutionEvents(ExecutionEventType eventType) const override;
	
	// Additional helper methods
	QByteArray GetJobId() const { return m_jobId; }
	void SetJobId(const QByteArray& jobId) { m_jobId = jobId; }

private:
	QByteArray m_jobId;
	
	// Map event types to message indices for fast lookup
	QMultiHash<ExecutionEventType, int> m_eventTypeMap;
};


typedef istd::TSharedInterfacePtr<CJobExecutionLog> CJobExecutionLogSharedPtr;


} // namespace imthype


