#pragma once


// ImtCore includes
#include <imtbase/IReferenceCollection.h>

// Acula includes
#include <imthype/IProcessingInfo.h>


namespace imthype
{


/**
	Common interface for a job output. The job output is a collection of object references categorized by their logical type.
	The interface provides also the access to the processing informations and logs.
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
};


typedef istd::TUniqueInterfacePtr<IJobOutput> IJobOutputUniquePtr;
typedef istd::TSharedInterfacePtr<IJobOutput> IJobOutputSharedPtr;


} // namespace imthype


