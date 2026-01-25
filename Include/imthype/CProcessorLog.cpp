#include <imthype/CProcessorLog.h>


namespace imthype
{


CProcessorLog::CProcessorLog()
{
}


CProcessorLog::CProcessorLog(const QByteArray& jobId, const QByteArray& processorId)
	:m_jobId(jobId),
	m_processorId(processorId)
{
}


QByteArray CProcessorLog::GetProcessorId() const
{
	return m_processorId;
}


void CProcessorLog::SetProcessorId(const QByteArray& processorId)
{
	m_processorId = processorId;
}


QByteArray CProcessorLog::GetJobId() const
{
	return m_jobId;
}


void CProcessorLog::SetJobId(const QByteArray& jobId)
{
	m_jobId = jobId;
}


} // namespace imthype


