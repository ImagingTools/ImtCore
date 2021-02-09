#include <imtbase/CPluginStatusMonitor.h>


// ACF includes
#include <iser/IArchive.h>
#include <imod/TModelWrap.h>
#include <istd/TSingleFactory.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CPluginStatusMonitor::CPluginStatusMonitor()
	:BaseClass("PluginStatus", "Plugin status collection", "PluginStatusCollection")
{
}


// reimplemented (imtbase::IPluginMonitor)

QByteArray CPluginStatusMonitor::GetPluginTypeId(const QByteArray& id) const
{
	if (m_pluginTypeIds.contains(id)){
		return m_pluginTypeIds[id];
	}

	return QByteArray();
}


void CPluginStatusMonitor::OnPluginStatusChanged(
			const QString& pluginPath,
			const QString& pluginName,
			const QByteArray& pluginTypeId,
			istd::IInformationProvider::InformationCategory category,
			const QString& statusMessage)
{
	istd::CChangeNotifier notifier(this);

	CSimpleStatus status;
	status.SetInformationId(0);
	status.SetInformationSource(pluginPath);
	status.SetInformationCategory(category);
	status.SetInformationDescription(statusMessage);
	status.SetInformationTimeStamp(QDateTime::currentDateTime());

	QByteArray pluginId;

	imtbase::ICollectionInfo::Ids ids = GetElementIds();
	for (QByteArray& id : ids){
		istd::IInformationProvider* informationProviderPtr = dynamic_cast<istd::IInformationProvider*>(
					const_cast<istd::IChangeable*>(GetObjectPtr(id)));

		Q_ASSERT(informationProviderPtr != nullptr);

		if (informationProviderPtr->GetInformationSource() == pluginPath){
			pluginId = id;
			break;
		}
	}

	if (pluginId.isEmpty()){
		pluginId = InsertNewObject(
					"PluginStatus",
					pluginName,
					"",
					&status);
	}

	if (!pluginId.isEmpty()){
		m_pluginTypeIds[pluginId] = pluginTypeId;
	}
}


// reimplemented (imtbase::IPluginMonitor)

const imtbase::ICollectionInfo& CPluginStatusMonitor::GetStatusList() const
{
	return *this;
}


const istd::IInformationProvider* CPluginStatusMonitor::GetStatus(const QByteArray& pluginId) const
{
	return dynamic_cast<const istd::IInformationProvider*>(GetObjectPtr(pluginId));
}


} // namespace imtbase


