#pragma once


// ImtCore includes
#include <imtbase/IPluginInfo.h>
#include <imtbase/IRepresentationController.h>
#include <imtbase/IObjectCollection.h>


namespace imtservice
{


/**
	Interface describing a task Plug-In.
*/
class IObjectCollectionPlugin: virtual public imtbase::IPluginInfo
{
public:
	typedef istd::TIFactory<imtbase::IObjectCollection> IObjectCollectionFactory;

	/**
		Get access to the ObjectCollection factory.
	*/
	virtual const IObjectCollectionFactory* GetObjectCollectionFactory() const = 0;
};


} // namespace imtservice


