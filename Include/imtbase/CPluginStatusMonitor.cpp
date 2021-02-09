#include <imtbase/CPluginStatusMonitor.h>


// ACF includes
#include <iser/IArchive.h>
#include <imod/TModelWrap.h>
#include <istd/TSingleFactory.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/CSimpleStatus.h>


namespace imtbase
{


// public methods

	CPluginStatusMonitor::CPluginStatusMonitor()
{
	m_collection.RegisterFactory(new istd::TSingleFactory<istd::IChangeable, imod::TModelWrap<CSimpleStatus>>("PluginStatus"), true);
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
	status.SetInformationTimeStamp(QDateTime::currentDateTime());

	QByteArray pluginId;

	imtbase::ICollectionInfo::Ids ids = m_collection.GetElementIds();
	for (QByteArray& id : ids){
		istd::IInformationProvider* informationProviderPtr = dynamic_cast<istd::IInformationProvider*>(
					const_cast<istd::IChangeable*>(m_collection.GetObjectPtr(id)));

		Q_ASSERT(informationProviderPtr != nullptr);

		if (informationProviderPtr->GetInformationSource() == pluginPath){
			pluginId = id;
			break;
		}
	}

	if (pluginId.isEmpty()){
		pluginId = m_collection.InsertNewObject(
					"PluginStatus",
					pluginName,
					statusMessage,
					&status);

		Q_ASSERT(pluginId.isEmpty());
	}
	else{
		Q_ASSERT(const_cast<istd::IChangeable*>(m_collection.GetObjectPtr(pluginId))->CopyFrom(status));
	}

	m_pluginTypeIds[pluginId] = pluginTypeId;
}


// reimplemented (imtbase::IPluginMonitor)

const imtbase::ICollectionInfo& CPluginStatusMonitor::GetStatusList() const
{
	return m_collection;
}


const istd::IInformationProvider* CPluginStatusMonitor::GetStatus(const QByteArray& pluginId) const
{
	return dynamic_cast<const istd::IInformationProvider*>(m_collection.GetObjectPtr(pluginId));
}


} // namespace imtbase


