#pragma once


// ImtCore includes
#include <imtbase/IPluginInfo.h>
#include <imtserverapp/IRepresentationController.h>
#include <imtservice/IConnectionCollection.h>


namespace imtservice
{


/**
	Interface describing a task Plug-In.
*/
class IConnectionCollectionPlugin: virtual public imtbase::IPluginInfo
{
public:
	typedef istd::TIFactory<imtservice::IConnectionCollection> IConnectionCollectionFactory;
	typedef istd::TUniqueInterfacePtr<imtservice::IConnectionCollection> IConnectionCollectionUniquePtr;
	typedef istd::TSharedInterfacePtr<imtservice::IConnectionCollection> IConnectionCollectionSharedPtr;

	/**
		Get access to the ConnectionCollection factory.
	*/
	virtual const IConnectionCollectionFactory* GetConnectionCollectionFactory() const = 0;
};


} // namespace imtservice


