// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		I_ASSIGN(m_defaultShowAxisAttrPtr, "ShowAxis", "If enabled, the axes are shown", true, true);
		I_ASSIGN(m_defaultShowGridAttrPtr, "ShowGrid", "If enabled, the grid is shown", true, true);
		I_ASSIGN(m_xRangeMinAttrPtr, "XRangeMin", "Minimum value on the X-axis", true, 0.0);
		I_ASSIGN(m_xRangeMaxAttrPtr, "XRangeMax", "Maximum value on the X-axis", true, 1.0);
		I_ASSIGN(m_yRangeMinAttrPtr, "YRangeMin", "Minimum value on the Y-axis", true, 0.0);
		I_ASSIGN(m_yRangeMaxAttrPtr, "YRangeMax", "Maximum value on the Y-axis", true, 1.0);
		I_ASSIGN(m_zRangeMinAttrPtr, "ZRangeMin", "Minimum value on the Z-axis", true, 0.0);
		I_ASSIGN(m_zRangeMaxAttrPtr, "ZRangeMax", "Maximum value on the Z-axis", true, 1.0);
		I_ASSIGN(m_axisLineLengthAttrPtr, "AxisLengthWidth", "Width of the axis line", true, 4.0);
		I_ASSIGN(m_xAxisLabelAttrPtr, "XAxisLabel", "Label of the X-axis", true, "X");
		I_ASSIGN(m_yAxisLabelAttrPtr, "YAxisLabel", "Label of the Y-axis", true, "Y");
		I_ASSIGN(m_zAxisLabelAttrPtr, "ZAxisLabel", "Label of the Z-axis", true, "Z");
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (imt3dgui::ISceneEventHandler)
	virtual void OnShowGrid(bool show) override;
	virtual void OnShowAxis(bool show) override;
	virtual void OnShowRuler(bool show) override;
	virtual void OnPointSelection(const QPoint& point, bool clearPreviousSelection) override;
	virtual void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) override;
	virtual void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) override;
	virtual void OnClearSelection() override;
	virtual void OnAllSelection() override;
	virtual void OnInvertSelection() override;
	virtual void OnDeleteSelection() override;
	virtual bool OnMousePress(QMouseEvent& e) override;
	virtual bool OnMouseMove(QMouseEvent& e) override;
	virtual bool OnMouseRelease(QMouseEvent& e) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

protected:
	virtual void SetupGridShape(CGridShape& gridShape) const;
	virtual void SetupAxisShape(CAxisShape& axisShape) const;
	virtual void SetupRulerShape(CRulerShape& rulerShape) const;

private:
	CGridShape m_gridShape;
	CAxisShape m_axisShape;
	CRulerShape m_rulerShape;

	I_ATTR(double, m_pointSizeAttrPtr);
	I_ATTR(bool, m_showInfoBoxAttrPtr);
	I_ATTR(bool, m_defaultShowAxisAttrPtr);
	I_ATTR(bool, m_defaultShowGridAttrPtr);
	I_ATTR(double, m_xRangeMinAttrPtr);
	I_ATTR(double, m_xRangeMaxAttrPtr);
	I_ATTR(double, m_yRangeMinAttrPtr);
	I_ATTR(double, m_yRangeMaxAttrPtr);
	I_ATTR(double, m_zRangeMinAttrPtr);
	I_ATTR(double, m_zRangeMaxAttrPtr);
	I_ATTR(double, m_axisLineLengthAttrPtr);
	I_TEXTATTR(m_xAxisLabelAttrPtr);
	I_TEXTATTR(m_yAxisLabelAttrPtr);
	I_TEXTATTR(m_zAxisLabelAttrPtr);
};


} // namespace imt3dgui


