#pragma once


// ACF includes
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IPluginInfo.h>
#include <imtbase/IRepresentationController.h>


namespace imtbase
{


/**
	Interface describing a task Plug-In.
*/
class IParamsSetPlugin: virtual public imtbase::IPluginInfo
{
public:
	typedef istd::TIFactory<iprm::IParamsSet> IParamsSetFactory;
	typedef istd::TIFactory<imtbase::IRepresentationController> IRepresentationControllerFactory;

	/**
		Get access to the task factory.
	*/
	virtual const IParamsSetFactory* GetParamsSetFactory() const = 0;
	virtual const IRepresentationControllerFactory* GetRepresentationControllerFactory() const = 0;
};


} // namespace imtbase


