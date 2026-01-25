#pragma once


// ACF includes
#include <ilog/IMessageContainer.h>


namespace imthype
{


/**
	Interface for processor/worker log.
	
	This log contains the actual processing output from the worker executing
	the job. It includes worker-generated messages, diagnostics, progress reports,
	and any output produced during job processing.
	This is distinct from the job execution log which tracks system-level
	controller events.
*/
class IProcessorLog: virtual public ilog::IMessageContainer
{
public:
	/**
		Get the worker/processor identifier that generated this log.
		\return Processor identifier
	*/
	virtual QByteArray GetProcessorId() const = 0;
	
	/**
		Set the worker/processor identifier.
		\param processorId Identifier of the processor
	*/
	virtual void SetProcessorId(const QByteArray& processorId) = 0;
};


typedef istd::TSharedInterfacePtr<IProcessorLog> IProcessorLogSharedPtr;


} // namespace imthype


