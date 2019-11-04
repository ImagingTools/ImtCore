#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <imod/CMultiModelDispatcherBase.h>

// Eagle includes
#include <eaglegui/CGridShape.h>
#include <eaglegui/CMeshShape.h>
#include <eaglegui/CEagleViewProviderComp.h>
#include <eaglegui/CAxisShape.h>
#include <eaglegui/CTargetPointerShape.h>


namespace eaglegui
{


class CMeshViewComp: public iqtgui::TGuiObserverWrap<CEagleViewProviderComp, CMeshShape>

{
public:
	typedef iqtgui::TGuiObserverWrap<CEagleViewProviderComp, CMeshShape> BaseClass;

	I_BEGIN_COMPONENT(CMeshViewComp);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(imod::IModelEditor);
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;

	// reimplemented (eaglegui::ISceneEventHandler
	void OnShowGrid(bool show) override;
	void OnShowAxis(bool show) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	void OnRestoreSettings(const QSettings& settings) override;
	void OnSaveSettings(QSettings& settings) const override;

private:
	CGridShape m_gridShape;
	CAxisShape m_axisShape;
};


} // namespace eaglegui


