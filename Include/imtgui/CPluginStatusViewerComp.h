#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IPluginMonitor.h>
#include <GeneratedFiles/imtgui/ui_CPluginStatusViewerComp.h>


namespace imtgui
{


class CPluginStatusViewerComp: public iqtgui::TDesignerGuiObserverCompBase<Ui::CPluginStatusViewerComp, imtbase::IPluginMonitor>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CPluginStatusViewerComp, imtbase::IPluginMonitor> BaseClass;

	I_BEGIN_COMPONENT(CPluginStatusViewerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
};


} // namespace imtgui


