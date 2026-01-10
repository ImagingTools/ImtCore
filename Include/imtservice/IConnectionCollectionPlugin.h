#pragma once


// ImtCore includes
#include <imtbase/IPluginInfo.h>
#include <imtservice/IConnectionCollection.h>


namespace imtservice
{


/**
	Interface describing a task Plug-In.
*/
class IConnectionCollectionPlugin: virtual public imtbase::IPluginInfo
{
public:
	typedef istd::TIFactory<IConnectionCollection> IConnectionCollectionFactory;
	typedef istd::TUniqueInterfacePtr<IConnectionCollection> IConnectionCollectionUniquePtr;
	typedef istd::TSharedInterfacePtr<IConnectionCollection> IConnectionCollectionSharedPtr;

	/**
		Get access to the ConnectionCollection factory.
	*/
	virtual const IConnectionCollectionFactory* GetConnectionCollectionFactory() const = 0;
};


} // namespace imtservice


