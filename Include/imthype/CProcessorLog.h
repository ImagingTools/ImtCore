#pragma once


// ImtCore includes
#include <imthype/IProcessorLog.h>

// ACF includes
#include <ilog/CMessageContainer.h>


namespace imthype
{


/**
	Concrete implementation of IProcessorLog.
	
	Stores worker/processor output and processing messages.
*/
class CProcessorLog: 
	virtual public IProcessorLog,
	public ilog::CMessageContainer
{
public:
	CProcessorLog();
	explicit CProcessorLog(const QByteArray& jobId, const QByteArray& processorId = QByteArray());
	
	// reimplemented (IProcessorLog)
	virtual QByteArray GetProcessorId() const override;
	virtual void SetProcessorId(const QByteArray& processorId) override;
	virtual QByteArray GetJobId() const override;
	virtual void SetJobId(const QByteArray& jobId) override;

private:
	QByteArray m_jobId;
	QByteArray m_processorId;
};


typedef istd::TSharedInterfacePtr<CProcessorLog> CProcessorLogSharedPtr;


} // namespace imthype


