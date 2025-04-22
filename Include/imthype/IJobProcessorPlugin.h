#pragma once


// ImtCore includes
#include <imtbase/IPluginInfo.h>

// Acula includes
#include <imthype/IJobProcessor.h>


namespace imthype
{


/**
	Interface for a job processor Plug-In.
*/
class IJobProcessorPlugin: virtual public imtbase::IPluginInfo
{
public:
	/**
		Get access to the job processor factory.
	*/
	virtual const imthype::IJobProcessorFactory* GetJobProcessorFactory() const = 0;
};


} // namespace imthype


