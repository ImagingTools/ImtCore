#pragma once


// ACF includes
#include <istd/TIFactory.h>
#include <iproc/IProcessor.h>

// ImtCore includes
#include <imtbase/IReferenceCollection.h>


namespace imthype
{


class IJobOutput;


/**
	Interface for a synchronous job task processing.
*/
class IJobProcessor: virtual public iproc::IProcessor
{
public:
	/**
		Get the type-ID of the task.
	*/
	virtual QByteArray GetTaskTypeId() const = 0;

	/**
		Execute a job task.
		\param [in] input				List of IDs for identifying input objects in the storage
		\param [in] params				Processing parameters
		\param [out] output				Result of job processing. Will be filled by the method implementation.
		\param [out] progressManagerPtr	(Optionally) Progress manager used for receiving informaton about job progress and cancelation.
		\return processing result \sa iproc::IProcessor::TaskState
	*/
	virtual int ExecuteTask(
				const imtbase::IReferenceCollection& input,
				const iprm::IParamsSet& params,
				IJobOutput& output,
				ibase::IProgressManager* progressManagerPtr = nullptr) const = 0;
};


typedef istd::TIFactory<IJobProcessor> IJobProcessorFactory;


} // namespace imthype


