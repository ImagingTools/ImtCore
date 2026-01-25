#include <imthype/CJobStatus.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imthype
{


// public methods

CJobStatus::CJobStatus()
	:m_processingStatus(IJobQueueManager::PS_NONE),
	m_progress(0.0)
{
}


CJobStatus::CJobStatus(const QByteArray& jobId)
	:m_jobId(jobId),
	m_processingStatus(IJobQueueManager::PS_NONE),
	m_progress(0.0)
{
}


// reimplemented (imthype::IJobStatus)

IJobQueueManager::ProcessingStatus CJobStatus::GetProcessingStatus() const
{
	return m_processingStatus;
}


void CJobStatus::SetProcessingStatus(IJobQueueManager::ProcessingStatus status)
{
	if (m_processingStatus != status){
		istd::CChangeNotifier changeNotifier(this);
		m_processingStatus = status;
	}
}


double CJobStatus::GetProgress() const
{
	return m_progress;
}


void CJobStatus::SetProgress(double progress)
{
	if (m_progress != progress){
		istd::CChangeNotifier changeNotifier(this);
		m_progress = progress;
	}
}


// reimplemented (istd::IChangeable)

bool CJobStatus::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	Q_UNUSED(mode);

	const CJobStatus* sourcePtr = dynamic_cast<const CJobStatus*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_jobId = sourcePtr->m_jobId;
		m_processingStatus = sourcePtr->m_processingStatus;
		m_progress = sourcePtr->m_progress;

		return true;
	}

	return false;
}


} // namespace imthype


