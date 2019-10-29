#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Interface for getting information about a Plug-In.
*/
class IPlugInInfo: virtual public istd::IPolymorphic
{
public:
	/**
		Get the name of the Plug-In.
	*/
	virtual QString GetPluginName() const = 0;
};


} // namespace imtbase


