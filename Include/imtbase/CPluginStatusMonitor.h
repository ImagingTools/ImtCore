#pragma once


// ImtCore includes
#include <imtbase/IPluginStatusMonitor.h>
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


class CPluginStatusMonitor: virtual public istd::IChangeable, virtual public IPluginStatusMonitor
{
public:
	CPluginStatusMonitor();

	// reimplemented (imtbase::IPluginMonitor)
	virtual QByteArray GetPluginTypeId(const QByteArray& id) const override;
	virtual void OnPluginStatusChanged(
				const QString& pluginPath,
				const QString& pluginName,
				const QByteArray& pluginTypeId,
				istd::IInformationProvider::InformationCategory category,
				const QString& statusMessage) override;

	// reimplemented (imtbase::IMultiStatusProvider)
	virtual const imtbase::ICollectionInfo& GetStatusList() const override;
	virtual const istd::IInformationProvider* GetStatus(const QByteArray& statusId) const override;

private:
	CObjectCollection m_collection;
	QMap<QByteArray, QByteArray> m_pluginTypeIds;
};


} // namespace imtbase


