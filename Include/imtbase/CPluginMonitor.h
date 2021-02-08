#pragma once


// ImtCore includes
#include <imtbase/IPluginMonitor.h>
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CSimpleStatus.h>


namespace imtbase
{


class CPluginMonitor: public TAggergatedObjectCollectionWrap<IPluginMonitor, CSimpleStatus>
{
public:
	typedef TAggergatedObjectCollectionWrap<IPluginMonitor, CSimpleStatus> BaseClass;

	CPluginMonitor();

	// reimplemented (imtbase::IPluginMonitor)
	virtual const imtbase::ICollectionInfo& GetPluginList() const override;
	virtual const istd::IInformationProvider* GetPluginStatus(const QByteArray& pluginId) const override;
};


} // namespace imtbase


