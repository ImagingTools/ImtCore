#include <imthype/CProcessorLog.h>


// ACF includes
#include <istd/CChangeNotifier.h>


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
	if (m_processorId != processorId) {
		istd::CChangeNotifier changeNotifier(this);
		m_processorId = processorId;
	}
}


QByteArray CProcessorLog::GetJobId() const
{
	return m_jobId;
}


void CProcessorLog::SetJobId(const QByteArray& jobId)
{
	if (m_jobId != jobId) {
		istd::CChangeNotifier changeNotifier(this);
		m_jobId = jobId;
	}
}


} // namespace imthype


