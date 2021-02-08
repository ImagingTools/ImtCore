#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


/**
	Interface for getting information about a Plug-In.
*/
class IPluginMonitor: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetPluginList() const = 0;
	virtual const istd::IInformationProvider* GetPluginStatus(const QByteArray& pluginId) const = 0;
};


} // namespace imtbase


