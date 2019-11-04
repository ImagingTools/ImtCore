#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtgui3d/CGridShape.h>
#include <imtgui3d/CMeshShape.h>
#include <imtgui3d/CView3dProviderComp.h>
#include <imtgui3d/CAxisShape.h>
#include <imtgui3d/CTargetPointerShape.h>


namespace imtgui3d
{


class CMeshViewComp: public iqtgui::TGuiObserverWrap<CView3dProviderComp, CMeshShape>

{
public:
	typedef iqtgui::TGuiObserverWrap<CView3dProviderComp, CMeshShape> BaseClass;

	I_BEGIN_COMPONENT(CMeshViewComp);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(imod::IModelEditor);
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;

	// reimplemented (imtgui3d::ISceneEventHandler
	void OnShowGrid(bool show) override;
	void OnShowAxis(bool show) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	void OnRestoreSettings(const QSettings& settings) override;
	void OnSaveSettings(QSettings& settings) const override;

private:
	CGridShape m_gridShape;
	CAxisShape m_axisShape;
};


} // namespace imtgui3d


