#include <imtbase/CPluginMonitor.h>


// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// public methods

CPluginMonitor::CPluginMonitor()
	:BaseClass("PluginStatus", "Plugin status", "PluginMonitor")
{
}


// reimplemented (imtbase::IPluginMonitor)

const imtbase::ICollectionInfo& CPluginMonitor::GetPluginList() const
{
	return m_collection;
}


const istd::IInformationProvider* CPluginMonitor::GetPluginStatus(const QByteArray& pluginId) const
{
	return dynamic_cast<const istd::IInformationProvider*>(m_collection.GetObjectPtr(pluginId));
}


} // namespace imtbase


