#pragma once


// ACF includes
#include <iqtgui/TGuiObserverWrap.h>

// ImtCore includes
#include <imt3dgui/CGridShape.h>
#include <imt3dgui/CMeshShape.h>
#include <imt3dgui/CRulerShape.h>
#include <imt3dgui/CAxisShape.h>
#include <imt3dgui/CView3dProviderComp.h>


namespace imt3dgui
{


class CMeshViewComp: public iqtgui::TGuiObserverWrap<CView3dProviderComp, CMeshShape>
{
public:
	typedef iqtgui::TGuiObserverWrap<CView3dProviderComp, CMeshShape> BaseClass;

	I_BEGIN_COMPONENT(CMeshViewComp);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(imod::IModelEditor);
		I_ASSIGN(m_showInfoBoxAttrPtr, "ShowInfoBox", "Show info box if enabled", true, false);
		I_ASSIGN(m_defaultShowAxisAttrPtr, "ShowAxis", "If enabled, the axes are shown", true, true);
		I_ASSIGN(m_defaultShowGridAttrPtr, "ShowGrid", "If enabled, the grid is shown", true, true);
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;

	// reimplemented (imt3dgui::ISceneEventHandler
	void OnShowGrid(bool show) override;
	void OnShowAxis(bool show) override;
	void OnShowRuler(bool show) override;
	void OnPointSelection(const QPoint& point, bool clearPreviousSelection) override;
	void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) override;
	void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) override;
	void OnClearSelection() override;
	void OnAllSelection() override;
	void OnInvertSelection() override;
	void OnDeleteSelection() override;
	bool OnMousePress(QMouseEvent& e) override;
	bool OnMouseMove(QMouseEvent& e) override;
	bool OnMouseRelease(QMouseEvent& e) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	void OnRestoreSettings(const QSettings& settings) override;
	void OnSaveSettings(QSettings& settings) const override;

private:
	CGridShape m_gridShape;
	CAxisShape m_axisShape;
	CRulerShape m_rulerShape;

	I_ATTR(bool, m_showInfoBoxAttrPtr);
	I_ATTR(bool, m_defaultShowAxisAttrPtr);
	I_ATTR(bool, m_defaultShowGridAttrPtr);
};


} // namespace imt3dgui


