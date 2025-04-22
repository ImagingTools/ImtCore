#pragma once


// ACF includes
#include <istd/TIFactory.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IPluginInfo.h>


namespace imthype
{


/**
	Interface describing a task parameter Plug-In.
*/
class ITaskParamsPlugin : virtual public imtbase::IPluginInfo
{
public:
	typedef istd::TIFactory<iprm::IParamsSet> IParamSetFactory;
	
	/**
		Get access to the factory of the task parameter set.
	*/
	virtual const IParamSetFactory* GetTaskParamsFactory() const = 0;
};


} // namespace imthype


