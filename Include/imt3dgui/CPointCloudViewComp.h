#pragma once


// ACF includes
#include <iqtgui/TGuiObserverWrap.h>

// ImtCore includes
#include <imt3dgui/CGridShape.h>
#include <imt3dgui/CPointCloudShape.h>
#include <imt3dgui/CView3dProviderComp.h>
#include <imt3dgui/CAxisShape.h>
#include <imt3dgui/CRulerShape.h>


namespace imt3dgui
{


class CPointCloudViewComp: public iqtgui::TGuiObserverWrap<CView3dProviderComp, CPointCloudShape>
{
	Q_OBJECT

public:
	typedef iqtgui::TGuiObserverWrap<CView3dProviderComp, CPointCloudShape> BaseClass;

	I_BEGIN_COMPONENT(CPointCloudViewComp);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(imod::IModelEditor);
		I_ASSIGN(m_pointSizeAttrPtr, "PointSize", "Size of the point used for rendering of the 3D-points in the cloud view", true, 3.0);
		I_ASSIGN(m_showInfoBoxAttrPtr, "ShowInfoBox", "Show info box if enabled", true, false);
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;

	// reimplemented (imt3dgui::ISceneEventHandler)
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

	I_ATTR(double, m_pointSizeAttrPtr);
	I_ATTR(bool, m_showInfoBoxAttrPtr);
};


} // namespace imt3dgui


