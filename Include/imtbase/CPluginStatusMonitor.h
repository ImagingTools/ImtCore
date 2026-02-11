// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IPluginStatusMonitor.h>
#include <imtbase/TAggergatedObjectCollectionWrap.h>

// ImtCore includes
#include <imtbase/CSimpleStatus.h>


namespace imtbase
{


class CPluginStatusMonitor:
			public TAggergatedObjectCollectionWrap<IPluginStatusMonitor, CSimpleStatus>
{
public:
	typedef TAggergatedObjectCollectionWrap<IPluginStatusMonitor, CSimpleStatus> BaseClass;

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
	virtual const istd::IInformationProvider* GetStatus(const QByteArray& pluginId) const override;

private:
	QMap<QByteArray, QByteArray> m_pluginTypeIds;
};


} // namespace imtbase


