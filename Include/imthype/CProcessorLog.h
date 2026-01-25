#pragma once


// ImtCore includes
#include <imthype/IProcessorLog.h>

// ACF includes
#include <ilog/CMessageContainer.h>
#include <iser/ISerializable.h>


namespace imthype
{


/**
	Concrete implementation of IProcessorLog.
	
	Stores worker/processor output and processing messages.
*/
class CProcessorLog: 
	virtual public IProcessorLog,
	public ilog::CMessageContainer,
	virtual public iser::ISerializable
{
public:
	CProcessorLog();
	explicit CProcessorLog(const QByteArray& processorId);
	
	// reimplemented (IProcessorLog)
	virtual QByteArray GetProcessorId() const override;
	virtual void SetProcessorId(const QByteArray& processorId) override;
	
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;
	
	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_processorId;
};


typedef istd::TSharedInterfacePtr<CProcessorLog> CProcessorLogSharedPtr;


} // namespace imthype


