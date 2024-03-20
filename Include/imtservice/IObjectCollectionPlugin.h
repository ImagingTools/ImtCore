#pragma once


// ImtCore includes
#include <imtbase/IPluginInfo.h>
#include <imtbase/IObjectCollection.h>


namespace imtservice
{


/**
	Interface describing an object collection Plug-In.
*/
class IObjectCollectionPlugin: virtual public imtbase::IPluginInfo
{
public:
	typedef istd::TIFactory<imtbase::IObjectCollection> IObjectCollectionFactory;

	/**
		Get access to the object collection factory.
	*/
	virtual const IObjectCollectionFactory* GetObjectCollectionFactory() const = 0;
};


} // namespace imtservice


