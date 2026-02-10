// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtbase/IMultiStatusProvider.h>
#include <imtbase/IPluginInfo.h>


namespace imtbase
{


/**
	Interface for getting information about a Plug-In.
*/
class IPluginStatusMonitor: virtual public imtbase::IMultiStatusProvider
{
public:
	virtual QByteArray GetPluginTypeId(const QByteArray& id) const = 0;
	virtual void OnPluginStatusChanged(
				const QString& pluginPath,
				const QString& pluginName,
				const QByteArray& pluginTypeId,
				istd::IInformationProvider::InformationCategory category,
				const QString& statusMessage) = 0;
};


} // namespace imtbase


