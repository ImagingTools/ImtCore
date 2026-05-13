// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IReferenceCollection.h>

// Acula includes
#include <imthype/IProcessingInfo.h>
#include <imthype/IJobExecutionLog.h>

// ACF includes
#include <ilog/IMessageContainer.h>


namespace imthype
{


/**
	Common interface for a job output. The job output is a collection of object references categorized by their logical type.
	The interface provides access to the processing information, execution log, and processor log.
*/
class IJobOutput: virtual public IProcessingInfo, virtual public imtbase::IReferenceCollection
{
public:
	/**
		Enumeration describing all possible types of processing output.
	*/
	enum ProcessingOutputType
	{
		/**
			The output is the result of the job processing.
		*/
		POT_RESULT,

		/**
			The output is a processing artifact.
			This can be a job log, some additional output, intermediate results, reports, diagnostics stuff, etc.
		*/
		POT_ARTIFACT
	};

	I_DECLARE_ENUM(ProcessingOutputType, POT_RESULT, POT_ARTIFACT);

	/**
		Set processing report.
	*/
	virtual void SetProcessingReport(
				const ilog::IMessageContainer& processingLog,
				const QDateTime& startTime,
				const QDateTime& finishTime) = 0;

	/**
		Get the type of processing output object.
	*/
	virtual ProcessingOutputType GetOutputType(const QByteArray& outputObjectId) const = 0;

	/**
		Set the type of processing output for the given output object.
	*/
	virtual void SetOutputType(const QByteArray& outputObjectId, ProcessingOutputType outputType) = 0;
	
	/**
		Get the job execution log (system-level controller events).
		\return Pointer to execution log, or nullptr if not set
	*/
	virtual const IJobExecutionLog* GetExecutionLog() const = 0;
	
	/**
		Set the job execution log.
		\param executionLog System-level execution log
	*/
	virtual void SetExecutionLog(const IJobExecutionLog& executionLog) = 0;
	
	/**
		Get the processor log (worker processing output).
		\return Pointer to processor log, or nullptr if not set
	*/
	virtual const ilog::IMessageContainer* GetProcessorLog() const = 0;
	
	/**
		Set the processor log.
		\param processorLog Worker processing log
	*/
	virtual void SetProcessorLog(const ilog::IMessageContainer& processorLog) = 0;
};


typedef istd::TUniqueInterfacePtr<IJobOutput> IJobOutputUniquePtr;
typedef istd::TSharedInterfacePtr<IJobOutput> IJobOutputSharedPtr;


} // namespace imthype


