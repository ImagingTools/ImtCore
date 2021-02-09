#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IPluginStatusMonitor.h>
#include <GeneratedFiles/imtgui/ui_CPluginStatusViewerComp.h>


namespace imtgui
{


class CPluginStatusViewerComp: public iqtgui::TDesignerGuiObserverCompBase<Ui::CPluginStatusViewerComp, imtbase::IPluginStatusMonitor>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CPluginStatusViewerComp, imtbase::IPluginStatusMonitor> BaseClass;

	I_BEGIN_COMPONENT(CPluginStatusViewerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
};


} // namespace imtgui


