#pragma once


// ImtCore includes
#include <imthype/IJobStatus.h>


namespace imthype
{


/**
	Concrete implementation of IJobStatus interface.
	
	Stores job execution status and progress information separately from 
	the job configuration (IJobTicket).
*/
class CJobStatus: virtual public IJobStatus
{
public:
	CJobStatus();
	explicit CJobStatus(const QByteArray& jobId);

	// reimplemented (imthype::IJobStatus)
	virtual IJobQueueManager::ProcessingStatus GetProcessingStatus() const override;
	virtual void SetProcessingStatus(IJobQueueManager::ProcessingStatus status) override;
	virtual double GetProgress() const override;
	virtual void SetProgress(double progress) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// Helper method for debugging and diagnostics (not part of IJobStatus interface)
	QByteArray GetJobId() const { return m_jobId; }

private:
	QByteArray m_jobId;
	IJobQueueManager::ProcessingStatus m_processingStatus;
	double m_progress;
};


typedef istd::TSharedInterfacePtr<CJobStatus> CJobStatusSharedPtr;


} // namespace imthype


